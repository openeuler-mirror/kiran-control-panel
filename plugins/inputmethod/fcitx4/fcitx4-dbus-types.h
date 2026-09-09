/**
 * Copyright (c) 2020 ~ 2026 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#pragma once

#include <QDBusArgument>
#include <QList>
#include <QMetaType>
#include <QString>
#include <QVector>

/// fcitx4 D-Bus 类型（IMList 属性 a(sssb)）的 Qt 映射。
/// 独立成头供 qdbusxml2cpp 生成 proxy 时 -i include（XML 的 QtTypeName 引用
/// Fcitx4ImItemList），同时 controller 直接使用该类型（原 Fcitx4Controller::ImListItem）。
/// 编解码顺序与 fcitx4 daemon 一致：a(sssb) = (name, uniqueName, langCode, enabled)。
struct Fcitx4ImItem
{
    QString name;        // 显示名，如 "Pinyin" / "English (US)"
    QString uniqueName;  // 如 "pinyin" / "fcitx-keyboard-us"
    QString langCode;    // 如 "zh_CN" / "en_US"，可能为空
    bool enabled = false;
};
typedef QList<Fcitx4ImItem> Fcitx4ImItemList;

Q_DECLARE_METATYPE(Fcitx4ImItem)
Q_DECLARE_METATYPE(Fcitx4ImItemList)

QDBusArgument& operator<<(QDBusArgument& arg, const Fcitx4ImItem& item);
const QDBusArgument& operator>>(const QDBusArgument& arg, Fcitx4ImItem& item);
