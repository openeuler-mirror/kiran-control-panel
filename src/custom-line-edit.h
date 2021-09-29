#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QLineEdit>
#include <QWidget>

namespace Ui
{
class CustomLineEdit;
}

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit CustomLineEdit(QWidget *parent = 0);
    ~CustomLineEdit();
    void setIcon(QString icon);
    void hideIcon();
    void hideButton();
    void setPlaceholderText(QString text);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

    virtual void inputMethodEvent(QInputMethodEvent *event);

private:
    void initUI();

private:
    Ui::CustomLineEdit *ui;
};

#endif  // CUSTOMLINEEDIT_H
