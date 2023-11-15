#include "cursor-image-loader.h"
#include <X11/Xcursor/Xcursor.h>

static QImage convertToNomalImage(const QImage& cursorImage)
{
    QRect rect(cursorImage.rect().bottomRight(), cursorImage.rect().topLeft());
    const quint32* pixelData = reinterpret_cast<const quint32*>(cursorImage.bits());
    for (int i = 0; i < cursorImage.height(); i++)
    {
        for (int j = 0; j < cursorImage.width(); j++)
        {
            if (*pixelData)
            {
                if (i < rect.left()) rect.setLeft(i);
                if (i > rect.right()) rect.setRight(i);
                if (j < rect.top()) rect.setTop(j);
                if (j > rect.bottom()) rect.setBottom(j);
                pixelData++;
            }
        }
    }
    return cursorImage.copy(rect.normalized());
}

QImage CursorImageLoader::getCursorImage(const QString& cursorTheme, const QString& cursorName, int size)
{
    XcursorImage* xCursorImage = XcursorLibraryLoadImage(cursorName.toStdString().c_str(),
                                                         cursorTheme.toStdString().c_str(),
                                                         size);

    QImage img((uchar*)xCursorImage->pixels,
               xCursorImage->width, xCursorImage->height,
               QImage::Format_ARGB32_Premultiplied);

    QImage cursorImg = convertToNomalImage(img);
    XcursorImageDestroy(xCursorImage);

    return cursorImg;
}
