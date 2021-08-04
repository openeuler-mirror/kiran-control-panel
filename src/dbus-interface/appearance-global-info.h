#ifndef APPEARANCEGLOBALINFO_H
#define APPEARANCEGLOBALINFO_H

#include <QObject>

class ComKylinsecKiranSessionDaemonAppearanceInterface;
class AppearanceGlobalInfo : public QObject
{
    Q_OBJECT
public:
    AppearanceGlobalInfo(QObject *parent = nullptr);
    ~AppearanceGlobalInfo();

    static AppearanceGlobalInfo *instance();

    bool getAllThemes(int themeType, QString &themesJson);
    bool setTheme(int themeType, QString themeName);
    bool getTheme(int type,QString &theme);

    QString getDesktopBackground();
    bool setDesktopBackground(QString);

    QString getLockScreenBackground();
    bool setLockScreenBackground(QString);

    QStringList getFont(int type);
    bool setFont(int fontType, QStringList fontInfoList);

private:
    ComKylinsecKiranSessionDaemonAppearanceInterface* m_appearanceInterface;
};

#endif // APPEARANCEGLOBALINFO_H
