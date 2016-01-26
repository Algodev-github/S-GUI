#include "winsettings.h"
#include "ui_winsettings.h"

#include <QDebug>
#include <fstream>
#include <QMessageBox>

extern test_settings_t test_sets;
extern advanced_settings_t adv;
extern QString dir_bench;


const int NUM_REQUIRED_FILE = 7;

const int MIN_rate = 10000;
const int MAX_rate = 20000;


/**
 * @brief load_requried_files load the array with the pathes of required files.
 * @param dir : path of folder where search the files.
 * @return a QString * with the pathes where to search the required files.
 */
static QString * load_requried_files(const QString& dir)
{
    QString * arr_files = new QString[NUM_REQUIRED_FILE];

    arr_files[0] = dir + "agg_thr-with-greedy_rw/aggthr-with-greedy_rw.sh";
    arr_files[1] = dir + "comm_startup_lat/comm_startup_lat.sh";
    arr_files[2] = dir + "config_params.sh";
    arr_files[3] = dir + "utilities/calc_overall_stats.sh";
    arr_files[4] = dir + "utilities/lib_utils.sh";
    arr_files[5] = dir + "utilities/calc_avg_and_co.sh";
    arr_files[6] = dir + "run_multiple_tests/run_all_tests_1.sh";

    return arr_files;
}

/**
 * @brief exist_required_files check if there are the required files
 *                              in the new selected folder.
 * @param dir : path of folder where search the files.
 * @return true if there are required files, false there not.
 */
static bool exist_required_files(const QString& dir)
{
    QString * arr_requrired_files = load_requried_files(dir);

    for (int i = 0; i < NUM_REQUIRED_FILE; i++)
        if(!check_if_file_exists(arr_requrired_files[i]))
        {
            qDebug()<<arr_requrired_files[i]<<" non esiste";
            return false;
        }

    return true;
}



WinSettings::WinSettings(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WinSettings)
{

    ui->setupUi(this);
    user_has_saved=true;
    ui->txt_test_time->setText(QString::number(test_sets.adv_settings.test_time));
    ui->txt_max_startup->setText(QString::number(test_sets.adv_settings.max_startup));

    ui->txt_max_rate->setText(QString::number(test_sets.adv_settings.max_rate));
    QString desc_rate = "Min value is:\t"+QString::number(MIN_rate)+"\nMax value is:\t"+QString::number(MAX_rate);
    ui->lbl_max_rate_desc->setText(desc_rate);

    ui->line_bench_dir->setText(dir_bench);
    ui->line_base_dir->setText(test_sets.adv_settings.dir_test);
    ui->chk_sync->setChecked(test_sets.adv_settings.sync);

    /*Connect all the browse's buttons*/
    connect(ui->btn_search_base_dir,SIGNAL(clicked()),this,SLOT(browse_button_clicked()));
    connect(ui->btn_search_bench_dir,SIGNAL(clicked()),this,SLOT(browse_button_clicked()));

}

WinSettings::~WinSettings()
{
    delete ui;
}

void WinSettings::browse_button_clicked()
{
    QString dir = get_dir(this);

    /*Check if the path is correct. If dir == "", the cancel button was pressed*/
    if(dir != "")
    {
        /* Get the index from the button(Sender)*/
        int id = sender()->property("index").toInt();

        /*Set the correct lineText*/
        switch (id) {
        case BENCH_DIR:

            /* Check if all the required files */
            if (!exist_required_files(dir))
            {
                QString * arr_required_files = load_requried_files("");
                QString message_error = "There aren't required files in selected folder. \n They are: \n";

                for (int i = 0; i<NUM_REQUIRED_FILE; i++)
                    message_error +=  arr_required_files[i] + '\n';

                QMessageBox::warning(this,"ERROR", message_error, QMessageBox::Ok);
                return;
            }
            //Control passed, can change the dir_bench and the text
            dir_bench=dir;
            ui->line_bench_dir->setText(dir_bench);
            user_has_saved=false;
            //update BASE_DIR and Size of file to use
            ui->txt_file_size->setText(read_value_from_file(NUM_BLOCKS_CREATE_SEQ_M));
            ui->line_base_dir->setText(read_value_from_file(BASE_DIR_M));
            break;

        case BASE_DIR:
            if (dir != "")
                //delete the "/"
                dir.truncate(dir.length()-1);
                ui->line_base_dir->setText(dir);
                user_has_saved=false;
            break;

        default:
            break;
        }

    }
}


