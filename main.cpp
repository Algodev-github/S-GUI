#include "winmain.h"
#include <QApplication>
#include "data.h"

/**
 * @brief Default advanced settings
 */
advanced_settings_t default_adv;
/**
 * @brief Main test settings
 */
test_settings_t test_sets;
/**
 * @brief Path to the benchmark-suite
 */
QString dir_bench="benchmark-suite/";


int main(int argc, char *argv[])
{
     /*Check if the application is launched with right permissions(root=0)*/
    if (getuid())
    {
       printf("No root privileges detected, aborting...\n");
       exit(NO_ROOT_PRIVILEGES_ERROR);
    }
    QApplication S(argc, argv);
    S.setWindowIcon(QIcon(":/algo-dev.png"));
    WinMain Win_main;
    Win_main.show();

    return S.exec();
}
