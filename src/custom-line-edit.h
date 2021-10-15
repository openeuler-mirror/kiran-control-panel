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
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

signals:
    void inputKeyCodes(QList<int> keycodes);

private:
    void initUI();
    bool m_flag = false;

private:
};

#endif  // CUSTOMLINEEDIT_H
