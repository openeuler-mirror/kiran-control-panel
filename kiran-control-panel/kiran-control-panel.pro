#-------------------------------------------------
#
# Project created by QtCreator 2020-11-09T15:26:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiran-control-panel
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
    kiran-control-panel-window.cpp \
    kiran-control-panel.cpp \
    ../global/kiran-control-panel-global.cpp \
    kiran-module-class-listwidget.cpp \
    kiran-module-class-listwidget-itemwidget.cpp

HEADERS += \
    kiran-control-panel-window.h \
    kiran-control-panel.h \
    ../global/kiran-control-panel-global.h \
    kiran-module-class-listwidget.h \
    kiran-module-class-listwidget-itemwidget.h \
    ../interface/kiran-control-panel-module-interface.h \
    ../interface/kiran-control-panel-module-interface.h

FORMS += \
    kiran-control-panel.ui

include(../widgets/kiran-module-widget/kiran-module-widget.pri)
include(../widgets/kiran-search-lineedit/kiran-search-lineedit.pri)
INCLUDEPATH += ../widgets \
                ../global

LIBS=-ldl

TRANSLATIONS += \
    kiran-control-panel.zh_CN.ts \
    kiran-control-panel.en_US.ts


LIBS += -L/usr/lib -lkiranwidgets-qt5


isEmpty( LIB_DIR ){
    LIB_DIR = "/usr/bin/"
}

target.path = $$DESTDIR/$${LIB_DIR}

translate.files += kiran-control-panel.zh_CN.qm \
                kiran-control-panel.en_US.qm
translate.path = /usr/share/kiran-control-panel

interface.files += ../interface/kiran-control-panel-module-interface.h
interface.path = /usr/include

desktop_files.path = /usr/share/kiran-control-panel/categories
desktop_files.files += desktop/files/*

desktop_images.path = /usr/share/kiran-control-panel/icons
desktop_images.files += desktop/images/*

INSTALLS += target \
            translate \
            interface \
            desktop_files \
            desktop_images


RESOURCES += \
    image.qrc

