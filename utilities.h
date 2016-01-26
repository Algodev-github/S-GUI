#ifndef UTILITIES
#define UTILITIES
#include <QString>
#include <QFileDialog>
#include "data.h"
#include <iostream>
#include <fstream>
#include <QVector>
#include <QDebug>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>

using namespace  std;

/**
 * Stats file name
 * This file contains the results of the current text
 */
#define STATS_FILE_NAME "stats.out"

/**
 * Get stats script name
 * This script is used to create @STAT_FILE_NAME
 */
#define GET_STAT_FILE "get_stat.sh"

/**
 * Startup output file
 * This file is used as stout of the Responsivness test
 */
#define STARTUP_OUT "startup.out"

/**
 * Errors macros
 */
#define ERROR_FEW_DATA          -1
#define ERROR_GET_MAX           -2
#define ERROR_FILE_NOT_FOUND    -3
#define NO_ROOT_PRIVILEGES_ERROR 4

/**
 * Schedulers' position in arrays
 * These positions must be respected in any
 * creation of an array of schedulers
 */
#define BFQ_POSITION        0
#define CFQ_POSITION        1
#define NOOP_POSITION       2
#define DEADLINE_POSITION   3

/**
 * Schedulers mask
 */
#define BFQ_MASK    8
#define CFQ_MASK    4
#define NOOP_MASK   2
#define DEAD_MASK   1

/**
 * Interval for progress bars
 */
#define PROGRESSBAR_INTERVAL  1

/**
 * Max number of plot permitted
 */
#define MAX_PLOTS_OPENED 5

extern QString dir_bench;

/**
 * @brief Get the full path of the directory choosen by the user
 * with a QFileDialog istance.
 * It starts with the /home folder
 *
 * @param parent
 * @return QString with the the entire path to the directory.
 */
QString get_dir(QWidget *parent);

/**
 * @brief Set the basic test arguments.
 * First, the correct name of the test is set, then the command is created
 * with all the properly arguments using the utility's functions:
 * @set_type_scheduler,
 * @set_type_terminal_command,
 * @set_type_rw;
 * Moreover the order of the functions' calls follows the one of the script's
 * arguments.
 * The arguments are taken from the test_sets variable and are not
 * modified.
 * @param test_sets used to retrieve parameters
 * @param command to modify with basic settings
 */
void set_basic_test(const test_settings_t& test_sets, QString& command);


/**
 * @brief Set the complete test arguments using also the
 * @set_basic_test function.
 * @param test_sets to retrieve parameters
 * @param command to modify with the advanced settings
 */
void set_complete_test(const test_settings_t& test_sets,  QString& command);

/**
 * @brief Set the scheduler of the command.
 * @param type_scheduler: scheduler to use
 * @param command to modify
 */
void set_type_scheduler(const type_scheduler_t& type_scheduler, QString& command);

/**
 * @brief Add the terminal command to the command
 * @param type_command: terminal command
 * @param command to modify
 */
void set_type_terminal_command(const type_command_t& type_command, QString& command);

/**
 * @brief set Read/Write type of the command
 * @param type_rw: type_rw_t to use
 * @param command to modify
 */
void set_type_rw(const type_rw_t& type_rw, QString& command);

/**
 * @brief get pid from @param file pid
 * @param file_pid: file where read the pid
 * @return pid as int
 *
 * NOT USED RIGHT NOW
 */
int get_pid_from_file(QString file_pid);

/**
 * @brief Get the "type_rw_t type_rw" data of the test
 * as a QString
 * @param rw_type: Qstring passed by reference that will be changed
 * with the type_rw of the @param test
 * @param test where it's stored the type_rw variable
 */
void get_string_from_test(QString* rw_type, const test_settings_t& test);

/**
 * @brief Get the stats of the test and store them into a QVector
 * @param avgData: Qvector where the data are stored
 * @param type: type_test_t used to go to the correct test folder
 * @return true if the loading was successfull, false otherwise.
 */
