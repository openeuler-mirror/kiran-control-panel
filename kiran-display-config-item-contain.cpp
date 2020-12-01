#include "kiran-display-config-item-contain.h"
#include "kiran-display-config-item.h"
#include "kiran-display-config-global.h"
#include <QButtonGroup>
#include <QPainter>
#include <QDebug>

const int  cAbsorbOffset = 5;
KiranDisplayConfigItemContain::KiranDisplayConfigItemContain(QWidget *parent) : QWidget(parent), m_isDrag(false), m_curCheckedItem(NULL)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMinimumSize(100, 50);
    m_btnGroup = new QButtonGroup(this);
    connect(m_btnGroup, SIGNAL(buttonToggled(QAbstractButton*,bool)), this, SLOT(onItemClicked(QAbstractButton*,bool)));
}

QVariantMap KiranDisplayConfigItemContain::getData()
{
    if(Q_UNLIKELY(!m_btnGroup)) return QVariantMap();

    QVariantMap ret;
    QList<QAbstractButton*> btns = m_btnGroup->buttons();
    foreach (QAbstractButton* btn, btns) {
        KiranDisplayConfigItem *item = static_cast<KiranDisplayConfigItem *>(btn);
        QVariantMap map;
        map.insert("x", item->screenGeometryF().x());
        map.insert("y", item->screenGeometryF().y());
        map.insert("rotation", item->rotateDrect());
        map.insert("name", item->text());
        ret.insert(item->monitorPath(), map);
    }

    return ret;
}

void KiranDisplayConfigItemContain::setData(const QVariantList &var_btns, const bool &clearChecked)
{
    if(Q_UNLIKELY(!m_btnGroup)) return;

    int curClickedIndex = -1;
    if(!clearChecked)
    {
        curClickedIndex = m_btnGroup->checkedId();
    }
    clear();
    //
    int count = var_btns.count();
    KiranDisplayConfigItem *checkedBtn = NULL;
    for(int i=0; i<count; ++i) {
        QVariantMap var = var_btns.at(i).toMap();
        KiranDisplayConfigItem *btn = new KiranDisplayConfigItem(this);
        btn->setText(var.value("text").toString());
        btn->setEnabled(var.value("enabled").toBool());
        btn->setToolTip(var.value("text").toString());
        btn->setMonitorPath(var.value("monitorPath", KIRAN_SCREEN_COPY_MODE_MONITOR_PATH).toString());
        btn->setScreenGeometryF(QRectF(var.value("x").toFloat(), var.value("y").toFloat(), var.value("w").toFloat(), var.value("h").toFloat()));
        btn->initRotateDrect((KiranDisplayConfigItem::RotateDrect)var.value("rotation", KiranDisplayConfigItem::ROTATION_0).toInt());
        m_btnGroup->addButton(btn, i);
        btn->show();
        connect(btn, &KiranDisplayConfigItem::sigDrag, this, &KiranDisplayConfigItemContain::onBtnDraging);
        connect(btn, &KiranDisplayConfigItem::sigEndDrag, this, &KiranDisplayConfigItemContain::onBtnEndDrag);

        if(checkedBtn)
        {
            if(checkedBtn->screenGeometryF().left() > btn->screenGeometryF().left()) checkedBtn = btn;//选出最左边的一个作为默认选中项。
        }
        else
        {
            checkedBtn = btn;
        }
    }

    gatherItemsFixPos(m_btnGroup->buttons());
    updatePri();
    //
    QList<QAbstractButton*> btns = m_btnGroup->buttons();
    if(btns.count() > curClickedIndex &&  curClickedIndex >= 0)
        m_btnGroup->button(curClickedIndex)->setChecked(true);
    else
    {
        if(!checkedBtn) return;
        checkedBtn->setChecked(true);
    }
}

void KiranDisplayConfigItemContain::setRotateDrect(const KiranDisplayConfigItem::RotateDrect &rotateDrect)
{
    if(Q_UNLIKELY(!m_btnGroup)) return;

    KiranDisplayConfigItem *item = static_cast<KiranDisplayConfigItem *>(m_btnGroup->checkedButton());
    item->alterRotateDrect(rotateDrect);
}

void KiranDisplayConfigItemContain::changeItemEnabled(const bool &enbled)
{
    if(Q_UNLIKELY(!m_btnGroup)) return;

    KiranDisplayConfigItem *item = static_cast<KiranDisplayConfigItem *>(m_btnGroup->checkedButton());
    item->changeEnabled(enbled);
    emit sigItemEnableChanged(item->enabled());
}

