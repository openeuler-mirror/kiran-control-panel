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

#ifndef __IM_SETTINGS_H__
#define __IM_SETTINGS_H__

/**
 * @brief 输入法插件 GSettings 访问的单一入口
 *
 * schema id、key 名与「key 缺失」防御集中在此，调用方不再各写一份字面量。
 * schema 未安装或缺少 key 时读接口回落默认值、写接口直接失败，均不抛异常。
 */
class ImSettings
{
public:
    /// 是否只显示当前语言的候选，默认 true
    static bool onlyCurrentLanguage();
    /// 写回 only-current-language，成功返回 true
    static bool setOnlyCurrentLanguage(bool value);
};

#endif  // __IM_SETTINGS_H__
