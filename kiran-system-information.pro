#-------------------------------------------------
#
# Project created by QtCreator 2021-02-02T14:54:15
#
#-------------------------------------------------

QT       += core gui \
            network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiran-system-information
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        kiran-system-information.cpp \
    information-list-item.cpp \
    hardware-information-widget.cpp \
    general-functions-class.cpp \
    single/singleapplication.cpp \
    single/singleapplication_p.cpp

HEADERS += \
        kiran-system-information.h \
    information-list-item.h \
    hardware-information-widget.h \
    general-functions-class.h \
    single/singleapplication.h \
    single/singleapplication_p.h

FORMS += \
        kiran-system-information.ui \
    information-list-item.ui \
    hardware-information-widget.ui

RESOURCES += \
    res.qrc
