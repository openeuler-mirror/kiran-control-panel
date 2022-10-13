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
 * Author:     yangxiaoqing <yangxiaoqing@kylinsec.com.cn>
 */

#ifndef KIRANDISPLAYCONFIGIDENTIFYING_H
#define KIRANDISPLAYCONFIGIDENTIFYING_H

#include <QWidget>

class QEventLoop;
class  KiranDisplayConfigIdentifying : public QWidget
{
    Q_OBJECT
public:
    explicit KiranDisplayConfigIdentifying(QWidget *parent = 0);
    ~KiranDisplayConfigIdentifying();

    void exec(const QPoint &pos);

    void setText(const QString &text);

private:
    void hideEvent(QHideEvent *event);
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

private:
    QEventLoop *m_loop;
    QString m_text;
};


#endif // KIRANDISPLAYCONFIGIDENTIFYING_H
