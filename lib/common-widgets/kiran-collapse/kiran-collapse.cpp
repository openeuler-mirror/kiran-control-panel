#include "kiran-collapse.h"
#include "ui_kiran-collapse.h"
#include <kiran-style/style-palette.h>

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QStyleOption>
#include <QPainter>
#include <QPropertyAnimation>

KiranCollapse::KiranCollapse(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranCollapse)
{
    init();

}

KiranCollapse::KiranCollapse(bool defaultIsExpand, const QString &title, QWidget* expansionSpaceWidget, QWidget *parent) :
QWidget(parent),
ui(new Ui::KiranCollapse)
{
    m_isExpanded = defaultIsExpand;
    init();
    // 设定顶栏
    ui->topBar->setTitle(title);

    // 设定扩展区
    ui->expansionSpaceContainer->addWidget(expansionSpaceWidget);
}

KiranCollapse::~KiranCollapse()
{
    delete ui;
}

void KiranCollapse::addExpansionSpaceWidget(QWidget* widget)
{
    auto layout = ui->expansionSpaceContainer;
    layout->addWidget(widget);
}

void KiranCollapse::delExpansionSpaceWidget(int index) {
    if (ui->expansionSpaceContainer->count() == 0){
        return;
    }
    auto item = ui->expansionSpaceContainer->takeAt(index);
    if (item) {
        QWidget *widget = item->widget();
        delete widget;
        delete item;
    }
}

void KiranCollapse::delExpansionSpaceWidget(const QString& widgetName) {
    // 通过对象名称查找和删除指定的widget
    QList<QWidget*> widgetsToRemove = ui->expansionSpaceContainer->findChildren<QWidget*>(widgetName);
    for (QWidget *widget : widgetsToRemove) {
        delExpansionSpaceWidget(ui->expansionSpaceContainer->indexOf(widget));
    }
}

void KiranCollapse::delAllExpansionSpaceWidget() {
    while (ui->expansionSpaceContainer->count() != 0)
    {
        QLayoutItem* item = ui->expansionSpaceContainer->takeAt(0);
        if (item)
        {
            delete item->widget();
            delete item;
        }
    }
}

void KiranCollapse::expand()
{
    if (m_isExpanded || m_animationForES->state() == QAbstractAnimation::Running)
    {
        return;
    }
    m_animationForES->setEasingCurve(QEasingCurve::OutCubic);
    m_animationForES->setStartValue(ui->expansionSpace->height());
    m_animationForES->setEndValue(m_maximumExpansionSpaceHeight);
    m_animationForES->start();

    m_isExpanded = true;
    ui->topBar->refreshFlagPixmap(m_isExpanded);
    emit expandSpaceExpanded();
}

void KiranCollapse::collapse()
{
    if (!m_isExpanded || m_animationForES->state() == QAbstractAnimation::Running)
    {
        return;
    }
    m_animationForES->setEasingCurve(QEasingCurve::InCubic);
    m_animationForES->setStartValue(m_maximumExpansionSpaceHeight);
    m_animationForES->setEndValue(0);
    m_animationForES->start();
    m_isExpanded = false;
    ui->topBar->refreshFlagPixmap(m_isExpanded);
    emit expandSpaceCollapsed();
}

void KiranCollapse::changeExpansionSpaceState()
{
    if (m_isExpanded) {
        collapse();
    } else {
        expand();
    }
}

void KiranCollapse::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    QStyle::State state;

    opt.initFrom(this);
    state = opt.state;

    QPainterPath painterPath;
    QRectF qRect = opt.rect;
    qRect.adjust(0.5, 0.5, -0.5, -0.5);
    painterPath.addRoundedRect(qRect, m_radius, m_radius);

    using namespace Kiran;
    auto getStateFunc = [this](QStyle::State state) -> StylePalette::ColorState
    {
        if (!(state & QStyle::State_Enabled))
        {
            return StylePalette::Disabled;
        }
        else if (state & QStyle::State_Sunken)
        {
            return StylePalette::Active;
        }
        else if ((state & QStyle::State_MouseOver) && testAttribute(Qt::WA_Hover))
        {
            return StylePalette::Hover;
        }
        else
        {
            return StylePalette::Normal;
        }
    };

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto kiranPalette = StylePalette::instance();
    if (m_drawBackground)
    {
        QColor backgroundColor;
        backgroundColor = kiranPalette->color(getStateFunc(state),
                                              StylePalette::Widget,
                                              StylePalette::Background);
        painter.fillPath(painterPath, backgroundColor);
    }

    QWidget::paintEvent(event);
}

void KiranCollapse::init() {
    ui->setupUi(this);
    ui->expansionSpace->setAttribute(Qt::WA_StyledBackground);
    ui->expansionSpaceContainer->setSpacing(10);

    setIsExpand(m_isExpanded);
    ui->expansionSpaceContainer->setContentsMargins(m_expansionSpaceContentMarginLeft,
                                                    m_expansionSpaceContentMarginTop,
                                                    m_expansionSpaceContentMarginRight,
                                                    m_expansionSpaceContentMarginBottom);
    setExpandSpaceAnimation();
    connect(ui->topBar, &TopBar::clickedBar, this, &KiranCollapse::changeExpansionSpaceState);
}

bool KiranCollapse::getIsExpand() const {
    return m_isExpanded;
}

void KiranCollapse::setIsExpand(bool isExpanded) {
    m_isExpanded = isExpanded;

    // 设置折叠
    if (!m_isExpanded)
    {
        ui->expansionSpace->setMaximumHeight(0);
    }
    ui->topBar->refreshFlagPixmap(m_isExpanded);
}

void KiranCollapse::addTopBarWidget(QWidget *widget) {
    ui->topBar->addWidget(widget);
}

void KiranCollapse::setExpandSpaceAnimation() {
    m_animationForES = new QPropertyAnimation(ui->expansionSpace, "maximumHeight", this);
    m_animationForES->setDuration(200);
}

void KiranCollapse::setTitle(const QString &title) {
    ui->topBar->setTitle(title);
}

void KiranCollapse::setTobBarFixedHeight(int height) {
    ui->topBar->setFixedHeight(height);
}

void KiranCollapse::setMaximumExpansionHeight(int maxExpandHeight) {
    m_maximumExpansionSpaceHeight = maxExpandHeight;
    ui->expansionSpace->setMaximumHeight(maxExpandHeight);
}
