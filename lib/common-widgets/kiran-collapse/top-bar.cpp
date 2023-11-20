#include "top-bar.h"
#include "ui_top-bar.h"

#include <kiran-style/style-global-define.h>
#include <kiran-style/style-palette.h>
#include <kiran-switch-button.h>
#include <QMouseEvent>
#include <QPainter>
#include "QtSvg/QSvgRenderer"
#include "top-bar-flag-pixmap.h"

TopBar::TopBar(QWidget* parent) : QWidget(parent),
                                  ui(new Ui::TopBar)
{
    ui->setupUi(this);
    init();
}

TopBar::~TopBar()
{
    delete ui;
}

void TopBar::setTitle(const QString& title)
{
    m_title = title;
    ui->title->setText(m_title);
}

void TopBar::init()
{
    this->setContentsMargins(12, 5, 12, 5);
    ui->horizontalLayout->setSpacing(m_spacing);
    this->setFixedHeight(m_height);
    this->refreshFlagPixmap(true);

    auto stylePalette = Kiran::StylePalette::instance();
    connect(stylePalette, &Kiran::StylePalette::themeChanged, this, [=](Kiran::PaletteType paletteType) {
        // 将 QPixmap 转换为 QImage
        QImage image = ui->flag->pixmap()->toImage();

        // 反转颜色
        for (int y = 0; y < image.height(); ++y)
        {
            for (int x = 0; x < image.width(); ++x)
            {
                QColor color = image.pixelColor(x, y);
                QColor invertedColor = QColor(255 - color.red(), 255 - color.green(), 255 - color.blue(), color.alpha());
                image.setPixelColor(x, y, invertedColor);
            }
        }

        // 将反转颜色后的 QImage 设置为标志的 pixmap
        ui->flag->setPixmap(QPixmap::fromImage(image));
    });
}

void TopBar::setFlagPixmap(const QString& flag_url)
{
    m_flag = QPixmap(flag_url).scaled(ui->flag->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->flag->setPixmap(m_flag);
}

void TopBar::setFlagPixmap(const QPixmap& pixmap)
{
    m_flag = pixmap;
    ui->flag->setPixmap(m_flag);
}

void TopBar::refreshFlagPixmap(bool isExpanded)
{
    auto stylePalette = Kiran::StylePalette::instance();
    auto styleType = stylePalette->paletteType();
    if (isExpanded)
    {
        ui->flag->setPixmap((styleType == Kiran::PALETTE_DARK) ? FlagPixmap::expansionFlagPixmap().scaled(ui->flag->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation) : FlagPixmap::expansionFlagPixmapDark().scaled(ui->flag->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        return;
    }
    ui->flag->setPixmap((styleType == Kiran::PALETTE_DARK) ? FlagPixmap::collapseFlagPixmap().scaled(ui->flag->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation) : FlagPixmap::collapseFlagPixmapDark().scaled(ui->flag->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void TopBar::addWidget(QWidget* widget)
{
    ui->horizontalLayout->insertWidget(ui->horizontalLayout->count() - 1, widget);
}

void TopBar::mousePressEvent(QMouseEvent* event)
{
    m_mousePos = QPoint(event->x(), event->y());
}

void TopBar::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_mousePos == QPoint(event->x(), event->y()))
    {
        emit clickedBar();
    }
}
