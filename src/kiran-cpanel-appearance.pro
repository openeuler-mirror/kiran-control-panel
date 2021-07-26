#-------------------------------------------------
#
# Project created by QtCreator 2021-05-25T15:36:48
#
#-------------------------------------------------

QT       += core gui network dbus \
            KI18n KConfigCore  x11extras \
            svg xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiran-cpanel-appearance
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

INCLUDEPATH += \
    /usr/include/kiranwidgets-qt5 \
    /usr/include/kiran-cc-daemon

LIBS  += \
        -lkiranwidgets-qt5 \
        -lklog-qt5 \
        -lX11 \
        -lXfixes \
        -lXcursor

SOURCES += \
        main.cpp \
        kiran-cpanel-appearance.cpp \
    pages/font/fonts.cpp \
    dbus-interface/Appearance.cpp \
    dbus-interface/appearance-global-info.cpp \
    pages/theme/themes.cpp \
    pages/theme/theme-widget.cpp \
    pages/theme/theme-widget-group.cpp \
    pages/theme/cursor-themes/cursor/cursortheme.cpp \
    pages/theme/cursor-themes/cursor/xcursortheme.cpp \
    pages/theme/cursor-themes/cursor-themes.cpp \
    pages/theme/icon-themes/icon-themes.cpp \
    pages/wallpaper/wallpaper.cpp \
    common/chooser-widget.cpp \
    pages/wallpaper/widget/preview-label.cpp \
    pages/wallpaper/widget/flowlayout.cpp \
    pages/wallpaper/widget/image-selector.cpp \
    pages/wallpaper/widget/kiran-image-item.cpp \
    pages/wallpaper/widget/kiran-image-load-manager.cpp \
    pages/wallpaper/widget/xml-management/thread-object.cpp \
    pages/wallpaper/widget/xml-management/xml-management.cpp

HEADERS += \
        kiran-cpanel-appearance.h \
    pages/font/fonts.h \
    dbus-interface/Appearance.h \
    dbus-interface/appearance-global-info.h \
    pages/theme/themes.h \
    pages/theme/theme-widget.h \
    pages/theme/theme-widget-group.h \
    pages/theme/cursor-themes/cursor/config-X11.h \
    pages/theme/cursor-themes/cursor/cursortheme.h \
    pages/theme/cursor-themes/cursor/xcursortheme.h \
    pages/theme/cursor-themes/cursor-themes.h \
    pages/theme/icon-themes/icon-themes.h \
    pages/wallpaper/wallpaper.h \
    common/chooser-widget.h \
    pages/wallpaper/widget/preview-label.h \
    pages/wallpaper/widget/flowlayout.h \
    pages/wallpaper/widget/image-selector.h \
    pages/wallpaper/widget/kiran-image-item.h \
    pages/wallpaper/widget/kiran-image-load-manager.h \
    pages/wallpaper/widget/xml-management/thread-object.h \
    pages/wallpaper/widget/xml-management/xml-management.h \
    pages/wallpaper/wallpaper-global.h

FORMS += \
        kiran-cpanel-appearance.ui \
    pages/font/fonts.ui \
    pages/theme/themes.ui \
    pages/theme/icon-themes/icon-themes.ui \
    pages/wallpaper/wallpaper.ui \
    common/chooser-widget.ui

TRANSLATIONS    +=  ../translations/kiran-cpanel-appearance.zh_CN.ts

#安装选项
target.path = $$DESTDIR/usr/bin/

target_translation.files = ../translations/kiran-cpanel-appearance.zh_CN.qm
target_translation.path = $$DESTDIR/usr/share/kiran-cpanel-appearance/translations/

INSTALLS = target target_translation

DISTFILES += \
    ../translations/kiran-cpanel-appearance.zh_CN.qm

RESOURCES += \
    ../resources/resources.qrc



