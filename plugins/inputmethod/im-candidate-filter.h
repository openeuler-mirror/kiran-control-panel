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
 * onlyCurrentLanguage 由 GSettings 提供，默认只列出与当前系统语言同语族的候选。
 * fcitx-keyboard-us 豁免语言预过滤，保证误删后仍可重新搜索添加；它仍参与文本搜索，
 * 因此无关搜索词下不会强制显示。
 */
class ImCandidateFilter
{
public:
    ImCandidateFilter() = default;

    void setOnlyCurrentLanguage(bool only) { m_onlyCurrentLanguage = only; }
    bool onlyCurrentLanguage() const { return m_onlyCurrentLanguage; }

    /// 覆盖参与匹配的系统语言，形如 "zh_CN"。留空则取 QLocale::system()
    void setLocaleName(const QString& localeName) { m_localeName = localeName; }
    QString localeName() const;

    /// 候选是否应进入「添加输入法」列表；文本搜索由对话框另行处理
    bool accepts(const ImEntry& entry) const;

private:
    static bool isLanguageFilterExempt(const ImEntry& entry);
    bool languageMatches(const QString& langCode) const;

private:
    bool m_onlyCurrentLanguage = true;
    QString m_localeName;
};

#endif  // __IM_CANDIDATE_FILTER_H__
