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

#include "user-avatar-widget.h"
#include "kcp-account-config.h"

#include <QFile>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <qt5-log-i.h>

UserAvatarWidget::UserAvatarWidget(QWidget *parent)
    : QWidget(parent),
      m_clickedEnable(false),
      m_isEnter(false),
      m_isHover(false)
{
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
    setDefaultImage();
}

void UserAvatarWidget::setImage(const QString &path)
{
    QFile file(path);
    if (!file.exists())
    {
        KLOG_DEBUG() << "UserAvatar: file path[" << path << "] is no't exist";
    }
    if (m_pixmap.load(path))
    {
        m_scaledPixmap = scalePixmapAdjustSize(m_pixmap);
        m_iconPath = path;
    }
    else
    {
        KLOG_DEBUG() << "UserAvatar: file path[" << path << "] load failed.";
        if (path != DEFAULT_USER_AVATAR)
        {
            setDefaultImage();
        }
    }
    update();
}

void UserAvatarWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen;
    if (!isVisible())
    {
        return;
    }
    painter.setRenderHints(
        QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing | QPainter::Antialiasing);
    double radius = (this->width() < this->height() ? this->width() : this->height()) / 2;
    if (!m_scaledPixmap.isNull())
    {
        QPainterPath painterPath;
        painterPath.addEllipse((this->width() - (radius * 2)) / 2, (this->height() - (radius * 2)) / 2, radius * 2,
                               radius * 2);
        painter.setClipPath(painterPath);
        painter.drawPixmap((this->width() - m_scaledPixmap.width()) / 2, (this->height() - m_scaledPixmap.height()) / 2,
                           m_scaledPixmap);
    }
    if (!m_hoverScaledPixmap.isNull() && m_isHover)
    {
        QPainterPath painterPath;
        painterPath.addEllipse((this->width() - (radius * 2)) / 2, (this->height() - (radius * 2)) / 2, radius * 2,
                               radius * 2);
        painter.setClipPath(painterPath);
        painter.drawPixmap((this->width() - m_hoverScaledPixmap.width()) / 2,
                           (this->height() - m_hoverScaledPixmap.height()) / 2, m_hoverScaledPixmap);
    }
    QWidget::paintEvent(event);
}

void UserAvatarWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_isHover)
    {
        return QWidget::mousePressEvent(event);
    }
    emit pressed();
    QWidget::mousePressEvent(event);
}

void UserAvatarWidget::enterEvent(QEvent *event)
{
    m_isEnter = true;
}

void UserAvatarWidget::leaveEvent(QEvent *event)
{
    m_isEnter = false;
    m_isHover = false;
}

// 计算平方的函数;
double square(const double num)
{
    return num * num;
}

//
// 计算屏幕上面两个点之间的直线距离的函数，需要与计算平方函数同时使用;
double TwoPtDistance(const QPointF &pt1, const QPointF &pt2)
{
    return sqrt(double(square(pt2.x() - pt1.x()) + square(pt2.y() - pt1.y())));
}

void UserAvatarWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isEnter)
    {
        QPoint center(this->width() / 2, this->height() / 2);
        double radius = (this->width() < this->height() ? this->width() : this->height()) / 2;
        QPoint pos = event->pos();
        int distance = qSqrt(qPow(pos.x() - center.x(), 2) + qPow(pos.y() - center.y(), 2));
        if ((distance <= radius) && !m_isHover)
        {
            m_isHover = true;
            repaint();
        }
        else if ((distance > radius) && m_isHover)
        {
            m_isHover = false;
            repaint();
        }
    }
}

void UserAvatarWidget::resizeEvent(QResizeEvent *event)
{
    if ((!m_pixmap.isNull()) && (!m_scaledPixmap.isNull()) && (m_scaledPixmap.size() != this->size()))
    {
        m_scaledPixmap = scalePixmapAdjustSize(m_pixmap);
    }
    if ((!m_hoverPixmap.isNull()) && (m_hoverScaledPixmap.size() != this->size()))
    {
        m_hoverScaledPixmap = scalePixmapAdjustSize(m_hoverPixmap);
    }
    QWidget::resizeEvent(event);
}

QPixmap UserAvatarWidget::scalePixmapAdjustSize(const QPixmap &pixmap)
{
    double radius = (this->width() < this->height() ? this->width() : this->height()) / 2;
    //NOTE:拉升保持长宽比，尽可能放大，不留白
    QPixmap temp = pixmap.scaled(radius * 2, radius * 2, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    return temp;
}

void UserAvatarWidget::setDefaultImage()
{
    setImage(DEFAULT_USER_AVATAR);
}

void UserAvatarWidget::setClickEnable(bool enable)
{
    m_clickedEnable = enable;
}

bool UserAvatarWidget::setHoverImage(const QString &path)
{
    QFile file(path);
    if (!file.exists())
    {
        KLOG_WARNING() << "UserAvatar: hover pixmap file path[" << path << "] is no't exist";
        return false;
    }
    if (m_hoverPixmap.load(path))
    {
        m_hoverScaledPixmap = scalePixmapAdjustSize(m_hoverPixmap);
    }
    else
    {
        KLOG_WARNING() << "UserAvatar: hover pixmap file path[" << path << "] load failed.";
        return false;
    }
    update();
    return true;
}

QString UserAvatarWidget::iconPath()
{
    return m_iconPath;
}
