 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
  */
 
#ifndef KIRAN_AVATAR_EDITOR_IMAGE_PREVIEW_WIDGET_H
#define KIRAN_AVATAR_EDITOR_IMAGE_PREVIEW_WIDGET_H

#include <QPixmap>
#include <QWidget>

class ImagePreviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImagePreviewWidget(QWidget *parent = nullptr);
    ~ImagePreviewWidget();

    void setSrcImagePath(const QString &imagePath);
    QString srcImagePath();

    void setDstImagePath(const QString &imagePath);
    QString dstImagePath();

    bool saveAvatar();

private:
    void loadSrcImage();
    //图片位置校验
    bool imagePositionVerification();
    //图片位置不合法，调整位置
    void imageAutomaticPosition();
    //图片放大
    void pixmapEnlarge();
    //图片缩小
    void pixmapReduce();

    QPixmap generateCircleRectPixmap();
    QPixmap generateCircleAvatar(const QPixmap &pixmap);

private Q_SLOTS:
    void pixmapCentering();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    //原图像路径
    QString m_srcImagePath;
    //目标图像路径
    QString m_dstImagePath;
    //原始图片
    QPixmap m_originalPixmap;
    //当前拉伸的图片
    QPixmap m_scaledPixmap;
    //当前拉伸的比例
    double m_pixmapScaleFactor = 1.0;
    //图片的左顶点
    QPoint m_pixmapLefttop;
    bool m_btnIsPressed;
    QPoint m_btnPressedPoint;
};

#endif  //KIRAN_AVATAR_EDITOR_IMAGE_PREVIEW_WIDGET_H
