#ifndef SCROLLAREACONTAINER_H
#define SCROLLAREACONTAINER_H

#include <QObject>
#include <QWidget>

class ScrollAreaContainer : public QWidget
{
    Q_OBJECT
public:
    explicit ScrollAreaContainer(QWidget *parent = nullptr);

    virtual QSize sizeHint() const override;

signals:

protected:
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // SCROLLAREACONTAINER_H
