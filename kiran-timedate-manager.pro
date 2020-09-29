QT       += core gui svg dbus network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += link_pkgconfig

TARGET = kiran-timedate-manager

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    disable-deselect-listwidget.cpp \
    kiran-clock.cpp \
    kiran-date-picker-widget.cpp \
    kiran-timedate-global-data.cpp \
    kiran-timedate-widget.cpp \
    kiran-time-picker-widget.cpp \
    kiran-time-zone.cpp \
    kiran-time-zone-item.cpp \
    kiran-time-zone-list.cpp \
    libs/SingleApplication/singleapplication.cpp \
    libs/SingleApplication/singleapplication_p.cpp \
    log.cpp \
    main.cpp \
    scroll-picker.cpp \
    tab-item.cpp \
    timedate-interface.cpp

HEADERS += \
    date-spinbox.h \
    disable-deselect-listwidget.h \
    kiran-clock.h \
    kiran-date-picker-widget.h \
    kiran-timedate-global-data.h \
    kiran-timedate-widget.h \
    kiran-time-picker-widget.h \
    kiran-time-zone.h \
    kiran-time-zone-item.h \
    kiran-time-zone-list.h \
    libs/SingleApplication/singleapplication.h \
    libs/SingleApplication/singleapplication_p.h \
    log.h \
    scroll-picker.h \
    tab-item.h \
    timedate-interface.h

FORMS += \
    kiran-date-picker-widget.ui \
    kiran-timedate-widget.ui \
    kiran-time-picker-widget.ui \
    kiran-time-zone-item.ui \
    kiran-time-zone-list.ui \
    kiran-time-zone.ui \
    tab-item.ui

RESOURCES += \
    images.qrc \
    themes.qrc

TRANSLATIONS += \
    ./translations/kiran-timedate-manager.zh_CN.ts

OTHER_FILES += \
    README.md \
    translations/kiran-timedate-manager.zh_CN.qm \
    config/kiran-timedate-manager.desktop

INCLUDEPATH += ./libs/

LIBS += -lX11 -lXrandr -lkiranwidgets-qt5

PKGCONFIG += gsettings-qt

target_translation.files = ./translations/kiran-timedate-manager.zh_CN.qm
target_translation.path = $$DESTDIR/usr/share/kiran-timedate-manager/translations/

target_config.files = ./config/kiran-timedate-manager.desktop
target_config.path = $$DESTDIR/usr/share/applications/

target.path = $$DESTDIR/usr/bin/

INSTALLS =  target target_translation target_config
