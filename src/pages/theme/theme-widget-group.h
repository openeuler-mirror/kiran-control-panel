#ifndef THEMEWIDGETGROUP_H
#define THEMEWIDGETGROUP_H
#include <QWidget>
#include <QObject>
class ThemeWidget;
class ThemeWidgetGroup : public QWidget
{
    Q_OBJECT
public:
    explicit ThemeWidgetGroup(QWidget *parent = 0);
    ~ThemeWidgetGroup();

    void addWidget(ThemeWidget* widget);
    void setCurrentWidget(ThemeWidget*);

signals:
    void themeWidgetChange(ThemeWidget* ,ThemeWidget*);

private:
    ThemeWidget* m_currThemeWidget = nullptr;
    ThemeWidget* m_preThemeWidget = nullptr;

};

#endif // THEMEWIDGETGROUP_H
