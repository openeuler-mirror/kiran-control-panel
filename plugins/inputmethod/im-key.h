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

#ifndef IM_KEY_H
#define IM_KEY_H

/// 设置项键名协议常量
///
/// 页面只按协议键名读写设置项，不感知 fcitx4 的配置树结构；
/// 键名到 fcitx4 config（section/key）的映射由 Fcitx4Controller 负责。
/// 跨翻译单元多处 include 时 constexpr 隐含 const → internal linkage，无 ODR 问题。
namespace ImKey
{
constexpr const char* triggerKeys = "triggerKeys";
constexpr const char* shareInputState = "shareInputState";
constexpr const char* imSwitchBackwardKey = "imSwitchBackwardKey";
}  // namespace ImKey

#endif  // IM_KEY_H
