QT       += core gui svg dbus

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
    log.cpp \
    main.cpp \
    scaling-helper.cpp \
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
    log.h \
    scaling-helper.h \
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
    translations/kiran-timedate-manager.zh_CN.qm \
    config/kiran-timedate-manager.desktop

PKGCONFIG += gsettings-qt

LIBS += -lX11 -lXrandr

LRELEASE_PATH=$$[QT_INSTALL_BINS]/lrelease
exists($${LRELEASE_PATH}) {
    message("find lrelease int $${LRELEASE_PATH}")
} else {
    error("can't find lrelease in $${LRELEASE_PATH}")
}
LRELEASE_CMD = $$shell_path($$[QT_INSTALL_BINS]/lrelease) $${PWD}'/'$${TRANSLATIONS}
target_translation.commands = $${LRELEASE_CMD}
target_translation.files = ./translations/*.qm
target_translation.path = $$DESTDIR/usr/share/kiran-timedate-manager/translations/

target_config.files = ./config/kiran-timedate-manager.desktop
target_config.path = $$DESTDIR/usr/share/applications/

target.path = $$DESTDIR/usr/bin/
INSTALLS =  target target_translation target_config
