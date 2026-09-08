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

#include <QString>
#include <QStringList>

/// fcitx4 config 文件读改写（默认 XDG 下 fcitx/config）。
/// fcitx4 开发库为 LGPL，故自行实现。
class Fcitx4ConfigFile
{
public:
    /// filePath 为空时使用默认路径
    explicit Fcitx4ConfigFile(const QString& filePath = {});

    /// 读入内存行缓存；文件不存在/不可读返回 false
    bool load();

    /// 读键值：实际键优先，否则回退注释行默认值；无键时 ok=false
    QString value(const QString& section, const QString& key, bool* ok = nullptr) const;

    /// 写键值到内存缓存（须再调 save()）；段不存在返回 false
    bool setValue(const QString& section, const QString& key, const QString& value);

    /// 原子写盘；若 load 后文件被外部改动则放弃并返回 false
    bool save();

    /// 实际文件路径
    QString filePath() const;

private:
    /// 行内 token 拆分
    static bool splitLine(const QString& line, QStringList* tokens);

private:
    QString m_filePath;
    QStringList m_lines;  // 逐行原文（不含换行符）
    qint64 m_loadedMTime = 0;
};
