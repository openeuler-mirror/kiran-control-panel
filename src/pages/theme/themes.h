#ifndef THEMES_H
#define THEMES_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class Themes;
}

class ComKylinsecKiranSessionDaemonAppearanceInterface;
class QButtonGroup;
class IconThemes;
class ThemeWidgetGroup;
class ChooserWidget;
class CursorThemes;
class Themes : public QWidget
{
    Q_OBJECT

public:
    explicit Themes(QWidget *parent = 0);
    ~Themes();
    void setPage(int index);

private:
    bool initUI();
    bool initThemesUI();
    bool initIconThemesUI();
    bool initCursorThemesUI();
    bool getThemes(int themeType);
    int  getJsonValueFromString(QString jsonString, QStringList *themeName, QStringList *themePath);
    void createThemeWidget();

signals:
    void clicked();

private:
    Ui::Themes *ui;
    ComKylinsecKiranSessionDaemonAppearanceInterface *m_appearanceInterface;

    IconThemes *m_iconThemes;
    ChooserWidget *m_chooseIconWidget;
    QString m_currIconThemes;

    CursorThemes *m_cursorThemes;
    ChooserWidget *m_chooseCursorWidget;
    QString m_currCursorThemes;

    //Gtk 主题
    QStringList m_allThemesName;
    QStringList m_allThemesPath;

    QString m_currentTheme;
    QStringList m_themesName;
    ThemeWidgetGroup *m_themeWidgetGroup;
};

#endif // THEMES_H
