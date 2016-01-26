#ifndef WINSETTINGS_H
#define WINSETTINGS_H

#include <QMainWindow>
#include <QFileDialog>
#include <QCloseEvent>
#include "utilities.h"
#include "data.h"

namespace Ui {
class WinSettings;
}

class WinSettings : public QMainWindow
{
    Q_OBJECT

public:
    explicit WinSettings(QWidget *parent = 0);
    ~WinSettings();
    void update_advanced_interface();

private slots:
    /**
     * @brief browse_button_clicked
     * Select a folder in order to change the directory of base_file or benchmark-suite.
     */
    void browse_button_clicked();

    /**
     * @brief on_btn_default_clicked
     * Reset default advanced settings.
     */
    void on_btn_default_clicked();

    /**
     * @brief on_btn_cancel_clicked
     * Close the window.
     */
    void on_btn_cancel_clicked();

    /**
     * @brief on_btn_save_clicked
     * Save the current advanced settings.
     */
    void on_btn_save_clicked();

    void on_slc_5_textChanged(const QString &arg1);

    void on_slc_6_textChanged(const QString &arg1);

    void on_slc_3_textChanged(const QString &arg1);


    void on_slc_2_currentIndexChanged(int index);

    void on_txt_file_size_editingFinished();

    void on_txt_file_size_textEdited(const QString &arg1);

private:
    enum dir_type{
            BENCH_DIR,
            BASE_DIR
    };

    advanced_settings_t temp_adv;
    unsigned int mask_enabled_save;
    /**
     * @brief dir_bench pre-save
     */
    QString pre_bench_dir;
    /**
     * @brief txt_size pre-save
     */
    QString pre_txt_size;

    Ui::WinSettings *ui;
    bool user_has_saved;

    /**
     * @brief Handle the window's close event
     * @param event
     */
    void closeEvent (QCloseEvent *event);

    /**
     * @brief correct_input check if the input values are correct or not, and shows the particular errors
     * @return true if the all values al correct, else false
     */
    bool correct_input();

    void set_test_parameters();

    void set_btn_save();

};

#endif // WINSETTINGS_H
