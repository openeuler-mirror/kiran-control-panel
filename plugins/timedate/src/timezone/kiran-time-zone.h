/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-timedate is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#ifndef KIRANTIMEZONE_H
#define KIRANTIMEZONE_H

#include <QWidget>

namespace Ui {
class KiranTimeZone;
}

#include <QEvent>

class KiranTimeZone : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool editHasFocus READ editHasFocus WRITE setEditHasFocus NOTIFY editHasFocusChanged)
public:
    explicit KiranTimeZone(QWidget *parent = nullptr);
    ~KiranTimeZone();

    bool editHasFocus() const;
    virtual QSize sizeHint() const override;

    bool save();

    void reset();

    Q_INVOKABLE void scrollToCurrent();
private:
    void initUI();

public slots:
    void setEditHasFocus(bool editHasFocus);

signals:
    void editHasFocusChanged(bool editHasFocus);

protected:
    virtual bool event(QEvent *event) override {
        if(event->type()==QEvent::ShowToParent){
            scrollToCurrent();
        }
        return QWidget::event(event);
    }

    virtual void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::KiranTimeZone *ui;
    bool m_editHasFocus;;
    QString m_selectedZoneID;
};

#endif // KIRANTIMEZONE_H