void WinSettings::on_btn_default_clicked()
{
    const advanced_settings_t default_adv;

    ui->txt_test_time->setText(QString::number(default_adv.test_time));
    ui->txt_max_startup->setText(QString::number(default_adv.max_startup));
    ui->txt_max_rate->setText(QString::number(default_adv.max_rate));
    ui->line_bench_dir->setText(default_adv.default_dir_bench);
    ui->line_base_dir->setText(default_adv.dir_test);
    ui->chk_sync->setChecked(default_adv.sync);

    /*Set reads/write numbers to default--> 1*/
    ui->slc_5->setText("1");
    ui->slc_6->setText("1");
    ui->txt_file_size->setText("500");
    user_has_saved=false;
    return;

}


void WinSettings::on_btn_cancel_clicked()
{
    this->close();
}

bool WinSettings::correct_input()
{

    if (!ui->txt_max_rate->text().toInt()
        || ui->txt_max_rate->text().toInt() < MIN_rate
        || ui->txt_max_rate->text().toInt() > MAX_rate)
    {
        QMessageBox::warning(this,"ERROR", "Max rate value not correct", QMessageBox::Ok);
        return false;
    }

    if (!ui->txt_max_startup->text().toInt() && ui->txt_test_time->text().toInt() < 0)
    {
        QMessageBox::warning(this,"ERROR", "Max startup time value not correct", QMessageBox::Ok);
        return false;
    }

    if (!ui->txt_test_time->text().toInt() && ui->txt_test_time->text().toInt() < 0)
    {
        QMessageBox::warning(this,"ERROR", "Test time value not correct", QMessageBox::Ok);
        return false;
    }

    //remove possibile whitespace at the begin and at the end of the string
    ui->txt_file_size->setText(ui->txt_file_size->text().trimmed());
    //control that are all digits
    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (!re.exactMatch(ui->txt_file_size->text()))
    {
        QMessageBox::warning(this,"ERROR...","File size must contain only digits without whitspaces",QMessageBox::Ok);
        return false;
    }
    return true;

}

void WinSettings::on_btn_save_clicked()
{
#define adv test_sets.adv_settings

    if (!correct_input())
        return;
    if (!update_config_file(ui->line_base_dir->text(),BASE_DIR_M))
        exit(44);
    if (!update_config_file(ui->txt_file_size->text(), NUM_BLOCKS_CREATE_SEQ_M))
        exit(45);

    adv.dir_test = ui->line_base_dir->text();
    adv.max_rate = static_cast<unsigned int>(ui->txt_max_rate->text().toInt());
    adv.test_time = static_cast<unsigned int>(ui->txt_test_time->text().toInt());
    adv.max_startup = static_cast<unsigned int>(ui->txt_max_startup->text().toInt());
    adv.sync = ui->chk_sync->isChecked();

    set_test_parameters();
    QString command;
    set_basic_test(test_sets,command);
    user_has_saved=true;

    this->close();
#undef adv
    return;
}

void  WinSettings::set_test_parameters()
{
    test_sets.type_command = static_cast<type_command_t>(ui->slc_1->currentIndex());
    test_sets.type_rw = static_cast<type_rw_t>(ui->slc_2->currentIndex());

    test_sets.n_iteration = ui->slc_3->text().toInt();
    test_sets.n_read = ui->slc_5->text().toInt();
    test_sets.n_write = ui->slc_6->text().toInt();
}