QString KiranDisplayConfigItemContain::getCurMonitorText()
{
    if(Q_UNLIKELY(!m_curCheckedItem)) return QString();
    return m_curCheckedItem->text();
}

void KiranDisplayConfigItemContain::updatePri()
{
    QButtonGroup *btnGroup = m_btnGroup;
    if(Q_UNLIKELY(!btnGroup)) return;
    QList<QAbstractButton*> btns = btnGroup->buttons();
    int count = btns.count();
    if(count <= 0) return;
    //用于计算各个屏幕占比
    QRectF sumScreenRectF;
    foreach (QAbstractButton* btn, btns) {
        KiranDisplayConfigItem *item = static_cast<KiranDisplayConfigItem *>(btn);
        QRectF &rectF = item->screenGeometryF2();
        if(rectF.width() == 0 || rectF.height() == 0) return;
        sumScreenRectF = sumScreenRectF.united(rectF);
    }
    foreach (QAbstractButton* btn, btns) {
        KiranDisplayConfigItem *item = static_cast<KiranDisplayConfigItem *>(btn);
        item->moveScreenGeometryFOffset(- sumScreenRectF.topLeft());
    }

    float sumScreenSizeWidth = sumScreenRectF.width();
    float sumScreenHeight = sumScreenRectF.height();

    if(Q_UNLIKELY(sumScreenSizeWidth == 0 || sumScreenHeight == 0)) return;

    //父窗口总大小
    float width = (float)this->width();
    float height = (float)this->height();
    if(Q_UNLIKELY(width == 0 || height == 0)) return;
    //
    float curZoom = 0;
    float offset = 0;
    bool smallHeight = width/height >= sumScreenSizeWidth/sumScreenHeight;
    if(smallHeight)
    {
        curZoom = height / sumScreenHeight;
        //水平居中
        offset = qAbs(width - curZoom*sumScreenSizeWidth)/2;
    }
    else
    {
        curZoom = width / sumScreenSizeWidth;
        offset = qAbs(height - curZoom*sumScreenHeight)/2;
    }

    for(int i=0; i<count; ++i)
    {
        KiranDisplayConfigItem *item = static_cast<KiranDisplayConfigItem *>(btns.at(i));
        const QPointF screenPos = item->screenGeometryF().topLeft();
        float new_x = curZoom * screenPos.x();
        float new_y = curZoom * screenPos.y();

        const QSizeF &screenSize = item->screenGeometryF().size();
        float new_h = curZoom * screenSize.height();
        float new_w  = curZoom * screenSize.width();

        auto *btn = m_btnGroup->button(i);
        if(!btn) continue;

        smallHeight ? btn->setGeometry(new_x+offset, new_y, new_w, new_h)
                    : btn->setGeometry(new_x, new_y+offset, new_w, new_h);
    }
}

void KiranDisplayConfigItemContain::clear()
{
    m_isDrag = false;
    m_curCheckedItem = NULL;
    m_anchorPos.clear();

    if(Q_LIKELY(m_btnGroup))
    {
        QList<QAbstractButton*> btns = m_btnGroup->buttons();
        foreach (QAbstractButton *btn, btns) {
            m_btnGroup->removeButton(btn);
            btn->deleteLater();
        }
    }
}

QString KiranDisplayConfigItemContain::getCurMonitorPath() const
{
    if(Q_UNLIKELY(!m_curCheckedItem)) return QString();

    return m_curCheckedItem->monitorPath();
}

void KiranDisplayConfigItemContain::onBtnDraging(QAbstractButton *b)
{
    if(Q_UNLIKELY(!m_btnGroup)) return;
    m_anchorPos = getMinDisGeometry(b, m_btnGroup->buttons());
    m_isDrag = true;
    update();
}

void KiranDisplayConfigItemContain::onBtnEndDrag(QAbstractButton *btn)
{
    if(Q_UNLIKELY(!m_btnGroup) || Q_UNLIKELY(!btn)) return;
    //
    KiranDisplayConfigItem *item = static_cast<KiranDisplayConfigItem *>(btn);
    item->setZoomPair(m_anchorPos.zoomPair);
    //QList<KiranDisplayConfigItem *> childs = item->childAnchorBtns();
    item->clearAnchoredChildBtns();
    item->setAnchorByBtn(m_anchorPos.anchorByBtn, m_anchorPos.drect);
    //
    insertItem(btn, m_anchorPos, m_btnGroup->buttons());
    //
    QList<QAbstractButton *> childs;
    mainCluster(item, m_btnGroup->buttons(), childs);
    gatherItems(childs);

    updatePri();

    m_isDrag = false;
    update();
}

