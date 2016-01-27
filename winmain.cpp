#include "winmain.h"
#include "ui_winmain.h"

extern test_settings_t test_sets;
extern advanced_settings_t adv;

void WinMain::create_list_history()
{
    stat_files = get_list_test();
    if (stat_files.length() == 0)
    {
        ui->btn_clean->setEnabled(false);
        return;
    }
    for (int i = 0; i<stat_files.length() ;i++)
        ui->lst_history->addItem(parsing_name_file(stat_files[i]));

    return;
}

WinMain::WinMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WinMain)
{
    ui->setupUi(this);
	ui->menuBar->setNativeMenuBar(false);
    startup_iterator=1;
    ui->logo->setPixmap(QPixmap(":/algodevlogo.png"));
    create_results_folder();
    create_list_history();

    setupGui();
}

WinMain::~WinMain()
{
    delete ui;
}

void WinMain::setupGui()
{
    ui->progressBar->setVisible(false);
    ui->btn_custom->setEnabled(false);
    ui->rbtn_light->setChecked(true);
    enable_scheduler_box();

    unsigned int sys_sched=check_sys_schedulers();
    ui->btn_start->setEnabled(sys_sched);

    this->on_rbtn_light_clicked();
}

void WinMain::setupGui_test_start()
{
    ui->btn_start->setEnabled(false);
    ui->box_1->setEnabled(false);
    ui->box_2->setEnabled(false);
    ui->box_3->setEnabled(false);
    ui->box_4->setEnabled(false);
    ui->btn_clean->setEnabled(false);
    ui->rbtn_custom->setEnabled(false);
    ui->rbtn_heavy->setEnabled(false);
    ui->rbtn_light->setEnabled(false);
    ui->test_selector->setEnabled(false);
    ui->lst_history->setEnabled(false);
    ui->progressBar->setVisible(true);
    ui->btn_custom->setEnabled(false);
}

void WinMain::setupGui_test_ended()
{
    enable_scheduler_box();
    ui->btn_start->setEnabled(true);
    ui->btn_clean->setEnabled(true);
    ui->rbtn_custom->setEnabled(true);
    ui->rbtn_heavy->setEnabled(true);
    ui->rbtn_light->setEnabled(true);
    ui->test_selector->setEnabled(true);
    ui->lst_history->setEnabled(true);
    ui->progressBar->setVisible(false);
    this->on_rbtn_custom_clicked();
}


void WinMain::handle_test_finished(int exit_status)
{
    //qDebug()<<"Test finito con esito: "<<exit_status;
    switch(exit_status)
    {
    case 0:
        ui->progressBar->setValue(ui->progressBar->value() + PROGRESSBAR_INTERVAL );
        sleep(1.5);
        if(!this->set_scheduler_for_test())
            test_ended();
        else
        {
            script_process.terminate();
            disconnect(&script_process, SIGNAL(finished(int)),this, SLOT(handle_test_finished(int)));
            start_test();
        }
        break;
    default:
        exit(exit_status);
        break;
    }

}

void WinMain::on_btn_start_clicked()
{    
  
    //Check if there is at least one scheduler checked
    if( ui->box_1->isChecked() ||
	ui->box_2->isChecked()||
	ui->box_3->isChecked()||
	ui->box_4->isChecked() )
	{
		this->set_mask();
   		this->set_scheduler_for_test();
    		this->start_test();
	}

}

void WinMain::start_test()
{
    //qDebug()<<"TEST START";
    /*Get the correct directory of the test*/
    //Save the current working directory
    char buff[PATH_MAX + 1];
    QString current_work_dir(getcwd(buff, PATH_MAX+1));
    QString test_directory=goto_correct_folder(test_sets.type_test);

    if(chdir(test_directory.toStdString().c_str()) == -1)
    {
	//TODO
    }


     if(check_if_file_exists(QString(STATS_FILE_NAME)))
          system("rm " STATS_FILE_NAME);

    QString command;

    //command creation
    set_complete_test(test_sets, command);

    //system("echo $PWD");
    //qDebug()<<command.toStdString().c_str();

    script_process.start(command);

    //Connections
    connect(&script_process, SIGNAL(readyReadStandardOutput()),this, SLOT(update_test()));
   //if(test_sets.type_test == STARTUP)
        connect(&script_process, SIGNAL(readyRead()),this, SLOT(process_output()));
    connect(&script_process, SIGNAL(finished(int)),this, SLOT(handle_test_finished(int)));

    //Back to the top working directory
    chdir(current_work_dir.toStdString().c_str());

}


