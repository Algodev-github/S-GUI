#include <QString>
#include <unistd.h>
#ifndef DATA_H
#define DATA_H

#define LIGHT_TEST_DESCRIPTION_A "This test performs 1 read and 1 "\
                                 "write sequentially for 20\nseconds."

#define HEAVY_TEST_DESCRIPTION_A "This test performs 3 read and 3 "\
                                 "write sequentially for 20\nseconds."

#define CUSTOM_TEST_DESCRIPTION "Create your custom test. Click on "\
                                "custom button.\n"\
                                "WARNING:use only if you know what "\
                                "you're doing."

#define LIGHT_TEST_DESCRIPTION_S "This test performs 1 read and 1 "\
                                 "write sequentially while a\ncommand "\
                                 "is executed 5 times."

#define HEAVY_TEST_DESCRIPTION_S "This test performs 3 reads and 3 "\
                                 "writes sequentially while a\ncommand "\
                                 "is executed 5 times."


#define BASE_DIR_M "BASE_DIR="

#define NUM_BLOCKS_CREATE_SEQ_M "NUM_BLOCKS_CREATE_SEQ="

const QString PATH_RESULTS = "test_results";

/**
* Tests available
* @AGGR: aggthr-with-greedy_rw.sh
* @STARTUP: comm_startup_lat.sh
* @MULTI_TEST: run_all_tests_1.sh
*/
enum type_test_t{
            AGGR,
            STARTUP,
            ERROR //GESTIRE BENE QUESTO ERROR.
};

/**
 * @brief Schedulers available for the tests
 */
enum type_scheduler_t{
              BFQ,
              CFQ,
              NOOP,
              DEADLINE
};

/**
 * @brief List of commands to lunch when testing I/O
 * and start-up time
 */
enum type_command_t{
            GNOME,
            TERM
};

/**
 * @brief Type of Reads and Writes
 * @SEQ: Sequential
 * @RAND: Random
 * @RAW_SEQ/ @RAW_RAND : read directly from the device
 * to avoid writing large files repeatedly
 */
enum type_rw_t{
             SEQ,
             RAND,
             RAW_SEQ,
             RAW_RAND
};

/**
 * @brief Structure that contains the advanced settings
 * used in the tests.
 * @dir_test: path to the base test directory, where the files are read and
 * written to. Default is /var/lib/bfq
 *
 * @test_time : maximum duration allowed for each test(in seconds)
 * @max_startup: maximum duration allowed for each command
 * invocation, in seconds.
 * @max_ratio: maximum write rate in [Kb/s]
 * @sync: invoke sync before starting readers/writers in the scripts
 * that allows it
 */
struct advanced_settings_t
{

    QString default_dir_bench="benchmark-suite/";
    QString dir_test = "/var/lib/bfq";

    unsigned int test_time = 20;
    unsigned int max_startup = 60;
    unsigned int max_rate = 16500;
    bool sync = true;

};

/**
 * @brief structure that contains the main settings for the test
 *
 * @type_test: current type of test
 * @type_scheduler: current scheduler
 * @type_command: current command to execute(if needed)
 * @type_rw: current type of Read/Write
 *
 * @dir_results: path to the directory where the test's results will be stored
 *
 * @n_iteration: number of test's iterations
 * @n_read: number of the reads to do
 * @n_write: numeber of the writes to do
 * @n_repetition_multi: number of repetitions(used only for @MULTI_TEST)
 * @adv_settings struct advanced_settings_t that contains the advanced test settings
 */
struct test_settings_t
{

    type_test_t type_test = AGGR;
    type_scheduler_t type_scheduler = BFQ;
    type_command_t type_command = GNOME;
    type_rw_t type_rw = RAW_SEQ;

    QString dir_results = ".";

    int n_iteration = 5;
    int n_read = 1;
    int n_write = 1;
    /* int n_repetition_multi; */
    advanced_settings_t adv_settings;

};

/**
 * @brief This is the structure that contains the descriptions of each test.
 */
struct tests_info_t
{
    QString aggr_info;
    QString comm_info;
};

/**
 * @brief This is the scheduler result structure that contains the scheduler
 * stats.
 */
struct sched_result_t
{
    double throug;
    double lat;
    double read;
    double write;

    type_scheduler_t sched;
};


#endif // DATA_H