void KiranDisplayConfigItemContain::onItemClicked(QAbstractButton *btn, bool isChecked)
{
    if(Q_UNLIKELY(!btn)) return;

    if(isChecked)
    {
        m_curCheckedItem = static_cast<KiranDisplayConfigItem *>(btn);
        emit sigButtonChecked(m_curCheckedItem->monitorPath());
        emit sigItemEnableChanged(m_curCheckedItem->enabled());
    }
}

void KiranDisplayConfigItemContain::resizeEvent(QResizeEvent *event)
{
    updatePri();
    QWidget::resizeEvent(event);
}

void KiranDisplayConfigItemContain::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    //    if(m_isDrag)
    //    {
    //        QPainter painter(this);
    //        QPen pen = painter.pen();
    //        if(!m_anchorPos.dashesLine.isNull())
    //        {
    //            painter.save();
    //            pen.setColor("#ffffff");
    //            painter.setPen(pen);
    //            QVector<qreal> dashes;
    //            qreal space = 3;
    //            dashes << 5 << space << 5 <<space;
    //            QPen pen = painter.pen();
    //            pen.setDashPattern(dashes);
    //            painter.setPen(pen);
    //            painter.drawLine(m_anchorPos.dashesLine);
    //            painter.restore();
    //        }

    //        pen.setWidth(2);
    //        pen.setColor("#2eb3ff");
    //        painter.setPen(pen);
    //        painter.drawLine(m_anchorPos.line);
    //    }
}
/*!
 * \brief KiranDisplayConfigItemContain::getAvailableGeometry 获取指定方向上的Anchor Rect
 * \param g1
 * \param g2
 * \param drect
 * \param magnet
 * \return
 */
