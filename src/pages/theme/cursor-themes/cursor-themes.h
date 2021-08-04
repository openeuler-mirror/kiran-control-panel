#ifndef CURSORTHEMES_H
#define CURSORTHEMES_H

#include <QObject>
#include <QWidget>

class ThemeWidgetGroup;
class ComKylinsecKiranSessionDaemonAppearanceInterface;
class CursorThemes : public QWidget
{
    Q_OBJECT
public:
    explicit CursorThemes(QWidget *parent = 0);
    bool initUI();

private:
    bool getCursorThemes(int Type);
    int  getJsonValueFromString(QString jsonString);
    QWidget *createCursorWidget();
    QImage convertToNomalImage(const QImage &cursorImage);

signals:
    void sigSetCursorTheme(bool);

private:
    ComKylinsecKiranSessionDaemonAppearanceInterface *m_appearanceInterface;
    QString m_currentCursorTheme;

    QStringList m_cursorThemesName;
    QStringList m_cursorThemesPath;

};

#endif // CURSORTHEMES_H
