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

#ifndef USERAVATARWIDGET_H
#define USERAVATARWIDGET_H

#include <QWidget>

class UserAvatarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserAvatarWidget(QWidget *parent = nullptr);
    virtual ~UserAvatarWidget() = default;

    void setImage(const QString &path);
    void setDefaultImage();
    void setClickEnable(bool enable);
    bool setHoverImage(const QString &path);
    QString iconPath();

signals:
    void pressed();

public slots:

protected:
    virtual void paintEvent(QPaintEvent *event) override final;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override final;

protected:
    QPixmap generateUserIconFitWidgetSize(const QPixmap &pixmap);

private:
    QPixmap scalePixmapAdjustSize(const QPixmap &pixmap);

private:
    QPixmap m_scaledPixmap;
    QPixmap m_pixmap;
    QPixmap m_hoverPixmap;
    QPixmap m_hoverScaledPixmap;
    bool m_clickedEnable;
    bool m_isEnter;
    bool m_isHover;
    QString m_iconPath;
};

#endif  // USERAVATARWIDGET_H
