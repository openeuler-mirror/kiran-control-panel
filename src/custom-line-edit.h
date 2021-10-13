#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QLineEdit>
#include <QWidget>

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit CustomLineEdit(QWidget *parent = 0);
    ~CustomLineEdit();

protected:
    virtual void keyReleaseEvent(QKeyEvent *event);

signals:
    void inputKeyCodes(QList<int> keycodes);

private:
    void initUI();

private:
};

#endif  // CUSTOMLINEEDIT_H
