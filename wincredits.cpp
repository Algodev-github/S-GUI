#include "wincredits.h"
#include "ui_wincredits.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QDebug>
#include <QUrl>

WinCredits::WinCredits(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WinCredits)
{
    ui->setupUi(this);
    ui->logo->setPixmap(QPixmap(":/algodevlogo.png"));
    ui->label_6->setText("<font color='blue'>http://algogroup.unimore.it/algodev/</font>");
}

WinCredits::~WinCredits()
{
    delete ui;


}
bool WinCredits::attention(){
    QString message="If you are using google chrome as a deafault web browser\nyou can't open the links above1:(\nProcede anyway?";
    QMessageBox::StandardButton resBtn = QMessageBox::question( this,"Warning",
                                                                 message,
                                                                 QMessageBox::No |
                                                                 QMessageBox::Yes,
                                                                 QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
        return true;
    else
        return false;
}


void WinCredits::on_btn_back_clicked()
{
    this->close();
}
