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
    void setRotateDrect(const KiranDisplayConfigItem::RotateDrect &rotateDrect);

    QString getCurMonitorText();
    QString getCurMonitorPath() const;

Q_SIGNALS:
    void buttonChecked(QString monitorPath);

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
