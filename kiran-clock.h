#ifndef KIRANCLOCK_H
#define KIRANCLOCK_H

#include <QWidget>
#include <QPixmap>
#include <QSvgRenderer>

class QGraphicsDropShadowEffect;

/**
 * @brief 时钟控件
 */
class KiranClock : public QWidget
{
    Q_OBJECT
public:
    explicit KiranClock(QWidget *parent = nullptr);
    virtual ~KiranClock();

    /// 设置更新时间ms
    void setUpdateTimeout(int ms);
protected:
    void loadPixmap();
    void initShadow();
    void updateDrawRect();

    void paintDial(QPainter& painter);
    void paintHour(QPainter& painter);
    void paintMinute(QPainter& painter);
    void paintSecond(QPainter& painter);

    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
signals:
private:
    QGraphicsDropShadowEffect* m_effect;
    QRect m_drawRect;
    int m_updateTimerID;

    QSvgRenderer m_dialReader;
    QSvgRenderer m_hourReader;
    QSvgRenderer m_minuteReader;
    QSvgRenderer m_secondReader;
};

#endif // KIRANCLOCK_H
