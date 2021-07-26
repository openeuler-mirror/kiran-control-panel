#ifndef THREADOBJECT_H
#define THREADOBJECT_H

#include <QObject>
#include <QMap>
class XmlManagement;
class ThreadObject : public QObject
{
    Q_OBJECT
public:
    explicit ThreadObject(QObject *parent = nullptr);
    ~ThreadObject();

    void updateWallpaperXml(QList<QMap<QString,QString>> mapList);

public slots:
    void loadWallpaperInfo();

signals:
    void getWallpaperInfo(QList<QMap<QString,QString>> mapList);

private:
    XmlManagement *m_xmlManagement;

    QList<QMap<QString,QString>> m_listMap;
};

#endif // THREADOBJECT_H
