#ifndef KCMMANAGER_H
#define KCMMANAGER_H

#include <QObject>
#include <QSharedPointer>

class KSMMouseProxy;
class KSMTouchPadProxy;
class KCMManager : public QObject
{
    Q_OBJECT
public:
    explicit KCMManager(QObject *parent = nullptr);
    ~KCMManager();

    bool isValidConnect();
    QSharedPointer<KSMMouseProxy> getMouseInterface();
    QSharedPointer<KSMTouchPadProxy> getTouchPadInterface();
    bool hasTouchPad();

private:
    QSharedPointer<KSMMouseProxy> m_mouseInterface;
    QSharedPointer<KSMTouchPadProxy> m_touchPadInterface;
};

#endif // KCMMANAGER_H
