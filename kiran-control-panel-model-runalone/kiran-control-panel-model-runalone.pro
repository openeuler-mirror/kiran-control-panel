#-------------------------------------------------
#
# Project created by QtCreator 2020-11-18T09:58:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiran-control-mode-runalone
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
    ../global/kiran-control-panel-global.cpp \
    kiran-control-panel-model-runalone-window.cpp

HEADERS += \
    ../global/kiran-control-panel-global.h \
    kiran-control-panel-model-runalone-window.h

FORMS +=

include(../widgets/kiran-model-widget/kiran-model-widget.pri)
INCLUDEPATH += ../widgets \
                ../global

LIBS=-ldl
LIBS += -L/usr/share -lkiranwidgets-qt5

