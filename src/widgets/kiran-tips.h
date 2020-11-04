#ifndef KIRANEDITTIP_H
#define KIRANEDITTIP_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>

namespace Ui {
class KiranTips;
}

class QGraphicsDropShadowEffect;
class QGraphicsOpacityEffect;
class KiranTips : private QWidget
{
    Q_OBJECT

public:
    enum ShowPostionHint{
        POSITION_LEFT,
        POSITION_RIGHT,
        POSITION_TOP,
        POSITION_BOTTM
    };
    Q_ENUM(ShowPostionHint)

public:
    explicit KiranTips(QWidget *parent);
    ~KiranTips();

    /**
     * @brief 设置显示的位置
     * @param 显示的位置
     */
    void setShowPosition(ShowPostionHint positionHint);

    /**
     * @brief 设置文本
     * @param 悬浮提示框文本
     */
    void setText(const QString& text);

    /**
     * @brief 设置定时退出时间
     * @param ms 毫秒，如果ms=0,则禁用定时退出功能
     */
    void setHideTimeout(int ms);

    /**
     * @brief 设置是否启用单击时隐藏tips
     * @param enable 是否启用
     */
    void setHideOnClickedEnable(bool enable);

    /**
     * @brief 设置是否启用动画
     * @param enable 是否启用动画
     */
    void setAnimationEnable(bool enable);

    /**
     * @brief 悬浮提示背景色
     * @param color 颜色
     */
    void setBackgroundColor(QColor color);

    /**
     * @brief 显示悬浮提示框在Widget周围
     * @param widget 悬浮提示框显示在该控件周围
     */
    void showTipAroundWidget(QWidget* widget);

    /**
     * @brief 隐藏悬浮提示框
     */
    void hideTip();

private:
    void drawUpArrowBackground(QPainter* painter);
    void drawDownArrowBackground(QPainter* painter);
    void drawLeftArrowBackground(QPainter* painter);
    void drawRightArrowBackground(QPainter* painter);
    QSize getRightSize() const;

protected:
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::KiranTips *ui;
    ShowPostionHint m_showPostion;
    QPropertyAnimation* m_EnterAnimation;
    QTimer m_fadeOutTimer;
    bool m_hideOnClicked;
    bool m_animationEnable;
    QColor m_bgColor;
    QWidget* m_showAroudWidget;
};

#endif // KIRANEDITTIP_H
