/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-display is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 */

#ifndef KIRANDISPLAYCONFIGITEM_H
#define KIRANDISPLAYCONFIGITEM_H

#include "kiran-session-daemon/display-i.h"
#include <QPushButton>
#include <QEvent>

Q_DECLARE_FLAGS(DisplayReflectTypes, DisplayReflectType)
Q_DECLARE_OPERATORS_FOR_FLAGS(DisplayReflectTypes)

class KiranDisplayConfigItem : public QPushButton
{
    Q_OBJECT
public:
    explicit KiranDisplayConfigItem(QWidget *parent = nullptr);

    enum AnchorByDrect{PosLeft=0,PosRight,PosTop,PosBottom,PosTopLeft,PosTopRight,PosBottomLeft,PosBottomRight};

    QRectF screenGeometryF() const;
    QRectF &screenGeometryF2();
    void setScreenGeometryF(const QRectF &screenGeometryF);

    void moveScreenGeometryFOffset(const QPointF &offsetF);

    KiranDisplayConfigItem *anchorByBtn() const;
    void setAnchorByBtn(KiranDisplayConfigItem *anchorByBtn, const AnchorByDrect &anchorByDrect);

    AnchorByDrect anchorByDrect() const;
    void setAnchorByDrect(const AnchorByDrect &anchorByDrect);

    void removeAnchoredChildBtn(KiranDisplayConfigItem *childBtn);
    void appendAnchoredChildBtn(KiranDisplayConfigItem *childBtn);
    void clearAnchorByBtn();
    void clearAnchoredChildBtns();

    bool hasIntersects(KiranDisplayConfigItem *item);

    QPair<int, int> zoomPair() const;
    void setZoomPair(const QPair<int, int> &zoomPair);

    DisplayRotationType rotateDrect() const;
    /**
     * @brief alterRotateDrect 旋转角度修改之后，需要通知item容器重新计算位置，要发送信号出去。
     * @param rotateDrect
     */
    void alterRotateDrect(const int &step=1);
    void initRotateDrect(const DisplayRotationType &rotateDrect);

    DisplayReflectTypes displayReflectType() const;
    void setDisplayReflectType(const DisplayReflectTypes &displayReflectType);

    QString monitorPath() const;
    void setMonitorPath(const QString &monitorPath);

    void setEnabled(bool enabled);
    void changeEnabled(const bool &enabled);
    bool enabled() const;

signals:
    void sigDrag(QAbstractButton* btn);
    void sigEndDrag(QAbstractButton* btn);

public slots:

private:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);

    void updateScreenGeometry();
    void updateOffset(KiranDisplayConfigItem *anchorByBtn, const AnchorByDrect &anchorByDrect, const bool &isDrag);
    DisplayRotationType rotationType(const DisplayRotationType &curType, const int &step);

private:
    bool m_mousePress;
    bool m_mouseDrag;
    QPoint m_pressPos;
    QEvent::Type m_statusType;
    //desktop invented data
    KiranDisplayConfigItem *m_anchorByBtn;
    QPair<int, int> m_zoomPair;
    //reality data
    bool m_enabled;
    QString m_monitorPath;
    AnchorByDrect m_anchorByDrect;
    QPointF m_screenOffset;
    QRectF m_screenGeometryF;
    DisplayRotationType m_rotateDrect;
    DisplayReflectTypes m_displayReflectType;

    QList<KiranDisplayConfigItem *> m_childAnchorBtns;
};

#endif // KIRANDISPLAYCONFIGITEM_H
