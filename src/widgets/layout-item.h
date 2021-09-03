#ifndef LAYOUTITEM_H
#define LAYOUTITEM_H

#include <QWidget>

namespace Ui
{
class LayoutItem;
}

class LayoutItem : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool isPressed READ isPressed WRITE setisPressed)
public:
    explicit LayoutItem(QWidget *parent = 0);
    ~LayoutItem();
    void setItemText(QString text);
    QString getItemText();
    void selectedItemChanged(QString coutryName);
    void setisPressed(bool pressed);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void clicked();

private:
    bool isPressed() const;

private:
    Ui::LayoutItem *ui;
    bool m_isPressed = false;
};

#endif  // LAYOUTITEM_H
