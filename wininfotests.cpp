#include "wininfotests.h"
#include "ui_wininfotests.h"

WinInfoTests::WinInfoTests(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WinInfoTests)
{
    ui->setupUi(this);
    load_info_tests();
    on_slc_tests_currentIndexChanged(0);
}
WinInfoTests::~WinInfoTests()
{
    delete ui;
}

void WinInfoTests::on_slc_tests_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0: ui->textBrowser->setText(info_tests.aggr_info);
        break;
    case 1: ui->textBrowser->setText(info_tests.comm_info);
        break;
    default: break;
    }
}

bool WinInfoTests::load_info_tests()
{
    QFile file_tests(":/info_test.txt");

    if (!file_tests.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QString num_test(file_tests.readLine());
    QString line;

    bool is_num;

    while(!file_tests.atEnd())
    {
        line = QString(file_tests.readLine());
        line.toInt(&is_num,10);
        if(is_num)
        {
            num_test = line;
            line = QString(file_tests.readLine());
        }

        switch(num_test.toInt())
        {
        case 0:   info_tests.aggr_info.append(line);
            break;
        case 1:   info_tests.comm_info.append(line);
            break;
        default: break;
        }

    }

    return true;
}



void WinInfoTests::on_btn_back_clicked()
{
    this->close();
}