void WinMain::process_output()
{
    QString outputString(this->script_process.readAllStandardOutput());
    //qDebug()<<outputString;
    QString controlla= QString("Iteration %1 / %2").arg(startup_iterator).arg(test_sets.n_iteration);
    if(outputString.contains(controlla))
        startup_iterator++;

}



void WinMain::update_test()
{

    if(script_process.state() == QProcess::Running)
    {
        //qDebug()<<"Set Gui for running";
        setupGui_test_start();
     }
    /*Progress bar settings*/
    connect(&timer_test, SIGNAL(timeout()),this,SLOT(update_progress_bar()));
    ui->progressBar->setValue(0);
    startup_iterator=1;


    ui->progressBar->setFormat(draw_sched_for_progress_bar());
    if(test_sets.type_test == AGGR)
    {
        ui->progressBar->setMaximum(test_sets.adv_settings.test_time + PROGRESSBAR_INTERVAL);
    }else
    {
        ui->progressBar->setMaximum(test_sets.n_iteration + PROGRESSBAR_INTERVAL);
    }
    timer_test.start(1000);

    disconnect(&script_process, SIGNAL(readyReadStandardOutput()),this, SLOT(update_test()));
}

void WinMain::update_progress_bar()
{
    if(ui->progressBar->value()==(ui->progressBar->maximum()- PROGRESSBAR_INTERVAL))
    {
        this->timer_test.stop();
        disconnect(&timer_test, SIGNAL(timeout()),this,SLOT(update_progress_bar()));
    }
    else
    {
         if(test_sets.type_test == AGGR)
              ui->progressBar->setValue(ui->progressBar->value() +1 );
         else
              ui->progressBar->setValue(this->startup_iterator);

    }
   ui->progressBar->setFormat(draw_sched_for_progress_bar());
}

void WinMain::kill_process()
{
    QString kill_command;
    kill_command = QString("kill -2 %1 ").arg(script_process.pid());
    //qDebug()<<kill_command;
    system(kill_command.toStdString().c_str());
    script_process.terminate();
    disconnect(&script_process, SIGNAL(finished(int)),this, SLOT(handle_test_finished(int)));
}



void WinMain::closeEvent (QCloseEvent *event)
{
   QString message;
   if(script_process.state() == QProcess::Running)
       message.append("Test is running. Are you sure to close?\nTest will be terminated.");
   else
       message.append("Are you sure ?");


   QMessageBox::StandardButton resBtn = QMessageBox::question( this,"Exit",
                                                                message,
                                                                QMessageBox::No |
                                                                QMessageBox::Yes,
                                                                QMessageBox::Yes);

    if (resBtn != QMessageBox::Yes)
    {
         event->ignore();
    } else
    {
        if(this->Win_settings.isEnabled() && !this->Win_settings.isHidden())
        {
            QMessageBox::information(this,"Closing...","All opened windows will be closed",QMessageBox::Escape);
              this->Win_settings.close();
        }
        if(script_process.state() == QProcess::Running)
        {
            QString message("Are you really sure? \n");
            QMessageBox::StandardButton sureBtn = QMessageBox::question(this,
                                                              "About to close while process running",
                                                             message,
                                                             QMessageBox::No |
                                                             QMessageBox::Yes,
                                                             QMessageBox::Yes);

                if(sureBtn == QMessageBox::Yes)
                {
                kill_process();
                sleep(1);
                }else
                    event->ignore();

         }else if(script_process.exitStatus() == QProcess::NormalExit)
        {
        event->accept();
        }
    }

}

void WinMain::on_actionInfo_benchmark_triggered()
{
    Win_info_tests.exec();
}

