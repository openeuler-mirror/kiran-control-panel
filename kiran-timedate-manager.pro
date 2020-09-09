QT       += core gui svg dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    timedate-interface.cpp \
    disable-deselect-listwidget.cpp \
    kiran-clock.cpp \
    kiran-date-setting-widget.cpp \
    kiran-timedate-global-data.cpp \
    kiran-time-setting-widget.cpp \
    kiran-time-zone-item.cpp \
    kiran-time-zone-list.cpp \
    kiran-time-zone.cpp \
    log.cpp \
    main.cpp \
    kiran-date-time-widget.cpp \
    tab-item.cpp

HEADERS += \
    timedate-interface.h \
    disable-deselect-listwidget.h\
    kiran-clock.h \
    kiran-date-setting-widget.h \
    kiran-date-time-widget.h \
    kiran-timedate-global-data.h \
    kiran-time-setting-widget.h \
    kiran-time-zone-item.h \
    kiran-time-zone-list.h \
    kiran-time-zone.h \
    log.h \
    tab-item.h

FORMS += \
    kiran-date-setting-widget.ui \
    kiran-date-time-widget.ui \
    kiran-time-setting-widget.ui \
    kiran-time-zone-item.ui \
    kiran-time-zone-list.ui \
    kiran-time-zone.ui \
    tab-item.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc \
    themes.qrc
