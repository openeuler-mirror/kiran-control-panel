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

#ifndef __KEYBOARD_SETTINGS_H__
#define __KEYBOARD_SETTINGS_H__

/**
 * @brief 键盘插件 GSettings 访问的单一入口
 *
 * schema id、key 名与「key 缺失」防御集中在此，调用方不再各写一份字面量。
 * schema 未安装或缺少 key 时读接口回落默认值，不抛异常。
 */
class KeyboardSettings
{
public:
    /// 是否提供「键盘布局」子项，默认 false
    ///
    /// 关闭时键盘插件只注册「键盘通用选项」；键盘布局的添加/切换由输入法插件的
    /// 键盘输入法（fcitx-keyboard-*）承担，避免两者同时改写 XKB 设置。
    static bool enableLayoutSubItem();
};

#endif  // __KEYBOARD_SETTINGS_H__