/* Da modificare a seconda del numero di test effettuato */
void WinMain::save_new_stats(const test_settings_t& test)
{
    /* Creo il nome del test */
    QString name_file = get_name_file_stat(test, real_mask);
    stat_files<<name_file;

    /* Aggiungo test fatto alla listview */
    ui->btn_clean->setEnabled(true);
    ui->lst_history->addItem(parsing_name_file(name_file));

    /* File non trovato, ne genero uno nuovo
    create_new_stats_file(sched_result, name_file, real_mask);*/

    ofstream new_stats_file;
    const QString file_path = PATH_RESULTS+"/"+ name_file;
    new_stats_file.open(file_path.toStdString());

    QVector<double> avgData;
    get_stat_from_file(avgData, test.type_test);

    for (int i = 0; i<avgData.length(); i++)
    {
        sched_result_t sched;
        if (avgData[i] != 0)
        {
            if (test.type_test == AGGR)
            {
                sched.throug=avgData[i];
                sched.lat=0;
            }
            else
            {
                sched.lat=avgData[i];
                sched.throug=0;
            }

            new_stats_file<<sched.lat<<'\n';
            new_stats_file<<sched.throug<<'\n';
        }
    }

    new_stats_file.close();

    return;
}


void WinMain::test_ended()
{
    //qDebug()<<"testendend";
    disconnect(&script_process, SIGNAL(finished(int)),this, SLOT(handle_test_finished(int)));
    script_process.terminate();
    setupGui_test_ended();
    /*Display graph stats*/
    if(generate_stat(test_sets,real_mask)==ERROR_FILE_NOT_FOUND)
        QMessageBox::warning(this,"Error..","Statistic generator file not found!",QMessageBox::Escape);
    else
    {
        save_new_stats(test_sets);
        if(Win_test_plot.prepareForShowing(test_sets))
            Win_test_plot.exec();
    }

}


void WinMain::on_test_selector_currentIndexChanged(int index)
{
    switch (index) {
    case AGGR:
        test_sets.type_test = AGGR;
        break;
    case STARTUP:
        test_sets.type_test = STARTUP;
        break;
    default:
        break;
    }
    if(ui->rbtn_heavy->isChecked())
        this->on_rbtn_heavy_clicked();
    else if (ui->rbtn_light->isChecked())
        this->on_rbtn_light_clicked();
}
void WinMain::on_rbtn_light_clicked()
{
    ui->btn_custom->setEnabled(false);
    change_testpar(&test_sets,true);
    if(test_sets.type_test==AGGR)
        ui->lbl_test_description->setText(LIGHT_TEST_DESCRIPTION_A);
    else
        ui->lbl_test_description->setText(LIGHT_TEST_DESCRIPTION_S);

}
void WinMain::on_rbtn_heavy_clicked()
{
    ui->btn_custom->setEnabled(false);
    change_testpar(&test_sets,false);
    if(test_sets.type_test==AGGR)
        ui->lbl_test_description->setText(HEAVY_TEST_DESCRIPTION_A);
    else
        ui->lbl_test_description->setText(HEAVY_TEST_DESCRIPTION_S);
}
void WinMain::on_rbtn_custom_clicked()
{
    ui->btn_custom->setEnabled(true);
    ui->lbl_test_description->setText(CUSTOM_TEST_DESCRIPTION);
}

void WinMain::on_btn_custom_clicked()
{
    Win_settings.update_advanced_interface();
    Win_settings.show();
}

