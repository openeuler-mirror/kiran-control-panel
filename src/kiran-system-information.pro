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

TARGET = kiran-cpanel-system
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

SOURCES += \ \
    custom-widgets/information-list-item.cpp \
    custom-widgets/kiran-list-widget.cpp \
    custom-widgets/kiran-scroll-area.cpp \
    dbus-wrapper/license-dbus.cpp \
    kiran-system-information.cpp \
    license/active-guide-widget.cpp \
    license/active-list-item.cpp \
    license/license-agreement.cpp \
    license/license-info-widget.cpp \
    license/load-animation-widget.cpp \
    license/qrcode-widget.cpp \
    license/show-qrcode.cpp \
    main.cpp \
    pages/license-information/license-information.cpp


HEADERS += \ \
    custom-widgets/information-list-item.h \
    custom-widgets/kiran-list-widget.h \
    custom-widgets/kiran-scroll-area.h \
    dbus-wrapper/license-dbus.h \
    kiran-system-information.h \
    license/active-guide-widget.h \
    license/active-list-item.h \
    license/license-agreement.h \
    license/license-info-widget.h \
    license/load-animation-widget.h \
    license/qrcode-widget.h \
    license/show-qrcode.h \
    pages/license-information/license-information.h


FORMS += \
    custom-widgets/information-list-item.ui \
    kiran-system-information.ui \
    license/active-guide-widget.ui \
    license/active-list-item.ui \
    license/license-agreement.ui \
    license/license-info-widget.ui \
    license/show-qrcode.ui \
    pages/license-information/license-information.ui


RESOURCES += \
    ../data/res.qrc \
    ../data/loading-data.qrc

LIBS += \
        -lqrencode \
        -lglib-2.0\
        -lklog-qt5

TRANSLATIONS    +=  ../translations/kiran-cpanel-system.zh_CN.ts

#安装选项
message($$DESTDIR)
target.path = $$DESTDIR/usr/bin/

target_translation.files = ../translations/kiran-cpanel-system.zh_CN.qm
target_translation.path = $$DESTDIR/usr/share/kiran-cpanel-system/translations/

INSTALLS = target target_translation

DISTFILES += \
    ../translations/kiran-cpanel-system.zh_CN.ts \
    ../translations/kiran-cpanel-system.zh_CN.qm \

DEFINES += QT_MESSAGELOGCONTEXT









