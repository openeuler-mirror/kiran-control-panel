#ifndef KIRANDISPLAYCONFIGITEM_H
#define KIRANDISPLAYCONFIGITEM_H

#include <QPushButton>
#include <QEvent>

class KiranDisplayConfigItem : public QPushButton
{
    Q_OBJECT
public:
    explicit KiranDisplayConfigItem(QWidget *parent = nullptr);

//    enum RotationType : uint16_t
//    {
//        ROTATION_0 = (1 << 0),
//        ROTATION_90 = (1 << 1),
//        ROTATION_180 = (1 << 2),
//        ROTATION_270 = (1 << 3),
//    };
    enum RotateDrect : uint16_t
    {
        ROTATION_0 = (1 << 0),
        Left = (1 << 1),
        Inverted = (1 << 2),
        Right = (1 << 3)
    };
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

    RotateDrect rotateDrect() const;
    void alterRotateDrect(const RotateDrect &rotateDrect);
    void initRotateDrect(const RotateDrect &rotateDrect);

    QString monitorPath() const;
    void setMonitorPath(const QString &monitorPath);

    void setEnabled(bool enabled);

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
    RotateDrect m_rotateDrect;

    QList<KiranDisplayConfigItem *> m_childAnchorBtns;
};

#endif // KIRANDISPLAYCONFIGITEM_H
