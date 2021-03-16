/**
 * @file information-list-item.h
 * @brief 系统信息列表
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#ifndef INFORMATIONLISTITEM_H
#define INFORMATIONLISTITEM_H

#include <QWidget>

namespace Ui {
class InformationListItem;
}

class InformationListItem : public QWidget
{
    Q_OBJECT

public:
    explicit InformationListItem(QWidget *parent = 0);
    ~InformationListItem();

protected:
    void paintEvent(QPaintEvent*event);

public:
    void setItemText(const QString &text);
    void setItemIcon(const QString &iconPath);
    void setItemArrow(bool isSelected);
    virtual QSize sizeHint() const override;

private:
    Ui::InformationListItem *ui;
};

#endif // InformationListItem_H
