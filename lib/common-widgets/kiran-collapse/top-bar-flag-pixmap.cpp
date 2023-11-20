//
// Created by skyzcyou on 2023/11/15.
//

#include "top-bar-flag-pixmap.h"
#include "constants.h"

QPixmap FlagPixmap::expansionFlagPixmap() {
    QImage image(RIGHT_ARROW);
    QImage transformedImage = rotateImage(image, 90.0);
    return QPixmap::fromImage(transformedImage);
}

QPixmap FlagPixmap::collapseFlagPixmap() {
    QImage image(RIGHT_ARROW);
    QImage transformedImage = rotateImage(image, -90.0);
    return QPixmap::fromImage(transformedImage);
}

QPixmap FlagPixmap::expansionFlagPixmapDark() {
    QImage image(RIGHT_ARROW);
    QImage transformedImage = rotateImage(image, 90.0);
    invertColors(transformedImage);
    return QPixmap::fromImage(transformedImage);
}

QPixmap FlagPixmap::collapseFlagPixmapDark() {
    QImage image(RIGHT_ARROW);
    QImage transformedImage = rotateImage(image, -90.0);
    invertColors(transformedImage);
    return QPixmap::fromImage(transformedImage);
}

QImage FlagPixmap::rotateImage(const QImage& image, qreal angle) {
    QMatrix matrix;
    matrix.rotate(angle);
    return image.transformed(matrix, Qt::FastTransformation);
}

void FlagPixmap::invertColors(QImage& image) {
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor color = image.pixelColor(x, y);
            QColor invertedColor = QColor(255 - color.red(), 255 - color.green(), 255 - color.blue(), color.alpha());
            image.setPixelColor(x, y, invertedColor);
        }
    }
}