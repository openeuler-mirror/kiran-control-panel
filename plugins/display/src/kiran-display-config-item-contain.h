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

#ifndef KIRANDISPLAYCONFIGITEMCONTAIN_H
#define KIRANDISPLAYCONFIGITEMCONTAIN_H

#include "kiran-display-config-item.h"
#include <QWidget>
#include <QMap>

class QAbstractButton;
class QButtonGroup;
class KiranDisplayConfigItemContain : public QWidget
{
    Q_OBJECT
public:
    typedef struct StuAnchorRectPos {
        StuAnchorRectPos(){clear();}
        QRect r;
        int d;
        QLine line;
        QLine dashesLine;
        QPair<int, int> zoomPair;
        KiranDisplayConfigItem::AnchorByDrect drect;
        KiranDisplayConfigItem *anchorByBtn;
        void clear()
        {
            r = QRect();
            d = 0;
            zoomPair = QPair<int, int>(0, 0);
            drect = KiranDisplayConfigItem::PosLeft;
            anchorByBtn = NULL;
        }

    }AnchorRectPos;

    explicit KiranDisplayConfigItemContain(QWidget *parent = nullptr);

    QVariantMap getData();
    void setData(const QVariantList &var_btns, const bool &clearChecked=false);
    void setRotateDrect(const int &step=1);
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

public slots:
protected slots:
    void onBtnDraging(QAbstractButton *b);
    void onBtnEndDrag(QAbstractButton *btn);
    void onItemClicked(QAbstractButton *btn, bool isChecked);

private:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    KiranDisplayConfigItemContain::AnchorRectPos getMinDisScreenGeometry(KiranDisplayConfigItem *b, QList<QAbstractButton *> btns, const bool &magnet=true);
    KiranDisplayConfigItemContain::AnchorRectPos getMinDisGeometry(QAbstractButton *b, QList<QAbstractButton *> btns, const bool &magnet=true);
    QList<KiranDisplayConfigItemContain::AnchorRectPos> getAvailableGeometrys(const QRect &g1, const QRect &g2, const bool &magnet);
    KiranDisplayConfigItemContain::AnchorRectPos getAvailableGeometry(const QRect &g1, const QRect &g2, const KiranDisplayConfigItem::AnchorByDrect &drect, const bool &magnet);
    KiranDisplayConfigItemContain::AnchorRectPos getMinDisGeometryPrivate(const QList<KiranDisplayConfigItemContain::AnchorRectPos> &list);

    KiranDisplayConfigItem::AnchorByDrect insertDrect(const QRectF &r, const QRectF &movedR, const KiranDisplayConfigItem::AnchorByDrect &drect);
    void insertItem(QAbstractButton *insertBtn, const KiranDisplayConfigItemContain::AnchorRectPos &anchorPos, const QList<QAbstractButton*> &btns);
    void gatherItemsFixPos(const QList<QAbstractButton *> &items);
    void gatherItems(QList<QAbstractButton *> &items);
    void mainCluster(KiranDisplayConfigItem *dragBtn, const QList<QAbstractButton *> &list, QList<QAbstractButton *> &ret);

    void updatePri();
    void clear();

private:
    bool m_isDrag;
    QButtonGroup *m_btnGroup;
    KiranDisplayConfigItem *m_curCheckedItem;
    KiranDisplayConfigItemContain::AnchorRectPos m_anchorPos;
};

#endif // KIRANDISPLAYCONFIGITEMCONTAIN_H