bool get_stat_from_file(QVector<double>& avgData,  type_test_t type);

/**
 * @brief Generate the test's statistics. It executes the @GET_STAT_FILE
 * @param test: test_settings_t used to retrieve settings
 * @param mask: retrieve the schedulers used by user
 * @return exit status of the script. If the file @STATS_FILE_NAME does
 * not exit, the @ERROR_FILE_NOT_FOUND is returned.
 */
int generate_stat(const test_settings_t& test,const unsigned int mask);

/**
* This function is used to retrieve the correct folder based on the test's type.
* The function assumes that you are in the main directory where the exectuable
* and the benchmark folder are. N.B:It doesn't check if the path exists.
*/
/**
 * @brief Get the correct folder based on the type_test_t type
 * N.B:It doesn't check if the path exists.
 * @param type
 * @return QString with the correct folder.
 */
QString goto_correct_folder(type_test_t type);

/**
 * @brief Check if the file exists
 * @param file to check
 * @return true if the file exists, false otherwise.
 */
bool check_if_file_exists(const QString& file);

/**
 * @brief get the max value from QVector of double
 * @param data vector
 * @return max value
 */
double get_max_from_data(const QVector<double> data);

/**
* Check if the the @param sched_mask is setted in @param mask
* and return the scheduler as QString
*/
QString check_mask_for_scheduler(const unsigned int sched_mask, 
				const unsigned int mask,QString scheduler);

/**
 * @brief change the test parameters for the light and heavy test
 * @param test to modify and is_light bool
 */
void change_testpar(test_settings_t* test,const bool is_light);

/**
 * @brief create the folder contains the results of test
 */
void create_results_folder();

/**
 * @brief read the name of files into folder of results
 * @return a vector of QString contains the name of test results
 */
QVector <QString> get_list_test();

/**
 * @brief remove all test results into folder of results
 */
void clean_result_test();

/**
 * @brief get stats from a selected file
 * @param name of file to get statics
 * @return stats of the test choose by name_file
 */
sched_result_t get_stats_to_draw(const QString& name_file);

/**
 * @brief update_config_file update config_params file with new parameters from settings
 * @param new_path: new parameter to be written in config_file file
 * @param string_to_find: position in the file where the parameter has to be written
 * @return true if config_params file has been updated, else false
 */
bool update_config_file(const QString new_path, const char string_to_find[]);

/**
 * @brief read initial values for the settings screen from confgi_file file
 * @param string_to_find: the value path to find in the file
 * @return QString containing the value
 */
QString read_value_from_file(const QString string_to_find);

/**
 * @brief check schedulers installed on system
 * @return unsigned int representing a bitmask of schedulers installed. 
 */
unsigned int check_sys_schedulers();

/**
 * @brief check for a single scheduler if it is installed on system. 
 * @return true if scheduler is installed, false if not. 
*/
bool check_scheduler(QString scheduler,QString sys_hd);

/**
 * @brief create a symbolic name for the test just finished
 * @param currently test
 * @param currently mask_sched
 * @return QString that describe the test just finished
 */
QString get_name_file_stat(const test_settings_t& test, const unsigned int &mask_sched);

/**
 * @brief get a short description of the test with with its settings
 * @param name of file to convert in a description of the test
 * @return QString with a short description
 */
QString parsing_name_file(const QString& qt_file_name);

/**
 * @brief create the file contains stat of test just finished
 * @param contains the stats of test
 * @param contains the name with the file must have
 */
void create_new_stats_file(const sched_result_t& sched_result,
                                  const QString& name_file);

/**
 * @brief Grep the current STARTUP test iteration
 * @param iteratation: current iteration
 * @param length_iteration: total number of iteration
 * @return the system() return value
 */
int grep_iteration(int iteration,int length_iteration);
#endif // UTILITIES
