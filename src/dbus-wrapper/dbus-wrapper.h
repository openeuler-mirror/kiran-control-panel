#ifndef DBUSWRAPPER_H
#define DBUSWRAPPER_H
#include <QObject>
#include <QSharedPointer>
class KSKKeyboardProxy;
class DbusWrapper : public QObject
{
    Q_OBJECT
public:
    explicit DbusWrapper(QObject *parent = nullptr);
    ~DbusWrapper();
    bool isValidConnect();
    QSharedPointer<KSKKeyboardProxy> getKeyboardInterface();

private:
    QSharedPointer<KSKKeyboardProxy> m_keyboardInterface;
};

#endif  // DBUSWRAPPER_H
