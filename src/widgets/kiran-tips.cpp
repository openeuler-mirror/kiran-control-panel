#include "kiran-tips.h"
#include "ui_kiran-tips.h"

#include <QTimerEvent>
#include <QDebug>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QStyleOption>

///tips离显示控件的间距
#define TIPS_SPACING 3

///背景矩形圆角
#define TIPS_RECT_RND 5

///布局margin
#define MARGIN 5

///三角形边长
#define TRIANGLE_SIDE_LENGTH 8

KiranTips::KiranTips (QWidget *parent) :
        QWidget(parent),
        ui(new Ui::KiranTips),
        m_showPostion(POSITION_RIGHT),
        m_EnterAnimation(new QPropertyAnimation(this)),
        m_fadeOutTimer(this),
        m_hideOnClicked(true),
        m_animationEnable(false),
        m_bgColor(Qt::white),
        m_showAroudWidget(nullptr)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setWindowFlag(Qt::FramelessWindowHint);

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(7);
    shadowEffect->setOffset(0, 0);
    setGraphicsEffect(shadowEffect);

    this->layout()->setContentsMargins(MARGIN, MARGIN, MARGIN + TRIANGLE_SIDE_LENGTH, MARGIN);

    m_fadeOutTimer.setSingleShot(true);
    connect(&m_fadeOutTimer, &QTimer::timeout, [this] () {
        hideTip();
    });

    m_EnterAnimation->setTargetObject(this);
    m_EnterAnimation->setPropertyName("size");
    m_EnterAnimation->setStartValue(QSize(0, 0));
    m_EnterAnimation->setDuration(220);
    m_EnterAnimation->setEasingCurve(QEasingCurve::OutQuad);
    connect(m_EnterAnimation, &QPropertyAnimation::finished, [this] () {
        if (m_EnterAnimation->direction() == QPropertyAnimation::Backward)
        {
            setVisible(false);
        }
    });
    setVisible(false);
}

KiranTips::~KiranTips ()
{
    delete ui;
}

void KiranTips::setShowPosition (KiranTips::ShowPostionHint positionHint)
{
    if (m_showPostion == positionHint)
    {
        return;
    }

    QMargins margins(MARGIN, MARGIN, MARGIN, MARGIN);
    switch (positionHint)
    {
        case POSITION_LEFT:margins.setRight(MARGIN + TRIANGLE_SIDE_LENGTH);
            break;
        case POSITION_RIGHT:margins.setLeft(MARGIN + TRIANGLE_SIDE_LENGTH);
            break;
        case POSITION_TOP:margins.setBottom(MARGIN + TRIANGLE_SIDE_LENGTH);
            break;
        case POSITION_BOTTM:margins.setTop(MARGIN + TRIANGLE_SIDE_LENGTH);
            break;
    }
    this->layout()->setContentsMargins(margins);

    m_showPostion = positionHint;
    update();
}

void KiranTips::setText (const QString &text)
{
    ui->label->setText(text);
}

void KiranTips::setHideTimeout (int ms)
{
    if (ms == 0 && m_fadeOutTimer.isActive())
    {
        m_fadeOutTimer.stop();
        m_fadeOutTimer.setInterval(0);
    }
    else if (m_fadeOutTimer.interval() != ms)
    {
        m_fadeOutTimer.setInterval(ms);
    }
}

void KiranTips::setHideOnClickedEnable (bool enable)
{
    if (m_hideOnClicked == enable)
    {
        return;
    }

    m_hideOnClicked = enable;
}

void KiranTips::showTipAroundWidget (QWidget *widget)
{
    Q_ASSERT(widget != nullptr);

    m_showAroudWidget = widget;

    if (this->isVisible())
    {
        this->setVisible(false);
    }

    if (m_animationEnable)
    {
        m_EnterAnimation->setEndValue(getRightSize());
        m_EnterAnimation->setDirection(QPropertyAnimation::Forward);
        m_EnterAnimation->stop();
        m_EnterAnimation->start();
    }
    else
    {
        resize(getRightSize());
    }

    this->show();

    if (m_fadeOutTimer.interval() != 0)
    {
        m_fadeOutTimer.start();
    }
}

void KiranTips::setAnimationEnable (bool enable)
{
    if (m_animationEnable == enable)
    {
        return;
    }

    m_animationEnable = enable;
}

void KiranTips::setBackgroundColor (QColor color)
{
    if (m_bgColor == color)
    {
        return;
    }
    m_bgColor = color;
}

void KiranTips::hideTip ()
{
    if (m_animationEnable)
    {
        m_EnterAnimation->setDirection(QPropertyAnimation::Backward);
        m_EnterAnimation->start();
    }
    else
    {
        hide();
    }
}

void KiranTips::drawUpArrowBackground (QPainter *painter)
{
    QPainterPath trianglesPath;
    ///三角形顶点X
    int vertexX = 10;
    trianglesPath.moveTo(vertexX, 0);
    trianglesPath.lineTo(vertexX - 4, TRIANGLE_SIDE_LENGTH);
    trianglesPath.lineTo(vertexX + 4, TRIANGLE_SIDE_LENGTH);
    trianglesPath.lineTo(vertexX, 0);
    painter->fillPath(trianglesPath, m_bgColor);

    QPainterPath rectPath;
    rectPath.addRoundRect(0, TRIANGLE_SIDE_LENGTH,
                          width(), height() - TRIANGLE_SIDE_LENGTH,
                          TIPS_RECT_RND, TIPS_RECT_RND);
    painter->fillPath(rectPath, m_bgColor);
}

