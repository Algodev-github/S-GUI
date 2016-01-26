#include "utilities.h"

QString get_dir(QWidget * parent)
{
    QString path(QFileDialog::getExistingDirectory(parent, "Open Directory",
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks
                                                    | QFileDialog::ReadOnly));
    if(path != "")
        path.append("/");
    return path;
}

void set_basic_test(const test_settings_t& test_sets, QString& command)
{
    switch (test_sets.type_test) {
    case AGGR:
       command.append("./aggthr-with-greedy_rw.sh");
       break;
    case STARTUP:
       command.append("./comm_startup_lat.sh");
       break;
    case ERROR:
       command="ERROR";
        break;
    default:
        break;
    }

    set_type_scheduler(test_sets.type_scheduler, command);
    command.append(QString(" " + QString::number(test_sets.n_read)) );
    command.append(QString(" " + QString::number(test_sets.n_write)) );
    set_type_rw(test_sets.type_rw, command);

    if (test_sets.type_test == STARTUP)
    {
        command.append(QString(" " + QString::number(test_sets.n_iteration)) );
        set_type_terminal_command(test_sets.type_command, command);
    }
    command.append(QString(" "+ test_sets.dir_results));
}

void set_complete_test(const test_settings_t& test_sets,  QString& command)

{
    set_basic_test(test_sets,command);
    if (test_sets.type_test == AGGR)
   {
       command.append(QString(" %1 ").arg(test_sets.adv_settings.test_time));
   }else
   {
       command.append(QString(" %1").arg(test_sets.adv_settings.max_startup));
   }

}

void set_type_scheduler(const type_scheduler_t& type_scheduler, QString& command)
{
    switch (type_scheduler) {
    case BFQ:
       command.append(" bfq");
       break;
    case CFQ:
       command.append(" cfq");
       break;
    case NOOP:
       command.append(" noop");
       break;
    case DEADLINE:
       command.append(" deadline");
       break;
    }
}

void set_type_terminal_command(const type_command_t& type_command, QString& command)
{
    switch (type_command) {
    case TERM:
       command.append(" \" xterm -e /bin/true \" ");
       break;
    case GNOME:
       command.append(" \"gnome-terminal -e /bin/true\" ");
       break;
    }
}

void set_type_rw(const type_rw_t& type_rw, QString& command)
{
    switch (type_rw) {
    case SEQ:
       command.append(" seq");
       break;
    case RAND:
       command.append(" rand");
       break;
    case RAW_RAND:
       command.append(" raw_rand");
       break;
    case RAW_SEQ:
       command.append(" raw_seq");
       break;
    }
}

int get_pid_from_file(QString file_pid)
{
    ifstream pid_file(file_pid.toStdString().c_str());

    int pid_to_kill;

    if(pid_file.is_open())
    {
        while(pid_file>>pid_to_kill);
        pid_file.close();
    }else
    {
        cout<<"file not found"<<endl;
        return -1;
    }
    pid_file.close();

    return pid_to_kill;
}

void get_string_from_test(QString* rw_type, const test_settings_t& test)
{

    switch (test.type_rw) {
    case RAND:
        *rw_type="rand";
        break;
    case SEQ:
        *rw_type="seq";
        break;
    case RAW_RAND:
        *rw_type="raw_rand";
        break;
    case RAW_SEQ:
        *rw_type="raw_seq";
        break;
    default:
        break;
    }
}

bool get_stat_from_file(QVector<double>& avgData,  type_test_t type)
{
    QString test_directory=goto_correct_folder(type);
    test_directory.append(STATS_FILE_NAME);
    ifstream stats_file(test_directory.toStdString().c_str());

     if(!stats_file){
         avgData<<ERROR_FILE_NOT_FOUND<<ERROR_FILE_NOT_FOUND<<ERROR_FILE_NOT_FOUND
               <<ERROR_FILE_NOT_FOUND;
        //qDebug()<<"ERROR_FILE_NOT_FOUND";
         return false;
     }

     double value=0;
     while(stats_file>>value)
     {
         if(value == ERROR_FEW_DATA)
         {
             avgData<<ERROR_FEW_DATA<<ERROR_FEW_DATA<<ERROR_FEW_DATA<<ERROR_FEW_DATA;
             return false;
         }
         avgData<<value;
     }

     return true;
}

int generate_stat(const test_settings_t& test,const unsigned int mask)
{
    QString scheduler,rw_type;

    scheduler+=check_mask_for_scheduler(BFQ_MASK,mask,"bfq");
    scheduler+=check_mask_for_scheduler(CFQ_MASK,mask,"cfq");
    scheduler+=check_mask_for_scheduler(NOOP_MASK,mask,"noop");
    scheduler+=check_mask_for_scheduler(DEAD_MASK,mask,"deadline");

    get_string_from_test(&rw_type,test);
    QString name_test;

    if(test.type_test==AGGR)
        name_test=QString::number(test.adv_settings.test_time)+" aggthr_stat ";
    else if(test.type_test==STARTUP)
        name_test=" 0 lat_thr_stat ";

    QString par(" "+scheduler+" "+QString::number(test.n_read) +" "+QString::number(test.n_write) +" "+rw_type+" "+
                        name_test);
    par.prepend(GET_STAT_FILE);
    par.append(goto_correct_folder(test.type_test));
    par.prepend("./");

    ifstream get_stat(GET_STAT_FILE);
    if(!get_stat)
        return ERROR_FILE_NOT_FOUND;

    if(system(par.toStdString().c_str()) == -1)
        return -1;
    else
        return 1;

}

