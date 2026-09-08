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

#include "key-sequence-edit/key-formatter.h"

/// fcitx4 热键编解码（单键下划线分隔；多键空格分隔，最多 2 组）
class Fcitx4KeyFormatter : public KeyFormatter
{
public:
    bool decode(const QString& stored, KeyEntry* entry) const override;
    bool encode(const KeyEntry& entry, QString* stored) const override;
    bool decodeList(const QString& stored, KeyEntryList* entries) const;
    bool encodeList(const KeyEntryList& entries, QString* stored) const;
};
