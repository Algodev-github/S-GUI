#ifndef WINTESTPLOT_H
#define WINTESTPLOT_H

#include <QDialog>
#include "qcustomplot.h"
#include "utilities.h"
#include "data.h"

namespace Ui {
class WinTestPlot;
}

class WinTestPlot : public QDialog
{
    Q_OBJECT

public:
    explicit WinTestPlot(QWidget *parent = 0);
    /**
     * @brief Set the data of avgData based on the @param result
     * @param result: data to store
     * @param testo: QString used for test description
     * @param type: used to know the test_type
     * @return true if the loading was successfull, false otherwise.
     */
    bool setData(const QVector<sched_result_t> result,QString testo,type_test_t type);

    /**
     * @brief Set the interface based on data loaded with the setData function.
     * This function must be called only if the setData function returned true.
     * @return true if the setup was successfull, false otherwise.
     */
    bool setGuiAfterData();

    /**
     * @brief Set interface based on the data stored in @STATS_FILE_NAME
     * @param test
     * @return true if the setup was successfull, false otherwise.
     */
    bool prepareForShowing(const test_settings_t& test);


    ~WinTestPlot();


private slots:
    /**
     * @brief Reset plot and interface and close the window
     */
    void on_pushButton_clicked();

private:
    /** Main interface */
    Ui::WinTestPlot *ui;
    /** String for the lables' window*/
    QString scheduler,n_read,n_write,rw_type;
    QString compare_string;
    /** Plot (Error) text*/
    QCPItemText *testo;
    /** Main plot data*/
    QVector<double> avgData;

    /**
     * @brief load Data from STATS_FILE_NAME
     * @param test
     * @return true if the loading was successfull, false otherwise.
     */
    bool loadData(const test_settings_t& test);

    /**
     * @brief Setup of the plot with the @param data
     * @param customPlot
     * @param data
     */
    void setupPlotAfterTest(QCustomPlot *customPlot,const QVector<double> data);

    /**
     * @brief Frontend for the interface's setup and plot's setup.
     * @param avgData used for the plot
     * @return true if the setup was successfull, false otherwise.
     */
    bool setupAllPlotsAfterTest(const QVector<double> avgData);

    /**
     * @brief Setup labels window
     * @param test
     */
    void setupLables(const test_settings_t& test);

    /**
     * @brief Save the default labels window
     */
    void saveDefaultLabel();

    /**
     * @brief Load the default labels window
     */
    void loadDefaultLabel();

    /**
     * @brief Handle the close event of the window
     * @param event
     */
    void closeEvent(QCloseEvent *event);



};

#endif // WINTESTPLOT_H
