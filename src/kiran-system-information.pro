#-------------------------------------------------
#
# Project created by QtCreator 2021-02-02T14:54:15
#
#-------------------------------------------------

QT       += core gui \
            network \
            dbus

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
CONFIG += c++11

CONFIG +=link_pkgconfig

PKGCONFIG += glib-2.0

#包含头文件目录 -I
INCLUDEPATH += \
    /usr/include/glib-2.0/ \
    /usr/lib64/glib-2.0/include/

SOURCES += \
        main.cpp \
        kiran-system-information.cpp \
    information-list-item.cpp \
    hardware-information-widget.cpp \
    general-functions-class.cpp \
    single/singleapplication.cpp \
    single/singleapplication_p.cpp \
    system-info-dbus.cpp \
    system-information-widget.cpp \
    license/active-guide-widget.cpp \
    license/show-qrcode.cpp \
    license/qrcode-widget.cpp \
    license/active-list-item.cpp \
    license/load-animation-widget.cpp \
    license/license-info-widget.cpp \
    license/user-license-agreement.cpp \
    kiran-system-widget.cpp

HEADERS += \
        kiran-system-information.h \
    information-list-item.h \
    hardware-information-widget.h \
    general-functions-class.h \
    single/singleapplication.h \
    single/singleapplication_p.h \
    system-info-dbus.h \
    system-information-widget.h \
    license/active-guide-widget.h \
    license/show-qrcode.h \
    license/qrcode-widget.h \
    license/active-list-item.h \
    license/load-animation-widget.h \
    license/license-info-widget.h \
    license/user-license-agreement.h \
    kiran-system-widget.h

FORMS += \
        kiran-system-information.ui \
    information-list-item.ui \
    hardware-information-widget.ui \
    system-information-widget.ui \
    license/active-guide-widget.ui \
    license/show-qrcode.ui \
    license/active-list-item.ui \
    license/license-info-widget.ui \
    license/user-license-agreement.ui

RESOURCES += \
    ../data/res.qrc \
    ../data/loading-data.qrc

LIBS += \
        -lQZint \
        -lglib-2.0\
        -lkiranwidgets-qt5

TRANSLATIONS    +=  ../translations/kiran-system-information.zh_CN.ts

#安装选项
target.path = $$DESTDIR/usr/bin/

target_translation.files = ../translations/kiran-system-information.zh_CN.qm
target_translation.path = $$DESTDIR/usr/share/kiran-system-information/translations/

INSTALLS = target target_translation

DISTFILES += \
    ../translations/kiran-system-information.zh_CN.ts \
    ../translations/kiran-system-information.zh_CN.qm \

DEFINES += QT_MESSAGELOGCONTEXT









