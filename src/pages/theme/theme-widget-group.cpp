#include "theme-widget-group.h"
#include "theme-widget.h"

ThemeWidgetGroup::ThemeWidgetGroup(QWidget *parent):
    QWidget(parent)
{

}

ThemeWidgetGroup::~ThemeWidgetGroup()
{

}

void ThemeWidgetGroup::addWidget(ThemeWidget *widget)
{
    connect(widget, &ThemeWidget::clicked, [=]{
        m_preThemeWidget = m_currThemeWidget;
        m_currThemeWidget = widget;

        emit themeWidgetChange(m_preThemeWidget, m_currThemeWidget);
    });
}

void ThemeWidgetGroup::setCurrentWidget(ThemeWidget *widget)
{
    m_currThemeWidget = widget;
}
