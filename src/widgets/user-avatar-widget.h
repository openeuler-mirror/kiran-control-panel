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