void KiranTips::drawDownArrowBackground (QPainter *painter)
{
    QPainterPath trianglesPath;
    ///三角形顶点X
    int vertexX = 10;
    trianglesPath.moveTo(vertexX, height());
    trianglesPath.lineTo(vertexX - TRIANGLE_SIDE_LENGTH / 2, height() - TRIANGLE_SIDE_LENGTH);
    trianglesPath.lineTo(vertexX + TRIANGLE_SIDE_LENGTH / 2, height() - TRIANGLE_SIDE_LENGTH);
    trianglesPath.lineTo(vertexX, height());
    painter->fillPath(trianglesPath, m_bgColor);

    QPainterPath rectPath;
    rectPath.addRoundRect(0, 0,
                          width(), height() - TRIANGLE_SIDE_LENGTH,
                          TIPS_RECT_RND, TIPS_RECT_RND);
    painter->fillPath(rectPath, m_bgColor);
}

void KiranTips::drawLeftArrowBackground (QPainter *painter)
{
    QPainterPath trianglesPath;
    trianglesPath.moveTo(0, height() / 2);
    trianglesPath.lineTo(TRIANGLE_SIDE_LENGTH,
                         height() / 2 - TRIANGLE_SIDE_LENGTH / 2);
    trianglesPath.lineTo(TRIANGLE_SIDE_LENGTH,
                         height() / 2 + TRIANGLE_SIDE_LENGTH / 2);
    trianglesPath.lineTo(0, height() / 2);
    painter->fillPath(trianglesPath, m_bgColor);

    QPainterPath rectPath;
    rectPath.addRoundRect(TRIANGLE_SIDE_LENGTH, 0,
                          width() - TRIANGLE_SIDE_LENGTH, height(),
                          TIPS_RECT_RND, TIPS_RECT_RND);
    painter->fillPath(rectPath, m_bgColor);
}

void KiranTips::drawRightArrowBackground (QPainter *painter)
{
    QPainterPath trianglesPath;
    trianglesPath.moveTo(width(), height() / 2);
    trianglesPath.lineTo(width() - TRIANGLE_SIDE_LENGTH, height() / 2 - TRIANGLE_SIDE_LENGTH / 2);
    trianglesPath.lineTo(width() - TRIANGLE_SIDE_LENGTH, height() / 2 + TRIANGLE_SIDE_LENGTH / 2);
    trianglesPath.lineTo(width(), height() / 2);
    painter->fillPath(trianglesPath, m_bgColor);

    QPainterPath rectPath;
    rectPath.addRoundRect(0, 0, width() - TRIANGLE_SIDE_LENGTH, height(), TIPS_RECT_RND, TIPS_RECT_RND);
    painter->fillPath(rectPath, m_bgColor);
}

QSize KiranTips::getRightSize () const
{
    QFont font = ui->label->font();
    QFontMetrics fontMetrics(font);
    QMargins layoutContentMargins = this->layout()->contentsMargins();

    ui->label->setFixedWidth(fontMetrics.width(ui->label->text()) + 10);
    ui->label->setFixedHeight(fontMetrics.height());

    QSize size;
    size.setWidth(ui->label->width() + layoutContentMargins.left() + layoutContentMargins.right());
    size.setHeight(ui->label->height() + layoutContentMargins.top() + layoutContentMargins.bottom());

    return size;
}

void KiranTips::paintEvent (QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing);

    QStyleOption option;
    option.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);

    if (m_showPostion == POSITION_RIGHT)
    {
        drawLeftArrowBackground(&painter);
    }
    else if (m_showPostion == POSITION_LEFT)
    {
        drawRightArrowBackground(&painter);
    }
    else if (m_showPostion == POSITION_TOP)
    {
        drawDownArrowBackground(&painter);
    }
    else if (m_showPostion == POSITION_BOTTM)
    {
        drawUpArrowBackground(&painter);
    }

}

void KiranTips::mousePressEvent (QMouseEvent *event)
{
    this->hideTip();
    QWidget::mousePressEvent(event);
}

void KiranTips::showEvent (QShowEvent *event)
{
    QWidget::showEvent(event);

    if (m_showAroudWidget == nullptr)
    {
        return;
    }

    QPoint widgetLeftPoint = m_showAroudWidget->mapToGlobal(QPoint(0, 0));
    QSize widgetSize = m_showAroudWidget->size();
    int yCenter = widgetLeftPoint.y() + widgetSize.height() / 2;
    QPoint tipLeftTop;
    switch (m_showPostion)
    {
        case POSITION_LEFT:tipLeftTop.setX(widgetLeftPoint.x() - width() - TIPS_SPACING);
            tipLeftTop.setY(yCenter - (height() / 2));
            break;
        case POSITION_RIGHT:tipLeftTop.setX(widgetLeftPoint.x() + widgetSize.width() + TIPS_SPACING);
            tipLeftTop.setY(yCenter - (height() / 2));
            break;
        case POSITION_TOP:tipLeftTop.setX(widgetLeftPoint.x() + 5);
            tipLeftTop.setY(widgetLeftPoint.y() - height() - TIPS_SPACING);
            break;
        case POSITION_BOTTM:tipLeftTop.setX(widgetLeftPoint.x() + 5);
            tipLeftTop.setY(widgetLeftPoint.y() + widgetSize.height() + TIPS_SPACING);
            break;
    }
    tipLeftTop = parentWidget()->mapFromGlobal(tipLeftTop);
    this->move(tipLeftTop);
}
