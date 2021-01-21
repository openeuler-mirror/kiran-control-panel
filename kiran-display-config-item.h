#ifndef KIRANDISPLAYCONFIGITEM_H
#define KIRANDISPLAYCONFIGITEM_H

#include <QPushButton>
#include <QEvent>

class KiranDisplayConfigItem : public QPushButton
{
    Q_OBJECT
public:
    explicit KiranDisplayConfigItem(QWidget *parent = nullptr);

    // 显示器的旋转角度
    enum DisplayRotationType
    {
        // 不旋转
        DISPLAY_ROTATION_0 = (1 << 0),
        // 90度旋转
        DISPLAY_ROTATION_90 = (1 << 1),
        // 180度旋转
        DISPLAY_ROTATION_180 = (1 << 2),
        // 270度旋转
        DISPLAY_ROTATION_270 = (1 << 3),
    };

    // 显示器的翻转方向
    enum DisplayReflectType
    {
        // 正常
        DISPLAY_REFLECT_NORMAL = 0,
        // X方向翻转
        DISPLAY_REFLECT_X = (1 << 4),
        // Y方向翻转
        DISPLAY_REFLECT_Y = (1 << 5),
        // XY方向翻转
        DISPLAY_REFLECT_XY = (1 << 4) + (1 << 5)
    };
    Q_DECLARE_FLAGS(DisplayReflectTypes, DisplayReflectType)

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

    KiranDisplayConfigItem::DisplayReflectTypes displayReflectType() const;
    void setDisplayReflectType(const KiranDisplayConfigItem::DisplayReflectTypes &displayReflectType);

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

Q_DECLARE_OPERATORS_FOR_FLAGS(KiranDisplayConfigItem::DisplayReflectTypes)
#endif // KIRANDISPLAYCONFIGITEM_H
