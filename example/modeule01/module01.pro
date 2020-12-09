#-------------------------------------------------
#
# Project created by QtCreator 2020-11-17T16:47:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = module01

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    dialog001.cpp \
    dialog002.cpp \
    main.cpp \
    popup.cpp

HEADERS += \
    dialog001.h \
    dialog002.h \
    popup.h

FORMS += \
    dialog001.ui \
    dialog002.ui \
    popup.ui


TRANSLATIONS += \
    module01.zh_CN.ts \
    module01.en_US.ts

translate.files += module01.zh_CN.qm \
                module01.en_US.qm
translate.path = /usr/share/kiran-control-panel/plugins/translate

equals(TEMPLATE, app){
    SOURCES += main.cpp
    isEmpty( LIB_DIR ){
        LIB_DIR = "/usr/bin/"
    }

    target.path = $$DESTDIR/$${LIB_DIR}
    DESKTOP_FILES_PATH = "/usr/share/applications"
    desktop_files.path = $${DESKTOP_FILES_PATH}
    desktop_files.files += desktop/files/*
    DEFINES += DESKTOP_FILES_PATHDEFINES=$${DESKTOP_FILES_PATH}

    DESKTOP_IMAGES_PATH = "/usr/share/icons/Kiran/places/16x16/"
    desktop_images.path = $${DESKTOP_IMAGES_PATH}
    desktop_images.files += desktop/images/*
    DEFINES += DESKTOP_IMAGES_PATH_DEFINES=$${DESKTOP_IMAGES_PATH}

    INSTALLS += target \
            translate \
            desktop_files \
            desktop_images
}

equals(TEMPLATE, lib){
    HEADERS += module01-interface.h
    SOURCES += module01-interface.cpp
    isEmpty( LIB_DIR ){
        LIB_DIR = "/usr/lib/"
    }

    target.path = $$DESTDIR/$${LIB_DIR}
    DESKTOP_FILES_PATH = "/usr/share/kiran-control-panel/plugins"
    desktop_files.path = $${DESKTOP_FILES_PATH}
    desktop_files.files += desktop/files/*

    DESKTOP_IMAGES_PATH = "/usr/share/kiran-control-panel/plugins/icons"
    desktop_images.path = $${DESKTOP_IMAGES_PATH}
    desktop_images.files += desktop/images/*

    INSTALLS += target \
                translate \
                desktop_files \
                desktop_images
}
