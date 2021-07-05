#ifndef ICONTHEMES_H
#define ICONTHEMES_H

#include <QWidget>

namespace Ui {
class IconThemes;
}

class ThemeWidgetGroup;
class ComKylinsecKiranSessionDaemonAppearanceInterface;
class IconThemes : public QWidget
{
    Q_OBJECT

public:
    explicit IconThemes(QWidget *parent = 0);
    ~IconThemes();
    bool initUI();

private:
    bool getIconThemes(int themeType);
    int  getJsonValueFromString(QString jsonString);
    void createIconWidgets();

signals:
    void sigSetIconTheme(bool);

private:
    Ui::IconThemes *ui;
    ComKylinsecKiranSessionDaemonAppearanceInterface *m_appearanceInterface;
    QStringList m_iconThemes;
    QStringList m_iconThemesPath;
    QString m_currentIconTheme;
    ThemeWidgetGroup* m_iconThemeWidgetGroup;
};

#endif // ICONTHEMES_H
