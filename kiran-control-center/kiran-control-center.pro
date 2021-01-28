#-------------------------------------------------
#
# Project created by QtCreator 2020-11-09T15:26:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiran-control-center
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
    kiran-module-class-listwidget.cpp \
    kiran-module-class-listwidget-itemwidget.cpp \
    kiran-module-class-widget.cpp \
    kiran-control-center-window.cpp \
    kiran-control-center.cpp \
    ../global/kiran-control-center-global.cpp

HEADERS += \
    kiran-module-class-listwidget.h \
    kiran-module-class-listwidget-itemwidget.h \
    kiran-module-class-widget.h \
    ../interface/module-interface.h \
    kiran-control-center-window.h \
    kiran-control-center.h \
    ../global/kiran-control-center-global.h

FORMS += \
    kiran-module-class-widget.ui \
    kiran-control-center.ui

include(../widgets/kiran-module-widget/kiran-module-widget.pri)
include(../widgets/kiran-search-lineedit/kiran-search-lineedit.pri)
INCLUDEPATH += ../widgets \
                ../global

LIBS=-ldl

TRANSLATIONS += \
    kiran-control-center.zh_CN.ts \
    kiran-control-center.en_US.ts


LIBS += -L/usr/lib -lkiranwidgets-qt5


isEmpty( LIB_DIR ){
    LIB_DIR = "/usr/bin/"
}

target.path = $$DESTDIR/$${LIB_DIR}

translate.files += kiran-control-panel.zh_CN.qm \
                kiran-control-panel.en_US.qm
translate.path = /usr/share/kiran-control-panel

interface.files += ../interface/module-interface.h
interface.path = /usr/include/kiran-control-panel

desktop_files.path = /usr/share/applications
desktop_files.files += app-desktop/files/*

desktop_images.path = /usr/share/kiran-control-panel/icons
desktop_images.files += app-desktop//images/*

desktop_files_categories.path = /usr/share/kiran-control-panel/categories
desktop_files_categories.files += categories-desktop/files/*

desktop_images_categories.path = /usr/share/kiran-control-panel/icons
desktop_images_categories.files += categories-desktop/images/*

INSTALLS += target \
            translate \
            interface \
            desktop_files \
            desktop_images \
            desktop_files_categories \
            desktop_images_categories


RESOURCES += \
    image.qrc

