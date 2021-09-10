#ifndef DBUSWRAPPER_H
#define DBUSWRAPPER_H
#include <QObject>
#include <QSharedPointer>
class KeyboardBackEndProxy;
class DbusWrapper : public QObject
{
    Q_OBJECT
public:
    explicit DbusWrapper(QObject *parent = nullptr);
    ~DbusWrapper();
    bool isValidConnect();
    QSharedPointer<KeyboardBackEndProxy> getKeyboardInterface();

private:
    QSharedPointer<KeyboardBackEndProxy> m_keyboardInterface;
};

#endif  // DBUSWRAPPER_H
