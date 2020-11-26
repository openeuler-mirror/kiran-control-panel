#-------------------------------------------------
#
# Project created by QtCreator 2020-10-19T15:01:28
#
#-------------------------------------------------

QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiran-display-tools
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
SOURCES += \
        main.cpp \
        kiran-display-configuration.cpp \
    kiran-display-configuration-window.cpp \
    kiran-display-config-item-contain.cpp \
    kiran-display-configuration-panel.cpp \
    kiran-display-config-item.cpp \
    kiran-combobox.cpp \
    kiran-display-config-global.cpp \
    kiran-display-config-identifying.cpp

HEADERS += \
        kiran-display-configuration.h \
    kiran-display-configuration-window.h \
    kiran-display-config-item-contain.h \
    kiran-display-configuration-panel.h \
    kiran-display-config-item.h \
    kiran-combobox.h \
    kiran-display-config-global.h \
    kiran-display-config-identifying.h

FORMS += \
        kiran-display-configuration.ui \
    kiran-display-configuration-panel.ui

RESOURCES += \
    image.qrc


TRANSLATIONS += \
    kiran-display-tools.zh_CN.ts \
    kiran-display-tools.en_US.ts


LIBS += -L/usr/lib -lkiranwidgets-qt5


isEmpty( LIB_DIR ){
    LIB_DIR = "/usr/bin/"
}

target.path = $$DESTDIR/$${LIB_DIR}

target1.files += kiran-display-tools.zh_CN.qm \
                kiran-display-tools.en_US.qm
target1.path = /usr/share/kiran-display-tools
target2.files += kiran-display-tools.desktop
target2.path = /usr/share/applications

INSTALLS += target \
            target1 \
            target2