KiranDisplayConfigItemContain::AnchorRectPos KiranDisplayConfigItemContain::getAvailableGeometry(const QRect &g1, const QRect &g2, const KiranDisplayConfigItem::AnchorByDrect &drect, const bool &magnet)
{
    QRect r = g1;
    int d = 999999;
    QLine line;
    QLine dashesLine;
    QPair<int , int> zoomPair;
    switch (drect) {
    case KiranDisplayConfigItem::PosLeft:
        if(g1.bottom()<=g2.top() || g1.top()>=g2.bottom() || g1.left()>=g2.center().x()) return KiranDisplayConfigItemContain::AnchorRectPos();
        r.moveRight(g2.left());

        if(magnet)
        {
            if(r.top()<=(g2.top()+cAbsorbOffset) && r.top()>=(g2.top()-cAbsorbOffset)) r.moveTop(g2.top());
            if(r.top()<=(g2.bottom()+cAbsorbOffset) && r.top()>=(g2.bottom()-cAbsorbOffset)) r.moveTop(g2.bottom());

            if(r.bottom()<=(g2.top()+cAbsorbOffset) && r.bottom()>=(g2.top()-cAbsorbOffset)) r.moveBottom(g2.top());
            if(r.bottom()<=(g2.bottom()+cAbsorbOffset) && r.bottom()>=(g2.bottom()-cAbsorbOffset)) r.moveBottom(g2.bottom());
        }

        d = qAbs(r.center().x() - g1.center().x());
        line.setPoints(r.topRight(), r.bottomRight());
        dashesLine.setLine(line.x1(), -999999, line.x1(), 999999);
        zoomPair.first = r.top() - g2.top();
        zoomPair.second = g2.height();
        break;
    case KiranDisplayConfigItem::PosTop:
        if(g1.right()<=g2.left() || g1.left()>=g2.right() || g1.top()>=g2.center().y()) return KiranDisplayConfigItemContain::AnchorRectPos();
        r.moveBottom(g2.top());

        if(magnet)
        {
            if(r.left()>=(g2.left()-cAbsorbOffset) && r.left()<=(g2.left()+cAbsorbOffset)) r.moveLeft(g2.left());
            if(r.left()>=(g2.right()-cAbsorbOffset) && r.left()<=(g2.right()+cAbsorbOffset)) r.moveLeft(g2.right());

            if(r.right()>=(g2.left()-cAbsorbOffset) && r.right()<=(g2.left()+cAbsorbOffset)) r.moveRight(g2.left());
            if(r.right()>=(g2.right()-cAbsorbOffset) && r.right()<=(g2.right()+cAbsorbOffset)) r.moveRight(g2.right());
        }

        d = qAbs(r.center().y() - g1.center().y());
        line.setPoints(r.bottomLeft(), r.bottomRight());
        dashesLine.setLine(-999999, line.y1(), 999999, line.y1());
        zoomPair.first = r.left() - g2.left();
        zoomPair.second = g2.width();
        break;
    case KiranDisplayConfigItem::PosRight:
        if(g1.bottom()<=g2.top() || g1.top()>=g2.bottom() || g1.right()<=g2.center().x()) return KiranDisplayConfigItemContain::AnchorRectPos();;
        r.moveLeft(g2.right());

        if(magnet)
        {
            if(r.top()<=(g2.top()+cAbsorbOffset) && r.top()>=(g2.top()-cAbsorbOffset)) r.moveTop(g2.top());
            if(r.top()<=(g2.bottom()+cAbsorbOffset) && r.top()>=(g2.bottom()-cAbsorbOffset)) r.moveTop(g2.bottom());

            if(r.bottom()<=(g2.top()+cAbsorbOffset) && r.bottom()>=(g2.top()-cAbsorbOffset)) r.moveBottom(g2.top());
            if(r.bottom()<=(g2.bottom()+cAbsorbOffset) && r.bottom()>=(g2.bottom()-cAbsorbOffset)) r.moveBottom(g2.bottom());
        }

        d = qAbs(r.center().x() - g1.center().x());
        line.setPoints(r.topLeft(), r.bottomLeft());
        dashesLine.setLine(line.x1(), -999999, line.x1(), 999999);
        zoomPair.first = r.top() - g2.top();
        zoomPair.second = g2.height();
        break;
    case KiranDisplayConfigItem::PosBottom:
        if(g1.right()<=g2.left() || g1.left()>=g2.right() || g1.bottom()<=g2.center().y()) return KiranDisplayConfigItemContain::AnchorRectPos();
        r.moveTop(g2.bottom());

        if(magnet)
        {
            if(r.left()>=(g2.left()-cAbsorbOffset) && r.left()<=(g2.left()+cAbsorbOffset)) r.moveLeft(g2.left());
            if(r.left()>=(g2.right()-cAbsorbOffset) && r.left()<=(g2.right()+cAbsorbOffset)) r.moveLeft(g2.right());

            if(r.right()>=(g2.left()-cAbsorbOffset) && r.right()<=(g2.left()+cAbsorbOffset)) r.moveRight(g2.left());
            if(r.right()>=(g2.right()-cAbsorbOffset) && r.right()<=(g2.right()+cAbsorbOffset)) r.moveRight(g2.right());
        }

        d = qAbs(r.center().y() - g1.center().y());
        line.setPoints(r.topLeft(), r.topRight());
        dashesLine.setLine(-999999, line.y1(), 999999, line.y1());
        zoomPair.first = r.left() - g2.left();
        zoomPair.second = g2.width();
        break;
    case KiranDisplayConfigItem::PosTopLeft:
    {
        if(!(g1.bottom()<=g2.top() && g1.right()<=g2.left()))return KiranDisplayConfigItemContain::AnchorRectPos();

        r.moveBottomRight(g2.topLeft());
        QLineF lineF(r.center(), g1.center());
        d = lineF.length();
        line.setPoints(g1.bottomRight(), g2.topLeft());
    }
        break;
    case KiranDisplayConfigItem::PosTopRight:
    {
        if(!(g1.left()>=g2.right() && g1.bottom()<=g2.top())) return KiranDisplayConfigItemContain::AnchorRectPos();
        r.moveBottomLeft(g2.topRight());
        QLineF lineF(r.center(), g1.center());
        d = lineF.length();
        line.setPoints(g1.bottomLeft(), g2.topRight());
    }
        break;
    case KiranDisplayConfigItem::PosBottomLeft:
    {
        if(!(g1.top()>=g2.bottom() && g1.right()<=g2.left())) return KiranDisplayConfigItemContain::AnchorRectPos();
        r.moveTopRight(g2.bottomLeft());
        QLineF lineF(r.center(), g1.center());
        d = lineF.length();
        line.setPoints(g1.topRight(), g2.bottomLeft());
    }
        break;
    case KiranDisplayConfigItem::PosBottomRight:
    {
        if(!(g1.top()>=g2.bottom() && g1.left()>=g2.right())) return KiranDisplayConfigItemContain::AnchorRectPos();
        r.moveTopLeft(g2.bottomRight());
        QLineF lineF(r.center(), g1.center());
        d = lineF.length();
        line.setPoints(g1.topLeft(), g2.bottomRight());
    }
    default:
        break;
    }

    KiranDisplayConfigItemContain::AnchorRectPos ret;
    ret.r = r;
    ret.d = d;
    ret.line = line;
    ret.dashesLine = dashesLine;
    ret.drect = drect;
    ret.zoomPair = zoomPair;
    return ret;
}
/*!
 * \brief KiranDisplayConfigItemContain::getAvailableGeometrys 获取所有方向的Anchor Rect
 * \param g1
 * \param g2
 * \param magnet
 * \return
 */
