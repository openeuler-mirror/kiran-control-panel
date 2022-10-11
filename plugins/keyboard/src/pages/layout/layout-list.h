/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
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
    Q_PROPERTY(bool editHasFocus READ editHasFocus WRITE setEditHasFocus NOTIFY editHasFocusChanged)

public:
    explicit LayoutList(QWidget *parent = 0);
    ~LayoutList();

    void setCountryList(QStringList layoutList);

    QString getSelectedCountry();
    bool editHasFocus() const;

    virtual QSize sizeHint() const override;

public slots:
    void itemClicked();
    void setEditHasFocus(bool editHasFocus);

signals:
    void itemChanged(QString countryName);
    void heightChanged(int height);
    void editHasFocusChanged(bool editHasFocus);

private:
    void search();
    void clearFilterItems();
    LayoutItem *addItems(QListWidget *parent, QString text);
    void adjustSize();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::LayoutList *ui;
    QTimer *m_timer = nullptr;
    QStringList m_filterList;
    QStringList m_lists;
    QString m_countryName = nullptr;
    QScrollBar *m_vb;
    bool m_editHasFocus;
};

#endif  // LAYOUTLIST_H
