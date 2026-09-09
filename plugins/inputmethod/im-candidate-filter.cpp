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

#include "im-candidate-filter.h"

#include <QLocale>

QString ImCandidateFilter::localeName() const
{
    return m_localeName.isEmpty() ? QLocale::system().name() : m_localeName;
}

bool ImCandidateFilter::languageMatches(const QString& langCode) const
{
    // fcitx5 的 langCode 粒度不统一：输入引擎多为 "zh_CN"，键盘布局多为 "zh"/"en"，
    // 另有一批（盲文、国际音标等）为空。取前两字符做语族比较，
    // 并用「系统 locale 以该语族开头」的方向匹配（"zh_CN".startsWith("zh")）。
    const QString language = langCode.left(2);
    if (language.isEmpty())
    {
        // 语言未知的候选无法判断归属，开启语言过滤时一律不列出
        return false;
    }

    return localeName().startsWith(language);
}

bool ImCandidateFilter::accepts(const ImEntry& entry) const
{
    if (!m_allowKeyboardLayout && entry.isKeyboard)
    {
        return false;
    }

    if (m_onlyCurrentLanguage && !languageMatches(entry.langCode))
    {
        return false;
    }

    return true;
}
