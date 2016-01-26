#include "wintestplot.h"
#include "ui_wintestplot.h"
#include "utilities.h"

WinTestPlot::WinTestPlot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WinTestPlot)
{
    ui->setupUi(this);
    saveDefaultLabel();
}

WinTestPlot::~WinTestPlot()
{
    delete ui;
}

void WinTestPlot::saveDefaultLabel()
{
    n_read=ui->lbl_n_reads->text();
    n_write=ui->lbl_b_writes->text();
    rw_type=ui->lbl_rw_type->text();
}

void WinTestPlot::loadDefaultLabel()
{
    ui->lbl_rw_type->setText("RW Type:");
    ui->lbl_n_reads->setText("N° Read:");
    ui->lbl_b_writes->setText("N° Write:");
    avgData.clear();
}

void WinTestPlot::setupLables(const test_settings_t &test)
{
    QString test_type;
    get_string_from_test(&test_type,test);

    ui->lbl_rw_type->setText(this->rw_type+" "+ test_type);
    ui->lbl_n_reads->setText(this->n_read +" " +QString::number(test.n_read));
    ui->lbl_b_writes->setText(this->n_write +" "+ QString::number(test.n_write));
    if(test.type_test== AGGR)
        ui->lbl_type_test->setText("Average throughput");
    else
        ui->lbl_type_test->setText("Average latency");

}

bool WinTestPlot::prepareForShowing(const test_settings_t& test)
{
    if(!this->loadData(test))
        return false;

    setupAllPlotsAfterTest(avgData);
    setupLables(test);
    return true;
}

void WinTestPlot::on_pushButton_clicked()
{
    ui->customPlot->clearPlottables();
    loadDefaultLabel();
    this->close();
}


bool WinTestPlot::loadData(const test_settings_t& test)
{
    bool file=get_stat_from_file(avgData,test.type_test);
    if (!file || avgData[0] == ERROR_FEW_DATA)
    {
        QString error_message;
        if(!file)
            error_message="File not found";
        else
            error_message="There are too few data!";

        QMessageBox message(QMessageBox::Warning, "Error",
                                               error_message,
                                               QMessageBox::Ok ,
                                               QDesktopWidget().screen());
        message.exec();

          return false;
    }else
        return file;
}

bool WinTestPlot::setupAllPlotsAfterTest(const QVector<double> data)
{
    /*ASSUMO CHE I VALORI SIANO DIPOSTI IN QUESTO MODO:
    * POSIZIONE         VALORE
    * 0                         BFQ
    * 1                         CFQ
    * 2                         NOOP
    * 3                         DEADLINE
    */
    if(data.length() > 4)
        return false;
    this->setupPlotAfterTest(ui->customPlot,data);

    ui->customPlot->replot();

     return true;
}

bool WinTestPlot::setData(QVector<sched_result_t> result, QString testo, type_test_t type)
{
    //qDebug()<<result[0].sched<<result.length();
    if(result[0].sched != BFQ || result.length() > 5)
        return false;

    if(type == AGGR)
    {
       avgData<<result[0].throug<<result[1].throug<<result[2].throug<<result[3].throug;
       ui->lbl_type_test->setText("Average throughput");
    }else
    {
       avgData<<result[0].lat<<result[1].lat<<result[2].lat<<result[3].lat;
       ui->lbl_type_test->setText("Average latency");
    }
    //remove data and get a QString of it
    QString date=QString("Date: "+testo.left(19));
    testo.remove(0,19);
    //get iteration if exists
    QString iteration="";
    if(type==STARTUP)
    {
        iteration=testo.right(13);
        testo.remove(iteration);
    }
    ui->lbl_compare->setText(QString(testo + "\n" + date+"\t\t"+iteration));
    ui->lbl_n_reads->setText("");
    ui->lbl_b_writes->setText("");
    ui->lbl_rw_type->setText("");
    return true;
}

bool WinTestPlot::setGuiAfterData()
{
    return this->setupAllPlotsAfterTest(avgData);
}

void WinTestPlot::setupPlotAfterTest(QCustomPlot *customPlot,const QVector<double> data)
{

    QCPBars *avg = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(avg);
    // set names and colors:
    QPen pen;
    pen.setWidthF(1.5);
    pen.setColor(QColor(255, 131, 0));

    //Setting avg Bar
    avg->setName("Average:");
    avg->setPen(pen);
    avg->setBrush(QColor(255, 131, 0, 50));

    // prepare x axis with country labels:
    QVector<double> ticks;
    QVector<QString> labels;
    /*latenza throughput read write*/

    labels<<"BFQ"<<"CFQ"<<"NOOP"<<"DEADLINE";
    ticks<<1<<3<<5<<7;

    double max_value=get_max_from_data(data);
    customPlot->yAxis->setRange(0,max_value+5);

    /*Impostazione asse X*/
    customPlot->xAxis->setAutoTicks(false);
    customPlot->xAxis->setAutoTickStep(false);
    customPlot->xAxis->setTickStep(200);
    customPlot->xAxis->setAutoTickLabels(false);
    customPlot->xAxis->setTickVector(ticks);
    customPlot->xAxis->setTickVectorLabels(labels);
    customPlot->xAxis->setTickLabelRotation(0);
    customPlot->xAxis->setTickLength(1, 3);
    customPlot->xAxis->grid()->setVisible(true);
    customPlot->xAxis->setRange(0, 8);

    /*Impostazione asse Y*/
    customPlot->yAxis->setPadding(2);
    customPlot->yAxis->grid()->setSubGridVisible(true);
    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    customPlot->yAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    customPlot->yAxis->grid()->setSubGridPen(gridPen);

    /*Caricamento dati*/
    avg->setData(ticks, data);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal);
}


void WinTestPlot::closeEvent(QCloseEvent *event)
{
    on_pushButton_clicked();
    event->accept();
}
