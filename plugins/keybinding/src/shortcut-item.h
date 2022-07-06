/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-keybinding is licensed under Mulan PSL v2.
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

#ifndef SHORTCUTITEM_H
#define SHORTCUTITEM_H

#include <QWidget>

#include "kiran-frame/kiran-frame.h"
#include "keybinding_def.h"

namespace Ui
{
class ShortcutItem;
}

class ShortcutItem : public KiranFrame
{
    Q_OBJECT
public:
    explicit ShortcutItem(int type, ShortcutInfoPtr shortcutInfo, QWidget *parent = 0);
    ~ShortcutItem();

    void setName(QString name);
    void setKeyBinding(QString);
    void setAction(QString action);
    void setEditMode(bool isEditMode);

    QString getName();
    int getType();
    QString getUid();
    QString getShowKeybinding();
    ShortcutInfoPtr getShortcut();

protected:
    //void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void initUI();

signals:
    void sigDelete(QString uid);
    void sigClicked(int type, QString uid, QString name, QString key_combination, QString action);

private:
    Ui::ShortcutItem *ui;
    int m_type;
    ShortcutInfoPtr m_shortcutInfo;
    bool m_isPressed = false;
};

#endif  // SHORTCUTITEM_H
