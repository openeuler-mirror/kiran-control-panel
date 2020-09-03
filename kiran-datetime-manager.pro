QT       += core gui svg dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SystemDaemonTimeDate.cpp \
    disabledeselectlistwidget.cpp \
    kiran-clock.cpp \
    kiran-date-setting-widget.cpp \
    kiran-time-setting-widget.cpp \
    kiran-time-zone-item.cpp \
    kiran-time-zone-list.cpp \
    kiran-time-zone.cpp \
    log.cpp \
    main.cpp \
    kiran-date-time-widget.cpp \
    tab-item.cpp

HEADERS += \
    SystemDaemonTimeDate.h \
    disabledeselectlistwidget.h \
    kiran-clock.h \
    kiran-date-setting-widget.h \
    kiran-date-time-widget.h \
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