QList<KiranDisplayConfigItemContain::AnchorRectPos> KiranDisplayConfigItemContain::getAvailableGeometrys(const QRect &g1, const QRect &g2, const bool &magnet)
{
    QList<KiranDisplayConfigItemContain::AnchorRectPos>  ret;
    for(int i=0; i<8; ++i)
    {
        KiranDisplayConfigItemContain::AnchorRectPos st = getAvailableGeometry(g1, g2, (KiranDisplayConfigItem::AnchorByDrect)i, magnet);
        if(st.r.isEmpty()) continue;

        ret << st;
    }
    return ret;
}

KiranDisplayConfigItemContain::AnchorRectPos KiranDisplayConfigItemContain::getMinDisScreenGeometry(KiranDisplayConfigItem *b, QList<QAbstractButton *> btns, const bool &magnet)
{
    QList<KiranDisplayConfigItemContain::AnchorRectPos> list;
    QRectF g = b->screenGeometryF();
    foreach (QAbstractButton *btn, btns) {
        if(btn == b) continue;
        KiranDisplayConfigItem *item = static_cast<KiranDisplayConfigItem *>(btn);
        QRectF geometry = item->screenGeometryF();
        QList<KiranDisplayConfigItemContain::AnchorRectPos> anchorList = getAvailableGeometrys(g.toRect(), geometry.toRect(), magnet);
        for(int i=0; i<anchorList.count(); ++i)
        {
            anchorList[i].anchorByBtn = static_cast<KiranDisplayConfigItem *>(btn);
        }
        list << anchorList;
    }

    return getMinDisGeometryPrivate(list);
}
KiranDisplayConfigItemContain::AnchorRectPos KiranDisplayConfigItemContain::getMinDisGeometry(QAbstractButton *b, QList<QAbstractButton *> btns, const bool &magnet)
{
    QList<KiranDisplayConfigItemContain::AnchorRectPos> list;
    QRect g = b->geometry();
    foreach (QAbstractButton *btn, btns) {//must use btns
        if(btn == b) continue;
        QRect geometry = btn->geometry();
        QList<KiranDisplayConfigItemContain::AnchorRectPos> anchorList = getAvailableGeometrys(g, geometry, magnet);
        for(int i=0; i<anchorList.count(); ++i)
        {
            anchorList[i].anchorByBtn = static_cast<KiranDisplayConfigItem *>(btn);
        }
        list << anchorList;
    }

    return getMinDisGeometryPrivate(list);
}

KiranDisplayConfigItemContain::AnchorRectPos KiranDisplayConfigItemContain::getMinDisGeometryPrivate(const QList<KiranDisplayConfigItemContain::AnchorRectPos> &list)
{
    KiranDisplayConfigItemContain::AnchorRectPos ret;
    ret.d = 999999;
    foreach (KiranDisplayConfigItemContain::AnchorRectPos l, list) {
        if(l.d < ret.d)
            ret = l;
    }

    return ret;
}

