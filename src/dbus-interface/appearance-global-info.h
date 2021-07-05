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

private:
    ComKylinsecKiranSessionDaemonAppearanceInterface* m_appearanceInterface;
};

#endif // APPEARANCEGLOBALINFO_H
