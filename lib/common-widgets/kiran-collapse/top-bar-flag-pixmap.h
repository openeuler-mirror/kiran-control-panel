#pragma once

#include <QPixmap>

class FlagPixmap
{
public:
    // 默认展开图标
    static QPixmap expansionFlagPixmap();
    static QPixmap expansionFlagPixmapDark();
    // 默认折叠图标
    static QPixmap collapseFlagPixmap();
    static QPixmap collapseFlagPixmapDark();

private:
    static QImage rotateImage(const QImage& image, qreal angle);
    static void invertColors(QImage& image);
};
