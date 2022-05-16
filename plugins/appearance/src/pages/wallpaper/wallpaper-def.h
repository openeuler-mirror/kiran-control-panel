#ifndef WALLPAPERGLOBAL_H
#define WALLPAPERGLOBAL_H

enum ImageType
{
    SYSTEM_IMAGE = 0,
    CUSTOM_IMAGE,
    ADDITION_IMAGE
};

enum WallpaperType
{
    DESKTOP = 0,
    LOCK_SCREEN
};

#define SYSTEM_BACKGROUND_PATH "/usr/share/backgrounds/kiran/"
#define LOCAL_WALLPAPER_FILE ".config/kylinsec/kiran-cpanel-appearance/wallpaper.xml"
#define SYSTEM_WALLPAPER_FILE "/usr/share/mate-background-properties/kiran_background.xml"
#define CACHE_IMAGE_DIR ".config/kylinsec/kiran-cpanel-appearance/"

#define FILENAME "filename"
#define NAME "name"
#define DELETED "deleted"
#define ARTIST "artist"
#define PCOLOR "pcolor"
#define SCOLOR "scolor"
#define SHADE_TYPE "shade_type"
#define OPTIONS "options"

#endif  // WALLPAPERGLOBAL_H
