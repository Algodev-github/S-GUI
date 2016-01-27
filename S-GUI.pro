#-------------------------------------------------
#
# Project created by Algodev group
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = S-GUI
TEMPLATE = app


SOURCES += main.cpp\
        winmain.cpp \
    winsettings.cpp \
    utilities.cpp \
    wininfotests.cpp \
    wintestplot.cpp \
    qcustomplot.cpp \
    wincredits.cpp

HEADERS  += winmain.h \
    winsettings.h \
    data.h \
    utilities.h \
    wininfotests.h \
    qcustomplot.h \
    wintestplot.h \
    wincredits.h

FORMS    += winmain.ui \
    winsettings.ui \
    wininfotests.ui \
    wintestplot.ui \
    wincredits.ui

RESOURCES += \
    bfqresources.qrc

CONFIG += c++11 console
