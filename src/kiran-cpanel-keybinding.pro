#-------------------------------------------------
#
# Project created by QtCreator 2021-09-23T09:40:54
#
#-------------------------------------------------

QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiran-cpanel-keybinding
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
        main-window.cpp \
    shortcut.cpp \
    shortcut-item.cpp \
    custom-line-edit.cpp

HEADERS += \
        main-window.h \
    shortcut.h \
    shortcut-item.h \
    keybinding_def.h \
    custom-line-edit.h

FORMS += \
        main-window.ui \
    shortcut.ui \
    shortcut-item.ui \
    custom-line-edit.ui

RESOURCES += \
    ../resources/resources.qrc


TRANSLATIONS    +=  ../translation/kiran-cpanel-keybinding.zh_CN.ts

#安装选项
target.path = $$DESTDIR/usr/bin/

target_translation.files = ../translation/kiran-cpanel-keybinding.zh_CN.qm
target_translation.path = $$DESTDIR/usr/share/kiran-cpanel-keybinding/translation/

INSTALLS = target target_translation
