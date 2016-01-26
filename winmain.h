#ifndef WINMAIN_H
#define WINMAIN_H

#include <QMainWindow>
#include <iostream>
#include <QDebug>
#include <unistd.h>
#include <QProcess>
#include <QListWidget>
#include <QCloseEvent>
#include <QTimer>
#include "wincredits.h"
#include "wininfotests.h"
#include "wintestplot.h"
#include "utilities.h"
#include "winsettings.h"
#include "data.h"

#define DEBUG
#define NUM_UI_ELEM 2

using namespace std;

namespace Ui {
class WinMain;
}

class WinMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit WinMain(QWidget *parent = 0);
    ~WinMain();

private slots:
    /**
     * @brief Launch the correct script for the test
     */
    void on_btn_start_clicked();

    /**
     * @brief Update Gui during the test session
     */
    void update_test();

    /**
     * @brief Kill the current test.
     * After got the current test's pid, the function send the SIGINT signal to the test
     * to terminate it.
     */
    void kill_process();

    /**
     * @brief display the info window of the benchmark's tests
     */
    void on_actionInfo_benchmark_triggered();

    /**
     * @brief Handle the end of a single test
     * @param exit_status of the test just ended
     */
    void handle_test_finished(int exit_status);

    /**
     * @brief Update the progress bar during the test session
     */
    void update_progress_bar();

    /**
     * @brief get process output and set the startup iterator variable
     * for the STARTUP test.
     */
    void process_output();

    /**
     * @brief Change the description and parameters of the test
     * for a light benchmark
     */
    void on_rbtn_light_clicked();

    /**
     * @brief Change type_test_t variable of the test and the
     * description label properly, based on the type_test_t.
     * @param index used to know the test type:
     * index 0: throughput-->AGGR
     * index 1: reponsivness-->STARTUP
     */
    void on_test_selector_currentIndexChanged(int index);

    /**
     * @brief Change the description and parameters of the test
     * for an heavy benchmark.
     */
    void on_rbtn_heavy_clicked();

    /**
     * @brief Open the Settings window
     */
    void on_btn_custom_clicked();

    /**
     * @brief Set the UI and enable custom button
     */
    void on_rbtn_custom_clicked();

    /**
     * @brief clean history of realized test
     */
    void on_btn_clean_clicked();

    /**
     * @brief draw the graph of the selected test
     * @param selected
     */
    void on_lst_history_itemDoubleClicked(QListWidgetItem *item);

    /**
     * @brief display credits window
     */
    void on_actionCredits_triggered();

private:
    /** Main interface */
    Ui::WinMain*   ui;
    /** Settings interface */
    WinSettings    Win_settings;
    /** Info interface */
    WinInfoTests   Win_info_tests;
    /** Plot interface */
    WinTestPlot    Win_test_plot;
    /** Credits interface */
    WinCredits     Win_credits;
    /** Main process variable */
    QProcess       script_process;
    /** Timer for the test*/
    QTimer         timer_test;

    /** Mask that stored the scheduler(s) choosen by the user
      * It's never changed, it's used fake_mask instead.
      */
    unsigned int   real_mask;

    /** Mask used to switch to next scheduler for the test.
      * It's changed when a single test ends with the
      * correct scheduler's mask
      */
    unsigned int   fake_mask;

    QVector<QString> stat_files;

    /** QVector of WinTestPlot pointers that stores the plot interfaces
      * for the history test
      */
    QVector<WinTestPlot*> Win_test_plots;
    /** Iterator for the progress bar*/
    int             startup_iterator;

    /**
     * @brief Setup the UI when the application starts
     */
    void setupGui();

    /**
     * @brief setup the UI when a test starts
     */
    void setupGui_test_start();

    /**
     * @brief setup the UI when a test ends
     */
    void setupGui_test_ended();

    /**
     * @brief Handle the window's close event
     * @param event
     */
    void closeEvent (QCloseEvent *event);

    /**
     * @brief Setup all the UI and disconnect everything when a test
     * session ends. Display the graphics stats of the test session just
     * ended.
     */
    void test_ended();

    /**
     * @brief read the schedulers' check box to set real and fake mask.
     */
    void set_mask();

    /**
     * @brief enable the scheduler that the user have.
     */
    void enable_scheduler_box();

    /**
     * @brief remove the flag into fake_mask of the next scheduler to process.
     *        It also set this scheduler into settings of test
     * @return false if there aren't scheduler to process.
     */
    bool set_scheduler_for_test();

    /**
     * @brief Setup the test and start the session
     */
    void start_test();

    /**
     * @brief Change the progress bar test depending on the scheduler
     * @return QString of the progress bar format test
     */
    QString draw_sched_for_progress_bar();

    /**
     * @brief read the file into the results folder and adds them in the
     *        listview.
     */
    void create_list_history();

    /**
     * @brief save the stats of the test just finished in a file into the
     *        result tests folder
     * @param settings of the file fust finished
     */
    void save_new_stats(const test_settings_t &test);

};

#endif // WINMAIN_H
