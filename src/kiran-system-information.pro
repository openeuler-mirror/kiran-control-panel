#-------------------------------------------------
#
# Project created by QtCreator 2021-02-02T14:54:15
#
#-------------------------------------------------

QT       += core gui \
            network \
            dbus    \
            printsupport

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
    custom-widgets/information-list-item.cpp \
    custom-widgets/kiran-list-widget.cpp \
    custom-widgets/kiran-scroll-area.cpp \
    dbus-wrapper/license-dbus.cpp \
    dbus-wrapper/system-info-dbus.cpp \
        main.cpp \
        kiran-system-information.cpp \
    pages/hardware-information/hardware-information.cpp \
    pages/license-information/license-information.cpp \
    pages/system-information/change-host-name-widget.cpp \
    pages/system-information/system-information.cpp \
    license/active-guide-widget.cpp \
    license/show-qrcode.cpp \
    license/qrcode-widget.cpp \
    license/active-list-item.cpp \
    license/load-animation-widget.cpp \
    license/license-info-widget.cpp \
    kiran-system-widget.cpp \
    pages/hardware-information/hardware-information-widget.cpp \
    pages/system-information/system-information.cpp \
    plug_interface/interface.cpp \
    license/license-agreement.cpp

HEADERS += \
    custom-widgets/information-list-item.h \
    custom-widgets/kiran-list-widget.h \
    custom-widgets/kiran-scroll-area.h \
    dbus-wrapper/license-dbus.h \
    dbus-wrapper/system-info-dbus.h \
        kiran-system-information.h \
    hardware-information-widget.h \
    kiran-system-widget.h \
    pages/hardware-information/hardware-information.h \
    pages/license-information/license-information.h \
    pages/system-information/change-host-name-widget.h \
    pages/system-information/system-information.h \
    license/active-guide-widget.h \
    license/show-qrcode.h \
    license/qrcode-widget.h \
    license/active-list-item.h \
    license/load-animation-widget.h \
    license/license-info-widget.h \
    plug_interface/interface.h \
    license/license-agreement.h

FORMS += \
    custom-widgets/information-list-item.ui \
        kiran-system-information.ui \
    hardware-information-widget.ui \
    pages/hardware-information/hardware-information.ui \
    pages/license-information/license-information.ui \
    pages/system-information/change-host-name-widget.ui \
    pages/system-information/system-information.ui \
    license/active-guide-widget.ui \
    license/show-qrcode.ui \
    license/license-info-widget.ui \
    pages/hardware-information/hardware-information-widget.ui \
    pages/system-information/system-information.ui \
    license/license-agreement.ui

RESOURCES += \
    ../data/res.qrc \
    ../data/loading-data.qrc

LIBS += \
        -lQZint \
        -lglib-2.0\
        -lkiranwidgets-qt5

TRANSLATIONS    +=  ../translations/kiran-cpanel-system.zh_CN.ts

#安装选项
target.path = $$DESTDIR/usr/bin/

target_translation.files = ../translations/kiran-cpanel-system.zh_CN.qm
target_translation.path = $$DESTDIR/usr/share/kiran-system-information/translations/

INSTALLS = target target_translation

DISTFILES += \
    ../translations/kiran-cpanel-system.zh_CN.ts \
    ../translations/kiran-cpanel-system.zh_CN.qm \

DEFINES += QT_MESSAGELOGCONTEXT