QString goto_correct_folder(type_test_t type)
{
    QString test_directory(dir_bench);
    switch (type) {
    case AGGR:
        test_directory.append("agg_thr-with-greedy_rw/");
        break;
    case STARTUP:
        test_directory.append("comm_startup_lat/");
        break;
    default:
        break;
    }
    return test_directory;

}

bool check_if_file_exists(const QString& file)
{
        ifstream filename(file.toStdString().c_str());
        if(filename)
            return true;
        else
            return false;
}

QString check_mask_for_scheduler(const unsigned int sched_mask, const unsigned int mask,QString scheduler)
{
    if(mask&sched_mask)
        return QString(" "+ scheduler +" ");
    else
        return QString(" 0 ");

}

void change_testpar(test_settings_t* test,const bool is_light)
{
    test->adv_settings.max_ratio=16500;
    test->adv_settings.max_startup=60;
    test->adv_settings.sync=true;
    test->adv_settings.test_time=20;

    test->n_iteration=5;
    if(is_light)
    {
        test->n_read=1;
        test->n_write=1;
    }else
    {
        test->n_read=3;
        test->n_write=3;
    }

    test->type_rw=SEQ;

}

bool update_config_file(const QString new_path, const char string_to_find[])
{
    QFile config_params(QString(dir_bench + "config_params.sh"));
    QFile help_file(QString(dir_bench + "config_params_help.sh"));

    help_file.open(QIODevice::WriteOnly);

    if (!config_params.open(QIODevice::ReadOnly))
        return false;

    const QString NEW_BASE_DIR = string_to_find + new_path +'\n';
    while(!config_params.atEnd())
    {
        QString line(config_params.readLine());
        if(line.contains(string_to_find))
            help_file.write(NEW_BASE_DIR.toStdString().c_str());
        else
            help_file.write(line.toStdString().c_str());
    }
    help_file.close();
    config_params.close();
    config_params.remove();

    help_file.rename(QString(dir_bench + "config_params.sh"));

    return true;
}

QString read_value_from_file(const QString string_to_find)
{
    system("pwd");
    QFile config_file(QString(dir_bench + "config_params.sh"));
    config_file.open(QIODevice::ReadWrite);

    while(!config_file.atEnd())
    {
        QString num_block(config_file.readLine());
        if(num_block.contains(string_to_find))
            return num_block.remove(string_to_find);
    }
    //qDebug()<<config_file.readLine();
    return QString("NaN");
}

void create_results_folder()
{
    if (opendir(PATH_RESULTS.toStdString().c_str()) == NULL)
    {
        const QString CREATE_FOLDER_COMMAND = "mkdir " + PATH_RESULTS;
        system(CREATE_FOLDER_COMMAND.toStdString().c_str());
    }

    return;
}


QVector <QString> get_list_test()
{
    QVector <QString> list_files;
    struct dirent *pent = NULL;
    DIR *pdir = opendir(PATH_RESULTS.toStdString().c_str());

    while ((pent = readdir (pdir))) // while there is still something in the directory to list
    {
        if (pent->d_name == NULL) // if pent has not been initialised correctly
            break;

        if (strcmp(pent->d_name,".") == 0 || strcmp(pent->d_name,"..") == 0)
            continue;

        list_files<<pent->d_name;
    }

    return list_files;
}

void clean_result_test()
{
    const QString CLEAN_COMMAND = "cd " + PATH_RESULTS + "; rm *; cd ..";
    system(CLEAN_COMMAND.toStdString().c_str());
    return;
}

void create_stat_file(const QString& name_file, const sched_result_t& values)
{
    ifstream is(name_file.toStdString());
    ofstream test_file_result;
    if (is) test_file_result.open(name_file.toStdString(), ios::app);
    else test_file_result.open(name_file.toStdString());

    const QString OPEN_FOLDER_COMMAND = "cd " + PATH_RESULTS;
    system(OPEN_FOLDER_COMMAND.toStdString().c_str());

    test_file_result<<values.lat<<'\n';
    /*Bisogna andare a capo*/
    test_file_result<<values.throug<<'\n';

    test_file_result.close();

    return;
}

