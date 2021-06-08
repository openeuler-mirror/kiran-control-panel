#-------------------------------------------------
#
# Project created by QtCreator 2021-04-20T11:10:10
#
#-------------------------------------------------

QT       += core gui \
            network \
            dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiran-cpanel-mouse
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
INCLUDEPATH += \
    /usr/include/kiranwidgets-qt5 \
    /usr/include/kiran-cc-daemon

CONFIG += C++11

SOURCES += \
        main.cpp \
        kiran-cpanel-mouse.cpp \
    tools/log.cpp \
    kiran-cpanel-mouse-widget.cpp \
    dbus-interface/mouse-interface.cpp \
    dbus-interface/touchpad-interface.cpp \
    pages/page-mouse/mouse-settings.cpp \
    general-functions/general-function-class.cpp \
    pages/page-touchpad/touchpad-settings.cpp \
    plugin_build/interface.cpp

HEADERS += \
        kiran-cpanel-mouse.h \
    tools/log.h \
    kiran-cpanel-mouse-widget.h \
    dbus-interface/mouse-interface.h \
    dbus-interface/touchpad-interface.h \
    pages/page-mouse/mouse-settings.h \
    general-functions/general-function-class.h \
    pages/page-touchpad/touchpad-settings.h \
    plugin_build/interface.h

FORMS += \
    kiran-cpanel-mouse-widget.ui \
    pages/page-mouse/mouse-settings.ui \
    pages/page-touchpad/touchpad-settings.ui

LIBS  += -lkiranwidgets-qt5\
         -lzlog

TRANSLATIONS    +=  ../translations/kiran-cpanel-mouse.zh_CN.ts

#安装选项
target.path = $$DESTDIR/usr/bin/

target_translation.files = ../translations/kiran-cpanel-mouse.zh_CN.qm
target_translation.path = $$DESTDIR/usr/share/kiran-cpanel-mouse/translations/

INSTALLS = target target_translation

RESOURCES += \
    ../resources/resources.qrc

