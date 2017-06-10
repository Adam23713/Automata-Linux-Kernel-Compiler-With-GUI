#-------------------------------------------------
#
# Project created by QtCreator 2016-11-05T11:14:53
#
#-------------------------------------------------

QT       += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LinuxKernelUpdate
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    kernelinformation.cpp \
    filedownloader.cpp

HEADERS  += mainwindow.h \
    kernelinformation.h \
    filedownloader.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc

DISTFILES += \
    Comment

LIBS += -L"/opt/Qt/5.7/gcc_64/lib/cmake/Qt5Widgets/lib/" -lqtermwidget5
