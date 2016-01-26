#ifndef WININFOTESTS_H
#define WININFOTESTS_H

#include <QDialog>
#include <QFile>
#include <QDebug>
#include "data.h"

namespace Ui {
class WinInfoTests;
}

class WinInfoTests : public QDialog
{
    Q_OBJECT

public:
    explicit WinInfoTests(QWidget *parent = 0);
    ~WinInfoTests();

private slots:
    void on_slc_tests_currentIndexChanged(int index);

    void on_btn_back_clicked();

private:
    Ui::WinInfoTests *ui;
    tests_info_t      info_tests;

    bool load_info_tests();
};

#endif // WININFOTESTS_H
