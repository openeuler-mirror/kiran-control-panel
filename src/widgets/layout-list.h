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

#ifndef LAYOUTLIST_H
#define LAYOUTLIST_H

#include <QListWidget>
#include <QTimer>
#include <QWidget>
namespace Ui
{
class LayoutList;
}

class LayoutItem;
class LayoutList : public QWidget
{
    Q_OBJECT

public:
    explicit LayoutList(QWidget *parent = 0);
    ~LayoutList();
    void setCountryList(QStringList layoutList);
    QString getSelectedCountry();
    virtual QSize sizeHint() const override;

public slots:
    void itemClicked();

signals:
    void itemChanged(QString countryName);
    void heightChanged(int height);

private:
    void search();
    void clearFilterItems();
    LayoutItem *addItems(QListWidget *parent, QString text);
    void adjustSize();

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Ui::LayoutList *ui;
    QTimer *m_timer = nullptr;
    QStringList m_filterList;
    QStringList m_lists;
    QString m_countryName = nullptr;
    QScrollBar *m_vb;
};

#endif  // LAYOUTLIST_H