void KiranDisplayConfigItemContain::insertItem(QAbstractButton *insertBtn, const KiranDisplayConfigItemContain::AnchorRectPos &anchorPos, const QList<QAbstractButton *> &btns)
{
    if(Q_UNLIKELY(!insertBtn)) return;
    KiranDisplayConfigItem *item = static_cast<KiranDisplayConfigItem *>(insertBtn);
    QRectF itemScreenRect = item->screenGeometryF();

    foreach (QAbstractButton *btn, btns) {
        if(btn == item) continue;
        KiranDisplayConfigItem *otherItem = static_cast<KiranDisplayConfigItem *>(btn);
        QRectF otherScreenGeometry = otherItem->screenGeometryF();

        QRectF intersec = itemScreenRect.intersected(otherScreenGeometry);
        if(intersec.isValid() && !intersec.isEmpty())
        {
            KiranDisplayConfigItem::AnchorByDrect spDrect = insertDrect(itemScreenRect, otherScreenGeometry, anchorPos.drect);
            otherItem->setAnchorByBtn(static_cast<KiranDisplayConfigItem *>(insertBtn), spDrect);
            //
            intersec = itemScreenRect.intersected(otherItem->screenGeometryF());
            if(intersec.isValid() && !intersec.isEmpty())
            {
                insertItem(insertBtn, anchorPos, btns);
            }
            else
            {
                AnchorRectPos anchPos;
                anchPos.drect = anchorPos.drect;

                QList<QAbstractButton *> btnList = btns;
                btnList.removeOne(insertBtn);
                btnList.removeOne(btn);
                insertItem(btn, anchPos, btnList);
            }
        }
    }
}

void KiranDisplayConfigItemContain::gatherItemsFixPos(const QList<QAbstractButton *> &items)
{
    foreach (QAbstractButton *btn, items) {
        KiranDisplayConfigItem *item = static_cast<KiranDisplayConfigItem *>(btn);

        KiranDisplayConfigItemContain::AnchorRectPos anchorPos = getMinDisScreenGeometry(item, items, false);

        if(!anchorPos.anchorByBtn) continue;
        if(anchorPos.anchorByBtn->anchorByBtn() == item) continue;

        item->setAnchorByBtn(anchorPos.anchorByBtn, anchorPos.drect);
    }
}

KiranDisplayConfigItem::AnchorByDrect KiranDisplayConfigItemContain::insertDrect(const QRectF &r, const QRectF &movedR, const KiranDisplayConfigItem::AnchorByDrect &drect)
{
    KiranDisplayConfigItem::AnchorByDrect ret = drect;
    switch (drect) {
    case KiranDisplayConfigItem::PosLeft:
        if(movedR.right() > r.right())
        {
            ret = KiranDisplayConfigItem::PosRight;
        }
        break;
    case KiranDisplayConfigItem::PosRight:
        if(movedR.left() < r.left())
        {
            ret = KiranDisplayConfigItem::PosLeft;
        }
        break;
    case KiranDisplayConfigItem::PosTop:
        if(movedR.bottom() > r.bottom())
        {
            ret = KiranDisplayConfigItem::PosBottom;
        }
        break;
    case KiranDisplayConfigItem::PosBottom:
        if(movedR.top() < r.top())
        {
            ret = KiranDisplayConfigItem::PosTop;
        }
        break;
    default:
        break;
    }

    return ret;
}

void KiranDisplayConfigItemContain::gatherItems(QList<QAbstractButton *> &items)
{
    QList<QAbstractButton *> t_items = items;
    QList<QAbstractButton *> btns = m_btnGroup->buttons();
    foreach (QAbstractButton *btn, btns) {
        if(items.contains(btn)) continue;
        KiranDisplayConfigItem *item = static_cast<KiranDisplayConfigItem *>(btn);

        KiranDisplayConfigItemContain::AnchorRectPos anchorPos = getMinDisScreenGeometry(item, t_items, false);
        item->setAnchorByBtn(anchorPos.anchorByBtn, anchorPos.drect);
        t_items << item;
    }
}

void KiranDisplayConfigItemContain::mainCluster(KiranDisplayConfigItem *dragBtn, const QList<QAbstractButton *> &list, QList<QAbstractButton *> &ret)
{
    if(Q_UNLIKELY(!dragBtn)) return ;

    QRectF screenGeo = dragBtn->screenGeometryF();
    screenGeo.adjust(-1, -1, 1, 1);
    foreach (QAbstractButton *btn, list) {
        KiranDisplayConfigItem *b = static_cast<KiranDisplayConfigItem *>(btn);
        if(screenGeo.intersects(b->screenGeometryF()) && !ret.contains(b))
        {
            ret << b;
            QList<QAbstractButton *> t_list = list;
            t_list.removeAll(b);
            mainCluster(b, t_list, ret);
        }
    }
}

