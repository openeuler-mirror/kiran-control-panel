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
 * Author:     yangxiaoqing <yangxiaoqing@kylinsec.com.cn>
 */

#pragma once

#include <QWidget>
#include "device-panel-item.h"
#include "generic-types.h"

class QAbstractButton;
class QButtonGroup;
class DisplayConfig;
class DevicePanelWidget : public QWidget
{
    Q_OBJECT
public:
    typedef struct StuAnchorRectPos
    {
        StuAnchorRectPos() { clear(); }
        QRect r;
        int d;
        QLine line;
        QLine dashesLine;
        QPair<int, int> zoomPair;
        DevicePanelItem::AnchorByDrect drect;
        DevicePanelItem *anchorByBtn;
        void clear()
        {
            r = QRect();
            d = 0;
            zoomPair = QPair<int, int>(0, 0);
            drect = DevicePanelItem::PosLeft;
            anchorByBtn = NULL;
        }

    } AnchorRectPos;

    explicit DevicePanelWidget(QWidget *parent = nullptr);

    void setRotateDrect(const int &step = 1);
    bool getHorizontalDisplayReflectType();
    bool getVerticalDisplayReflectType();
    void setHorizontalDisplayReflectType(bool checked);
    void setVerticalDisplayReflectType(bool checked);
    void changeItemEnabled(const bool &enbled);

    QString getCurMonitorText();
    QString getCurMonitorPath() const;

Q_SIGNALS:
    void sigButtonChecked(QString monitorPath);
    /*!
     * \brief sigEnableChanged 状态更新信号与点击信号分开，两者需要执行的内容流程不能完全一致。
     * \param enabled
     */
    void sigItemEnableChanged(const bool &enabled);

protected slots:
    void onBtnDraging(QAbstractButton *b);
    void onBtnEndDrag(QAbstractButton *btn);
    void onItemClicked(QAbstractButton *btn, bool isChecked);
    void handleConfigModeChanged(ConfigMode mode);
    void updateScreenGeometry();

private:
    void resizeEvent(QResizeEvent *event) override;

    DevicePanelWidget::AnchorRectPos getMinDisScreenGeometry(DevicePanelItem *b, QList<QAbstractButton *> btns, const bool &magnet = true);
    DevicePanelWidget::AnchorRectPos getMinDisGeometry(QAbstractButton *b, QList<QAbstractButton *> btns, const bool &magnet = true);
    QList<DevicePanelWidget::AnchorRectPos> getAvailableGeometrys(const QRect &g1, const QRect &g2, const bool &magnet);
    DevicePanelWidget::AnchorRectPos getAvailableGeometry(const QRect &g1, const QRect &g2, const DevicePanelItem::AnchorByDrect &drect, const bool &magnet);
    DevicePanelWidget::AnchorRectPos getMinDisGeometryPrivate(const QList<DevicePanelWidget::AnchorRectPos> &list);

    DevicePanelItem::AnchorByDrect insertDrect(const QRectF &r, const QRectF &movedR, const DevicePanelItem::AnchorByDrect &drect);
    void insertItem(QAbstractButton *insertBtn, const DevicePanelWidget::AnchorRectPos &anchorPos, const QList<QAbstractButton *> &btns);
    void gatherItemsFixPos(const QList<QAbstractButton *> &items);
    void gatherItems(QList<QAbstractButton *> &items);
    void mainCluster(DevicePanelItem *dragBtn, const QList<QAbstractButton *> &list, QList<QAbstractButton *> &ret);

    void updatePri();
    void clear();

private:
    bool m_isDrag;
    QButtonGroup *m_btnGroup;
    DevicePanelItem *m_curCheckedItem;
    DevicePanelWidget::AnchorRectPos m_anchorPos;
    DisplayConfig *m_displayConfig;
};
