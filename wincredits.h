#ifndef WINCREDITS_H
#define WINCREDITS_H

#include <QDialog>

namespace Ui {
class WinCredits;
}

class WinCredits : public QDialog
{
    Q_OBJECT

public:
    explicit WinCredits(QWidget *parent = 0);
    bool attention();
    ~WinCredits();

private slots:


    void on_btn_back_clicked();

private:
    Ui::WinCredits *ui;
};

#endif // WINCREDITS_H
