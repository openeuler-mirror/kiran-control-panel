#ifndef USERSCONTAINER_H
#define USERSCONTAINER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QBoxLayout;
QT_END_NAMESPACE

class UsersContainer : public QWidget
{
    Q_OBJECT
public:
    UsersContainer(QWidget* parent = nullptr);
    virtual ~UsersContainer();

    void addFeatureItem(QWidget* widget);
    QList<QWidget*> getAllFeatureItem();
    void clear();

private:
    void init();
    void adjustSizeToItmes();

private:
    QBoxLayout* m_mainLayout;
    QBoxLayout* m_containerLayout;
};
#endif  // USERSCONTAINER_H
