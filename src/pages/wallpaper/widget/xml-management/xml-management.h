#ifndef XMLMANAGEMENT_H
#define XMLMANAGEMENT_H

#include <QWidget>
#include <QMap>

class QXmlStreamReader;
class XmlManagement : public QWidget
{
    Q_OBJECT

public:
    explicit XmlManagement(QWidget *parent = 0);
    ~XmlManagement();

    bool xmlReader(QString filePath);
    bool xmlWriter();
    void xmlUpdate(QList<QMap<QString, QString> > updateList);
//    void getVisibleWallpaper();
    QList<QMap<QString,QString>> getXmlBodyInfo();

private:
    void loadXmlFiles();
    void parseWallpapers(QXmlStreamReader &reader);

private:
    QMap<QString,QString> m_wallpaperXmlHead;
    QList<QMap<QString,QString>> m_mapList;

    QStringList m_visibleWallpaper;
};

#endif // XMLMANAGEMENT_H