/* Funzioni che abilitano/disabilitano il pulsante per salvare:
 *
 * bit 0: read
 * bit 1: write
 * bit 2: iteration
 *
 * unsigned int mask_enabled_save = 0;
 *
 * Quando selezione read e write fisse la metto a 3 (evento della combobox).
 * Devo passare alla finestra il tipo di test che si vuole effettuare, in modo
 * da capire se devo considerare o meno le iterazioni.
 *
 * Devo ricordarmi le impostazioni appena salvate (esiste già una struct globale?)
 * in modo da abilitare/disabilitare read/write quando apro la finestra.
 * Cambiare le struct delle impostazioni e impostazioni avanzate?
 *
 * Rifare meglio l'interfaccia principale, e accorpare le due schede delle
 * impostazioni: dove seleziono le cartelle e dove setto i parametri.
 *
 *
 *
 * pulsante di save settings
 */

void WinSettings::set_btn_save(){

    if ((mask_enabled_save & ~(1<<2)) != 0 && test_sets.type_test == AGGR)
    {
        ui->btn_save->setEnabled(true);
        return;
    }

    if (mask_enabled_save > 4 && test_sets.type_test == STARTUP)
    {
        ui->btn_save->setEnabled(true);
        return;
    }

    ui->btn_save->setEnabled(false);
    return;

}



/* read */
void WinSettings::on_slc_5_textChanged(const QString &arg1)
{
    if (arg1.toInt() <= 0 || arg1 == NULL)
        mask_enabled_save &= ~1;
    else
        mask_enabled_save |= 1;

    set_btn_save();
}

/* write */
void WinSettings::on_slc_6_textChanged(const QString &arg1)
{
    if (arg1.toInt() <= 0 || arg1 == NULL)
        mask_enabled_save &= ~(1<<1);
    else
        mask_enabled_save |= (1<<1);

    set_btn_save();
}

void WinSettings::on_slc_3_textChanged(const QString &arg1)
{
    if (arg1.toInt() <= 0 || arg1 == NULL)
        mask_enabled_save &= ~(1<<2);
    else
        mask_enabled_save |= (1<<2);

    set_btn_save();
}


void WinSettings::update_advanced_interface()
{
    ui->slc_5->setText(QString::number(test_sets.n_read));
    ui->slc_6->setText(QString::number(test_sets.n_write));
    ui->slc_3->setText(QString::number((test_sets.n_iteration)));
    ui->slc_2->setCurrentIndex(test_sets.type_rw);
    ui->slc_3->setEnabled(true);
    ui->slc_1->setEnabled(true);

    if (test_sets.type_test != STARTUP)
    {
        ui->slc_1->setEnabled(false);
        ui->slc_3->setEnabled(false);
    }

    ui->txt_file_size->setText(read_value_from_file(NUM_BLOCKS_CREATE_SEQ_M));
    ui->line_base_dir->setText(read_value_from_file(BASE_DIR_M));
    ui->line_bench_dir->setText(dir_bench);
    pre_txt_size=ui->txt_file_size->text();
    pre_bench_dir=dir_bench;

    return;
}
void WinSettings::on_slc_2_currentIndexChanged(int index)
{
     ui->slc_6->setEnabled(true);
     test_sets.n_write=1;
     if(index == RAW_SEQ || index == RAW_RAND )
     {
         ui->slc_6->setEnabled(false);
         test_sets.n_write=0;
     }
}

void WinSettings::closeEvent(QCloseEvent *event)
{


    if(!user_has_saved)
    {
        QString message ="Settings not saved. Do you want to procede?\nIf you click Yes your setup will be lost.";

        QMessageBox::StandardButton resBtn = QMessageBox::question( this,"Exit",
                                                                    message,
                                                                    QMessageBox::No |
                                                                    QMessageBox::Yes,
                                                                    QMessageBox::Yes);

        if (resBtn == QMessageBox::Yes)
        {
            //qDebug()<<"Uscito senza salvare.";
            dir_bench=pre_bench_dir;
            user_has_saved=true;
            event->accept();
        }else
        {
            //qDebug()<<"Resto";
            event->ignore();
        }
    }else{
        //qDebug()<<"salvato. Uscita.";
    }

}



void WinSettings::on_txt_file_size_editingFinished()
{
    user_has_saved=false;
}

void WinSettings::on_txt_file_size_textEdited(const QString &arg1)
{
    if((pre_txt_size.toInt(0,10) != arg1.toInt(0,10)))
        user_has_saved=false;
}

