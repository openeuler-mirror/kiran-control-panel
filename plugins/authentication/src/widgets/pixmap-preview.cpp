#include "pixmap-preview.h"
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QResizeEvent>
#include <QDebug>

PixmapPreview::PixmapPreview(QWidget* parent)
    : QWidget(parent)
{
}

PixmapPreview::~PixmapPreview()
{
}

void PixmapPreview::setPixmap(const QString& path)
{
    if( !m_pixmap.load(path) && !path.isEmpty() )
    {
        qWarning() << path << "pixmap load failed" << path;
    }
    adjustPixmap(size());
    update();
}

void PixmapPreview::setPixmap(const QPixmap& pixmap)
{
    m_pixmap = pixmap;
    adjustPixmap(size());
    update();
}

void PixmapPreview::adjustPixmap(QSize size)
{
    if( m_pixmap.isNull() )
    {
        return;
    }

    m_scaledPixmap = m_pixmap.scaled(size, Qt::KeepAspectRatioByExpanding);
}

void PixmapPreview::resizeEvent(QResizeEvent* event)
{
    adjustPixmap(event->size());
    QWidget::resizeEvent(event);
}

void PixmapPreview::paintEvent(QPaintEvent* event)
{
    if (!m_scaledPixmap.isNull())
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        auto pixmapRect = m_scaledPixmap.rect();
        pixmapRect.moveCenter(rect().center());
        painter.drawPixmap(pixmapRect, m_scaledPixmap);
    }
    
    QWidget::paintEvent(event);
}
