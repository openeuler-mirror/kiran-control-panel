#-------------------------------------------------
#
# Project created by QtCreator 2021-08-20T14:32:49
#
#-------------------------------------------------

QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiran-cpanel-keyboard
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
LIBS += \
    -lkiranwidgets-qt5\
    -lklog-qt5

INCLUDEPATH += /usr/include/kiranwidgets-qt5/ \
               /usr/include/kiran-cc-daemon

SOURCES += \
        main.cpp \
        kcp-window.cpp \
    general-page.cpp \
    layout-page.cpp \
    dbus-wrapper/dbus-wrapper.cpp \
    widgets/choose-item.cpp \
    widgets/layout-item.cpp \
    widgets/layout-list.cpp \
    dbus-wrapper/KeyboardBackEndProxy.cpp

HEADERS += \
        kcp-window.h \
    general-page.h \
    layout-page.h \
    dbus-wrapper/dbus-wrapper.h \
    widgets/choose-item.h \
    widgets/layout-item.h \
    widgets/layout-list.h \
    dbus-wrapper/KeyboardBackEndProxy.h

FORMS += \
        kcp-window.ui \
    general-page.ui \
    layout-page.ui \
    widgets/choose-item.ui \
    widgets/layout-item.ui \
    widgets/layout-list.ui

TRANSLATIONS    +=  ../translation/kiran-cpanel-keyboard.zh_CN.ts

#安装选项
target.path = $$DESTDIR/usr/bin/

target_translation.files = ../translation/kiran-cpanel-keyboard.zh_CN.qm
target_translation.path = $$DESTDIR/usr/share/kiran-cpanel-keyboard/translation/

INSTALLS = target target_translation

RESOURCES += \
    ../resource/resources.qrc