void WinMain::set_mask()
{
    real_mask = 0;
    if(ui->box_1->isChecked())
         real_mask |= BFQ_MASK;
    if(ui->box_2->isChecked())
         real_mask |= CFQ_MASK;
    if(ui->box_3->isChecked())
         real_mask |= NOOP_MASK;
    if(ui->box_4->isChecked())
         real_mask |= DEAD_MASK;

    fake_mask=real_mask;
}
void WinMain::enable_scheduler_box()
{
    bool setted=false;
    unsigned int sched_mask=check_sys_schedulers();

    if(sched_mask &BFQ_MASK)
    {
        ui->box_1->setEnabled(true);
        ui->box_1->setChecked(true);
        setted=true;
    }
    else
        ui->box_1->setEnabled(false);

    if(sched_mask&CFQ_MASK)
    {
        ui->box_2->setEnabled(true);
        if(!setted)
        {
            ui->box_2->setChecked(true);
            setted=true;
        }
    }
    else
        ui->box_2->setEnabled(false);

    if(sched_mask &NOOP_MASK)
    {
        ui->box_3->setEnabled(true);
        if(!setted)
        {
            ui->box_3->setChecked(true);
            setted=true;
        }
    }
    else
        ui->box_3->setEnabled(false);

    if(sched_mask&DEAD_MASK)
    {
        ui->box_4->setEnabled(true);
        if(!setted)
        {
            ui->box_4->setChecked(true);
            setted=true;
        }
    }
    else
        ui->box_4->setEnabled(false);

}
bool WinMain::set_scheduler_for_test()
{
    if(fake_mask &BFQ_MASK)
    {
        fake_mask^=BFQ_MASK;
        test_sets.type_scheduler=BFQ;
        return true;
    }
    if(fake_mask&CFQ_MASK)
    {
        fake_mask^=CFQ_MASK;
        test_sets.type_scheduler=CFQ;
        return true;
    }
    if(fake_mask&NOOP_MASK)
    {
        fake_mask^=NOOP_MASK;
        test_sets.type_scheduler=NOOP;
        return true;
    }
    if(fake_mask&DEAD_MASK)
    {
        fake_mask^=DEAD_MASK;
        test_sets.type_scheduler=DEADLINE;
        return true;
    }

    return false;
}
QString WinMain::draw_sched_for_progress_bar()
{
    QString sched_in_progress;
    switch(test_sets.type_scheduler)
    {
        case BFQ:
            sched_in_progress=QString("BFQ in progress...");
        break;
        case CFQ:
            sched_in_progress=QString("CFQ in progress...");
        break;
        case NOOP:
            sched_in_progress=QString("NOOP in progress...");
        break;
        case DEADLINE:
            sched_in_progress=QString("DEADLINE in progress...");
        break;
        default:
        break;
        }
    return sched_in_progress;
}


void WinMain::on_btn_clean_clicked()
{
    clean_result_test();
    ui->btn_clean->setEnabled(false);

    stat_files.remove(0);

    for (int i = 0; i<stat_files.length() ;i++)
        stat_files.remove(i);

    /*Eliminare voci nella combobox*/
    while (ui->lst_history->count() > 0)
        ui->lst_history->removeItemWidget(ui->lst_history->takeItem(ui->lst_history->count()-1));

    return;
}

void WinMain::on_lst_history_itemDoubleClicked(QListWidgetItem *item)
{

	for(int i=0; i<Win_test_plots.length(); i++)
	{
                if(!Win_test_plots[i]->isVisible())
                    Win_test_plots.remove(i);
	}

    //qDebug()<<"premuto sull'item:"<<item->text();
    QString file_to_open = stat_files[ui->lst_history->currentRow()];
    const QString file_path = PATH_RESULTS+"/"+ file_to_open;

    ifstream is(file_path.toStdString());
    if (!is) return;

    QVector <sched_result_t> sched_result;
    /* Guardo quali scheduler usa il test */
    for (int i = 1; i < 5;i++)
    {
        sched_result_t sched;
        if (file_to_open[i] == 'X')
        {
            is>>sched.lat;
            is>>sched.throug;
        }
        else
        {
            sched.lat = 0;
            sched.throug = 0;
        }
        switch (i){
            case 1: sched.sched=BFQ; break;
            case 2: sched.sched=CFQ; break;
            case 3: sched.sched=DEADLINE; break;
            case 4: sched.sched=NOOP; break;
        }
        sched_result<<sched;
    }

    WinTestPlot *test= new WinTestPlot();
    QString name_file = parsing_name_file(stat_files[ui->lst_history->currentRow()]);
    type_test_t type=ERROR;
    switch (file_to_open.at(0).toLatin1())
    {
        case 'A':
            type=AGGR;
            break;
        case 'S':
            type=STARTUP;
            break;
    }
    //TO DO: type_test_t"type" value control
    if(Win_test_plots.length() < MAX_PLOTS_OPENED)
    {
        if(test->setData(sched_result,name_file,type))
        {
            test->setGuiAfterData();
            test->show();
            this->Win_test_plots.append(test);
        }else
            test->deleteLater();
    }else
       QMessageBox::warning(this,"Warning","Too many plots opened.\nTry to close one",QMessageBox::Escape);


    is.close();

    return;
}

void WinMain::on_actionCredits_triggered()
{
    Win_credits.exec();
}
