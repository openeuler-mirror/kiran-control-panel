#ifndef SCROLLCONTAINER_H
#define SCROLLCONTAINER_H

#include <QWidget>

class ScrollContainer : public QWidget
{
    Q_OBJECT
public:
    explicit ScrollContainer(QWidget *parent = nullptr);

signals:
    void resized(QSize size);
protected:
    virtual void resizeEvent(QResizeEvent *e);
};

#endif // SCROLLCONTAINER_H
