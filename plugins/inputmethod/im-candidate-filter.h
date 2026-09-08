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

#ifndef __IM_CANDIDATE_FILTER_H__
#define __IM_CANDIDATE_FILTER_H__

#include <QString>

#include "im-entry.h"

/**
 * @brief 「添加输入法」候选过滤策略
 *
 * 两个开关由 gsettings 提供（com.kylinsec.kiran.control-panel.plugin）：
 *   - allowKeyboardLayout   默认 false，不把键盘布局列为候选
 *   - onlyCurrentLanguage   默认 true，只列出与当前系统语言匹配的候选
 *
 * 默认组合下只纯中文输入引擎。
 *
 * 注意：这里是硬过滤，被排除的候选也无法通过搜索找到。这与 fcitx5-configtool
 * 「搜索时绕过语言过滤」的行为不同，是产品上有意的选择。
 */
class ImCandidateFilter
{
public:
    ImCandidateFilter() = default;

    void setAllowKeyboardLayout(bool allow) { m_allowKeyboardLayout = allow; }
    bool allowKeyboardLayout() const { return m_allowKeyboardLayout; }

    void setOnlyCurrentLanguage(bool only) { m_onlyCurrentLanguage = only; }
    bool onlyCurrentLanguage() const { return m_onlyCurrentLanguage; }

    /// 覆盖参与匹配的系统语言，形如 "zh_CN"。留空则取 QLocale::system()
    void setLocaleName(const QString& localeName) { m_localeName = localeName; }
    QString localeName() const;

    /// 候选是否应出现在「添加输入法」列表中
    bool accepts(const ImEntry& entry) const;

private:
    bool languageMatches(const QString& langCode) const;

private:
    bool m_allowKeyboardLayout = false;
    bool m_onlyCurrentLanguage = true;
    QString m_localeName;
};

#endif  // __IM_CANDIDATE_FILTER_H__
