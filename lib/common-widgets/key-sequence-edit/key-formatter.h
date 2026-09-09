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

#ifndef KEY_FORMATTER_H
#define KEY_FORMATTER_H

#include <QList>
#include <QString>
#include <Qt>

/// 单个快捷键触发组合（格式无关）
///
/// qtKey 是实际触发键；modifiers 是触发时完整的修饰键状态。若实际触发键本身
/// 是修饰键，modifiers 也包含它，因此 Ctrl+Shift 可表达为「触发 Shift，状态
/// 为 Ctrl|Shift」。keysym 和 side 只用于保留 X11 的左右修饰键信息。
struct KeyEntry
{
    int qtKey = 0;                       ///< Qt::Key（布局相关，作兜底）
    Qt::KeyboardModifiers modifiers = Qt::NoModifier;  ///< 主键按下时的修饰键位掩码
    quint32 keysym = 0;                  ///< X11 keysym（nativeVirtualKey），0 表示不可用
    enum Side
    {
        SideNone,
        SideLeft,
        SideRight
    } side = SideNone;                   ///< 修饰键侧别（Shift_L vs Shift_R）

    // C++11 下带默认成员初始化器的结构体非 aggregate，无法聚合初始化，需显式构造
    KeyEntry() = default;
    KeyEntry(int key, Qt::KeyboardModifiers mods, quint32 sym, Side s)
        : qtKey(key), modifiers(mods), keysym(sym), side(s)
    {
    }

    bool isModifier() const;
    /// 比较快捷键的 canonical identity：优先使用双方都具备的 X11 keysym，缺失时回退到 qtKey。
    bool operator==(const KeyEntry& other) const;
    bool operator!=(const KeyEntry& other) const { return !(*this == other); }
};

using KeyEntryList = QList<KeyEntry>;

/// 后端快捷键编解码策略。
///
/// 此基类不包含任何输入法后端知识：具体插件提供自己的实现并注入录制控件和
/// 后端 controller。控件层只持有 KeyEntry，展示文本由基类的通用实现生成。
class KeyFormatter
{
public:
    virtual ~KeyFormatter() = default;

    /// 后端存储字符串 → 格式无关的内部键值。空字符串表示未设置，解析成功。
    virtual bool decode(const QString& stored, KeyEntry* entry) const = 0;

    /// 格式无关的内部键值 → 后端存储字符串。无法由该后端表达时返回 false。
    virtual bool encode(const KeyEntry& entry, QString* stored) const = 0;

    /// 格式无关的内部键值 → 面向用户的展示文本，不泄漏后端序列化格式。
    static QString displayText(const KeyEntry& entry);
};

#endif  // KEY_FORMATTER_H
