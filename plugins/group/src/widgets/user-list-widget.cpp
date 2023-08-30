#include "user-list-widget.h"
#include <kiran-label.h>
#include <style-palette.h>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include "auxiliary.h"
#include "kiran-switch-button.h"

UserListWidget::UserListWidget(QWidget* parent)
    : KiranFrame(parent)
{
    initUI();
}

UserListWidget::~UserListWidget()
{
}

void UserListWidget::setUserData(QVariant userData)
{
    m_userData = userData;
}

void UserListWidget::setText(const QString& text)
{
    m_label->setText(text);
}

QString UserListWidget::getText()
{
    return m_label->text();
}

void UserListWidget::setLeftButtonVisible(bool visible, const QString& iconPath)
{
    m_leftButton->setVisible(visible);
    m_leftIcon = iconPath;
    updateIcon();
}

void UserListWidget::setRightButtonVisible(bool visible, const QString& iconPath)
{
    m_rightButton->setVisible(visible);
    m_rightIcon = iconPath;
    updateIcon();
}

bool UserListWidget::getRightButtionVisible()
{
    return m_rightButton->isVisible();
}

void UserListWidget::setSwitcherVisible(bool visible)
{
    m_switcher->setVisible(visible);
}

void UserListWidget::setSwitcherChecked(bool checked)
{
    m_switcher->setChecked(checked);
}

void UserListWidget::setClickable(bool clickable)
{
    if (m_clickable == clickable)
    {
        return;
    }

    m_clickable = clickable;
    setAttribute(Qt::WA_Hover, m_clickable);
}

void UserListWidget::initUI()
{
    setDrawBroder(false);
    setFixedHeight(36);

    auto layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(6);

    m_leftButton = new QPushButton(this);
    m_leftButton->setFixedSize(16, 16);
    m_leftButton->setFlat(true);
    m_leftButton->setVisible(false);
    layout->addWidget(m_leftButton);
    connect(m_leftButton, &QPushButton::clicked, this, [this]()
            { emit leftButtonClicked(m_userData); });

    m_label = new KiranLabel(this);
    m_label->setAlignment(Qt::AlignVCenter);
    m_label->setElideMode(Qt::ElideRight);
    layout->addWidget(m_label, 0, Qt::AlignVCenter);

    layout->addStretch();

    m_switcher = new KiranSwitchButton(this);
    m_switcher->setVisible(false);
    layout->addWidget(m_switcher, 0, Qt::AlignVCenter);
    connect(m_switcher, &KiranSwitchButton::toggled, this, [this](bool checked)
            { emit switchButtonToggled(m_userData, checked); });

    m_rightButton = new QPushButton(this);
    m_rightButton->setFixedSize(16, 16);
    m_rightButton->setFlat(true);
    m_rightButton->setVisible(false);
    layout->addWidget(m_rightButton);
    connect(m_rightButton, &QPushButton::clicked, this, [this]()
            { emit rightButtonClicked(m_userData); });

    updateIcon();
    connect(Kiran::StylePalette::instance(), &Kiran::StylePalette::themeChanged, this, &UserListWidget::updateIcon);
}

void UserListWidget::updateIcon()
{
    auto kiranPalette = Kiran::StylePalette::instance();

    QMap<QPushButton*, QString> buttonIconMap = {
        {m_leftButton, m_leftIcon},
        {m_rightButton, m_rightIcon}};

    for (auto iter = buttonIconMap.begin(); iter != buttonIconMap.end(); iter++)
    {
        auto button = iter.key();
        auto iconPath = iter.value();

        QIcon icon(iconPath);
        auto pixmap = icon.pixmap(QSize(16, 16));
        if (kiranPalette->paletteType() == Kiran::PALETTE_LIGHT)
        {
            auto image = pixmap.toImage();
            image.invertPixels(QImage::InvertRgb);
            pixmap = QPixmap::fromImage(image);
        }

        button->setIcon(pixmap);
    }
}

void UserListWidget::mousePressEvent(QMouseEvent* event)
{
    if (m_clickable)
    {
        m_clicked = true;
    }

    KiranFrame::mousePressEvent(event);
}

void UserListWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_clickable && m_clicked)
    {
        emit clicked();
    }
    m_clicked = false;

    KiranFrame::mouseReleaseEvent(event);
}