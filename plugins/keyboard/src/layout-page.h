/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-keyboard is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#ifndef LAYOUTPAGE_H
#define LAYOUTPAGE_H

#include <QMap>
#include <QVBoxLayout>
#include <QWidget>
namespace Ui
{
class LayoutPage;
}

class ChooseItem;
class KeyboardBackEndProxy;
class LayoutPage : public QWidget
{
    Q_OBJECT

public:
    explicit LayoutPage(QWidget *parent = 0);
    ~LayoutPage();
    QSize sizeHint() const override;

private:
    void init();
    void initUI();
    void initConnection();

    void loadValidLayouts();
    void createLayoutItem();
    void updateLayout();

signals:
    void layoutSelectChanged(QString layoutName);

public slots:
    void deleteLayout(QString deletedLayout);
    void setEditMode();
    bool addLayout(QString layoutName);
    void chooseItemClicked();

private:
    Ui::LayoutPage *ui;
    KeyboardBackEndProxy* m_keyboardInterface;
    QMap<QString, QString> m_layoutMap;
    QList<ChooseItem *> m_itemList;
    QStringList m_layoutList;
    QString m_layout;

    //    ChooseItem *m_firstItem;
    QVBoxLayout *m_vLayout;
    bool m_editFlag = false;
};

#endif  // LAYOUTPAGE_H
