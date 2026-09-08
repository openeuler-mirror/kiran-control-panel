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

#include <QList>
#include <QMetaType>
#include <QString>

struct ImEntry
{
    // 主键，如 "pinyin", "keyboard-us" / "fcitx-keyboard-us"
    QString uniqueName;
    // 已本地化显示名，UI 主显示
    QString name;
    // 母语名，常为空（键盘引擎为空，回退到 name）
    QString nativeName;
    // 主题图标名，如 "fcitx-pinyin", "input-keyboard"
    // （fcitx5 由 D-Bus 上报原样透传；fcitx4 IMList 无此字段，
    //  由 Fcitx4Controller 按 "fcitx-<uniqueName>" 派生，键盘项留空）
    QString iconName;
    // 托盘短标签，如 "拼", "en"
    QString label;
    // 语言代码，如 "zh_CN", "en"
    QString langCode;
    // 写回 a(ss) 第二段 layoutOverride，常为空
    QString layout;
    // addon 名，如 "pinyin", "keyboard"（fcitx4 无此字段，为空）
    QString addon;
    bool configurable = false;

    // 是否为键盘布局项：由各框架兼容层在构建 ImEntry 时标记
    // fcitx5 按 addon=="keyboard" 或 uniqueName 前缀
    // fcitx4 按 fcitx-keyboard- 前缀
    // 消费方（过滤/图标）只读此字段，不感知具体框架命名规则。
    bool isKeyboard = false;
};
