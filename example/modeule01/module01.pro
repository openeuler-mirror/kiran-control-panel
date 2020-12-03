#-------------------------------------------------
#
# Project created by QtCreator 2020-11-17T16:47:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = module01
TEMPLATE = lib #编译成库文件

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
    module01-interface.cpp \
    main.cpp

HEADERS += \
    dialog001.h \
    dialog002.h \
    module01-interface.h

FORMS += \
    dialog001.ui \
    dialog002.ui


TRANSLATIONS += \
    module01.zh_CN.ts \
    module01.en_US.ts

equals(TEMPLATE, lib){
isEmpty( LIB_DIR ){
    LIB_DIR = "/usr/lib/"
}
}
target.path = $$DESTDIR/$${LIB_DIR}

translate.files += module01.zh_CN.qm \
                module01.en_US.qm
translate.path = /usr/share/kiran-control-panel/plugins/translate

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
