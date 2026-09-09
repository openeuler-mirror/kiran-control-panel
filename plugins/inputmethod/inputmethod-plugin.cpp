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

#include "inputmethod-plugin.h"
#include "fcitx4/fcitx4-controller.h"
#include "fcitx4/fcitx4-im-subitem.h"
#include "im-candidate-filter.h"
#include "logging-category.h"

Q_LOGGING_CATEGORY(qLcInputMethod, "kcp.inputmethod", QtMsgType::QtDebugMsg);

InputMethodPlugin::InputMethodPlugin(QObject* parent)
    : QObject(parent)
{
}

InputMethodPlugin::~InputMethodPlugin()
{
}

int InputMethodPlugin::init(KiranControlPanel::PanelInterface* interface)
{
    Q_UNUSED(interface);

    // 「添加输入法」的候选过滤策略：2.6 的 PanelInterface 无 queryCofnig 配置下发，
    // 使用默认策略（关闭键盘布局显示、仅当前语言）。
    ImCandidateFilter filter;
    KLOG_DEBUG(qLcInputMethod) << "candidate filter: allowKeyboardLayout"
                               << filter.allowKeyboardLayout()
                               << "onlyCurrentLanguage" << filter.onlyCurrentLanguage()
                               << "locale" << filter.localeName();

    // 本插件服务fcitx4，子项注册只表示提供输入法设置入口，不依赖初始化瞬间的D-Bus在线状态。
    // Fcitx4ImPage构造时创建Fcitx4Controller，由其service watcher持续探测fcitx4服务的上线/掉线，避免服务晚于控制中心启动时子项永久缺失。
    // 注意：无论当前服务状态如何都返回 0，非 0 会被PluginV2::load判定为初始化失败并卸载整个插件。
    KLOG_DEBUG(qLcInputMethod) << "register fcitx4 input method subitem;"
                               << "fcitx4 available now:" << Fcitx4Controller::probeAvailable();
    m_subitems.append(KiranControlPanel::SubItemPtr(new Fcitx4ImSubItem(filter)));
    return 0;
}

void InputMethodPlugin::uninit()
{
}

QVector<KiranControlPanel::SubItemPtr> InputMethodPlugin::getSubItems()
{
    return m_subitems;
}
