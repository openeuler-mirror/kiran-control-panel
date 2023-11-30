/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
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
