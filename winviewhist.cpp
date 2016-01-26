#include "winviewhist.h"
#include "ui_winviewhist.h"

WinViewHist::WinViewHist(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WinViewHist)
{
    ui->setupUi(this);

    create_results_folder();

    set_view_clean_btn();

}

WinViewHist::~WinViewHist()
{
    delete ui;
}

void WinViewHist::on_btn_cancel_clicked()
{
    this->close();
}

void WinViewHist::set_view_clean_btn()
{
    create_results_folder();
    QVector <QString> list_test = get_list_test();
    bool enabled = (list_test.length() == 0) ;

    ui->btn_clean->setEnabled(enabled);
    ui->btn_view->setEnabled(enabled);

    return;
}

void WinViewHist::on_btn_clean_clicked()
{
    clean_result_test();
    ui->btn_clean->setEnabled(false);
    ui->btn_view->setEnabled(false);

    /*Eliminare voci nella combobox*/
    while (ui->cmb_list_test->count() > 1)
        ui->cmb_list_test->removeItem(ui->cmb_list_test->count() - 1);

    return;
}

QString parsing_name_file(const QString& qt_file_name)
{
    char name_file[11];
    strcpy(name_file, qt_file_name.toStdString().c_str());

    QString caption = "";

    switch (name_file[0])
    {
        case 'A':
            caption += "AGGR ";
            break;
        case 'S':
            caption = "STARTUP ";
            break;
    }

    if (name_file[2] == 'S') caption+= "SEQ ";
    else
        switch (name_file[3])
        {
        case 'A':
            caption+= "RAND ";
            break;
        case 'R':
            caption+= "RAW RAND ";
            break;
        case 'S':
            caption+= "RAW SEQ ";
            break;
        default:
            break;
        }

    caption+= "READ: " + QString::number(name_file[4]+name_file[5]) + " ";
    caption+= "WRITE: " + QString::number(name_file[6]+name_file[7])+ " ";
    if (name_file[0] == 'S') caption += "IITERATION: " +
                            QString::number(name_file[8]+name_file[9]);

    return caption;
}

void WinViewHist::update_combobox()
{
    QVector <QString> list_test = get_list_test();

    for (int i = 0; i<list_test.length(); i++)
        if (ui->cmb_list_test->findText(parsing_name_file(list_test[i]),
                                        static_cast<Qt::MatchFlags>(Qt::MatchContains)) != -1)
            ui->cmb_list_test->addItem(parsing_name_file(list_test[i]));

    return;
}

void WinViewHist::update_checkbox(const QString& name_file,const QString& item_combobox)
{
     if (parsing_name_file(name_file) == item_combobox)
     {
         char sched = name_file[0].toLatin1();

         switch (sched) {
         case 'B':
             ui->chk_bfq->setEnabled(true);
             break;
         case 'C':
             ui->chk_cfq->setEnabled(true);
             break;
         case 'N':
             ui->chk_noop->setEnabled(true);
             break;
         case 'D':
             ui->chk_deadline->setEnabled(true);
             break;
         default:
             break;
         }
     }

     return;
}

void WinViewHist::on_cmb_list_test_currentIndexChanged(const QString &arg1)
{
    QVector <QString> list_test = get_list_test();

    ui->chk_bfq->setEnabled(false);
    ui->chk_cfq->setEnabled(false);
    ui->chk_deadline->setEnabled(false);
    ui->chk_noop->setEnabled(false);

    for (int i = 0; i<list_test.length(); i++)
        update_checkbox(list_test[i], arg1);

    return;
}

bool WinViewHist::checked_sched_to_show(const QString& sched)
{

    switch (sched.toStdString().c_str()[0]) {
    case 'B':
        return ui->chk_bfq->isChecked();
    case 'C':
        return ui->chk_cfq->isChecked();
    case 'D':
        return ui->chk_deadline->isChecked();
    case 'N':
        return ui->chk_noop->isChecked();
    default:
        break;
    }

    return false;
}

void WinViewHist::on_btn_view_clicked()
{
    QVector <QString> list_test = get_list_test();
    QVector <sched_result_t> results;

    for (int i = 0; i<list_test.length(); i++)
        if (ui->cmb_list_test->currentText() == parsing_name_file(list_test[i])
            && checked_sched_to_show(list_test[i]))
        {
            /* Leggi file giusto e salvare le statistiche */
            results<<get_stats_to_draw(list_test[i]);
        }

    /* Mostrare i grafici selezionati nel vettore results */

    return;
}
