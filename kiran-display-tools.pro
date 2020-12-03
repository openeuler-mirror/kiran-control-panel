#-------------------------------------------------
#
# Project created by QtCreator 2020-10-19T15:01:28
#
#-------------------------------------------------

QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiran-display-tools

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
        kiran-display-configuration.cpp \
    kiran-display-configuration-window.cpp \
    kiran-display-config-item-contain.cpp \
    kiran-display-configuration-panel.cpp \
    kiran-display-config-item.cpp \
    kiran-combobox.cpp \
    kiran-display-config-global.cpp \
    kiran-display-config-identifying.cpp \
    interface.cpp \
    kiran-display-module-base.cpp

HEADERS += \
        kiran-display-configuration.h \
    kiran-display-configuration-window.h \
    kiran-display-config-item-contain.h \
    kiran-display-configuration-panel.h \
    kiran-display-config-item.h \
    kiran-combobox.h \
    kiran-display-config-global.h \
    kiran-display-config-identifying.h \
    interface.h \
    kiran-display-module-base.h

FORMS += \
        kiran-display-configuration.ui \
    kiran-display-configuration-panel.ui

RESOURCES += \
    image.qrc


TRANSLATIONS += \
    kiran-display-tools.zh_CN.ts \
    kiran-display-tools.en_US.ts


LIBS += -L/usr/lib -lkiranwidgets-qt5

#TEMPLATE = app
TEMPLATE = lib

equals(TEMPLATE, app){
SOURCES += main.cpp
isEmpty( LIB_DIR ){
    LIB_DIR = "/usr/bin/"
}
}

equals(TEMPLATE, lib){
isEmpty( LIB_DIR ){
    LIB_DIR = "/usr/lib/"
}
}


target.path = $$DESTDIR/$${LIB_DIR}

translate.files += kiran-display-tools.zh_CN.qm \
                kiran-display-tools.en_US.qm
translate.path = /usr/share/kiran-control-panel/plugins/kiran-display-tools/translate


DESKTOP_FILES_PATH = "/usr/share/kiran-control-panel/plugins"
desktop_files.path = $${DESKTOP_FILES_PATH}
desktop_files.files += desktop/files/*
DEFINES += DESKTOP_FILES_PATHDEFINES=$${DESKTOP_FILES_PATH}

DESKTOP_IMAGES_PATH = "/usr/share/kiran-control-panel/plugins/kiran-display-tools/icons"
desktop_images.path = $${DESKTOP_IMAGES_PATH}
desktop_images.files += desktop/images/*
DEFINES += DESKTOP_IMAGES_PATH_DEFINES=$${DESKTOP_IMAGES_PATH}


INSTALLS += target \
            translate \
            desktop_files \
            desktop_images
