//
// Created by lxh on 2020/11/5.
//

#ifndef KIRAN_ACCOUNT_MANAGER_MASK_WIDGET_H
#define KIRAN_ACCOUNT_MANAGER_MASK_WIDGET_H

#include <QWidget>

class MaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MaskWidget (QWidget *parent);
    ~MaskWidget () = default;

private Q_SLOTS:

protected:
    virtual bool eventFilter (QObject *watched, QEvent *event) override;

private:
    QWidget *const m_maskedWidget;
};


#endif //KIRAN_ACCOUNT_MANAGER_MASK_WIDGET_H