QString get_name_file_stat(const test_settings_t& test, const unsigned int& mask_sched)
{
    QString name_file = "";
    char name_test='E';
    char rw_type[3];

    switch (test.type_test)
    {
        case AGGR:
            name_test = 'A';
            break;
        case STARTUP:
            name_test = 'S';
            break;
        case ERROR:
            name_test = 'E';
            break;
        default:
            break;
    }

    name_file += name_test;

    /* Aggiungere gli scheduler in base alla maschera */
    if (mask_sched & BFQ_MASK) name_file += 'X';
    else name_file += '-';
    if (mask_sched & CFQ_MASK) name_file += 'X';
    else name_file += '-';
    if (mask_sched & DEAD_MASK) name_file += 'X';
    else name_file += '-';
    if (mask_sched & NOOP_MASK) name_file += 'X';
    else name_file += '-';

    switch (test.type_rw) {
    case RAND:
        strcpy(rw_type,"RA");
        break;
    case SEQ:
        strcpy(rw_type,"SE");
        break;
    case RAW_RAND:
        strcpy(rw_type,"RR");
        break;
    case RAW_SEQ:
        strcpy(rw_type,"RS");
        break;
    default:
        break;
    }

    /* ES aggr bfq raw sec r 10 w 5 = AX---RS100500 */
    name_file += rw_type;
    if (test.n_read < 10) name_file += '0' + QString::number(test.n_read);
    else name_file += QString::number(test.n_read);

    if (test.n_write < 10) name_file += '0' + QString::number(test.n_write);
    else name_file += QString::number(test.n_write);

    if (test.type_test == STARTUP)
        if (test.n_iteration < 10) name_file += '0' + QString::number(test.n_iteration);
        else name_file += QString::number(test.n_iteration);
    else name_file += "00";

    /* devo estrarre la data */
    char buffer[255];
    time_t current_time;
    time(&current_time);
    struct tm * timeinfo;
    timeinfo = localtime (&current_time);

    strftime(buffer, 80, "%d %m %y-%H:%M:%S", timeinfo);
    name_file += buffer;
    name_file+=".txt";

    return name_file;
}


QString parsing_name_file(const QString& qt_file_name)
{
    char name_file[255];
    strcpy(name_file, qt_file_name.toStdString().c_str());

    QString caption = "";
    caption += qt_file_name.right(21).remove(qt_file_name.right(21).length()-4,4) + "  ";

    caption += "TEST: ";
    switch (name_file[0])
    {
        case 'A':
            caption += "Workload ";
            break;
        case 'S':
            caption += "Latency ";
            break;
    }

    caption += "SCHEDULER: ";

    /* Selezionare lo scheduler, va da 1 a 4*/
    if (name_file[1] == 'X') caption+= "bfq ";
    if (name_file[2] == 'X') caption+= "cfq ";
    if (name_file[3] == 'X') caption+= "dead ";
    if (name_file[4] == 'X') caption+= "noop ";

    caption += "RAW TYPE: ";

    if (name_file[5] == 'S') caption+= "seq ";
    else
        switch (name_file[6])
        {
        case 'A':
            caption+= "rand ";
            break;
        case 'R':
            caption+= "raw rand ";
            break;
        case 'S':
            caption+= "raw seq ";
            break;
        default:
            break;
        }


    caption+= "READ: " + QString::number(name_file[7]-'0'+name_file[8]-'0') + " ";
    caption+= "WRITE: " + QString::number(name_file[9]-'0'+name_file[10]-'0')+ " ";
    if (name_file[0] == 'S') caption += "ITERATION: " +
                            QString::number(name_file[11]-'0'+name_file[12]-'0');
    return caption;
}


double get_max_from_data(const QVector<double> data)
{
    double max=0;
    for(int i = 0; i< data.length();i++)
               if(data[i]>max)
                   max=data[i];

    return max;
}

unsigned int check_sys_schedulers()
{
    QString search_sys_hd("echo $(basename `mount | grep \" on / \" | cut -f 1 -d \" \"` | sed 's/\\(...\\).*/\\1/g') > hd.txt");
    system(search_sys_hd.toStdString().c_str());

    QFile hd_file("hd.txt");
    if (!hd_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream hd_file_in(&hd_file);
    QString sys_hd;
    hd_file_in>>sys_hd;
    system("rm hd.txt");

    unsigned int sched_mask=0;

    if(check_scheduler("bfq",sys_hd))
        sched_mask |=BFQ_MASK;
    if(check_scheduler("cfq",sys_hd))
        sched_mask |=CFQ_MASK;
    if(check_scheduler("noop",sys_hd))
        sched_mask |=NOOP_MASK;
    if(check_scheduler("deadline",sys_hd))
        sched_mask |=DEAD_MASK;

    return sched_mask;
}

bool check_scheduler(QString scheduler, QString sys_hd)
{
    QString command;
    command=QString("grep %1 /sys/block/%2/queue/scheduler > /dev/null").arg(scheduler).arg(sys_hd);
    if(!system(QString(command).toStdString().c_str()))
        return true;
    else
        return false;
}

int grep_iteration(int iteration, int lenght_iteration)
{
    QString iteration_string = QString("Iteration %1 / %2").arg(iteration).arg(lenght_iteration);
    QString command = QString("grep \" %1 \" %2 ").arg(iteration_string).arg(STARTUP_OUT);
    //qDebug()<<command;
    return system(command.toStdString().c_str());
}
