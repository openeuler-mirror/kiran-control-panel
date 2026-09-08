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

#include "fcitx4-config-file.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <QTextStream>

namespace
{
/// 行内 key=value token 及其在 body 中的位置
struct Token
{
    QString key;
    QString value;
    int pos = 0;
    int len = 0;
};

/// 解析一行正文（已去整行 # 与首尾空白），返回全部 key=value token
QVector<Token> scanBodyTokens(const QString& body)
{
    QVector<Token> tokens;
    int i = 0;
    const int n = body.size();
    while (i < n) {
        while (i < n && body.at(i).isSpace())
            ++i;
        if (i >= n)
            break;

        const int start = i;
        while (i < n && !body.at(i).isSpace())
            ++i;
        QString tok = body.mid(start, i - start);

        // 兼容 token 前导 '#'（如行内 "#Key=v"）
        if (tok.startsWith(QLatin1Char('#')))
            tok.remove(0, 1);

        const int eq = tok.indexOf(QLatin1Char('='));
        if (eq > 0) {
            Token t;
            t.key = tok.left(eq).trimmed();
            t.value = tok.mid(eq + 1);
            t.pos = start;
            t.len = i - start;
            tokens.append(t);
        }
    }
    return tokens;
}

bool isCommentLine(const QString& line)
{
    return line.trimmed().startsWith(QLatin1Char('#'));
}

/// 从 body 删除 [pos, pos+len) 的 token 及其前导空白
QString removeTokenFromBody(const QString& body, int pos, int len)
{
    QString result = body;
    result.remove(pos, len);
    int p = pos;
    while (p > 0 && result.at(p - 1).isSpace())
        --p;
    if (p < pos)
        result.remove(p, pos - p);
    return result.trimmed();
}
}  // namespace

Fcitx4ConfigFile::Fcitx4ConfigFile(const QString& filePath)
    : m_filePath(filePath.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
                                      + QStringLiteral("/fcitx/config")
                                    : filePath)
{
}

QString Fcitx4ConfigFile::filePath() const
{
    return m_filePath;
}

bool Fcitx4ConfigFile::load()
{
    m_lines.clear();
    m_loadedMTime = 0;

    QFile file(m_filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream stream(&file);
    while (!stream.atEnd())
        m_lines.append(stream.readLine());
    m_loadedMTime = QFileInfo(m_filePath).lastModified().toMSecsSinceEpoch();
    return true;
}

QString Fcitx4ConfigFile::value(const QString& section, const QString& key, bool* ok) const
{
    if (ok)
        *ok = false;

    // 定位段起始行
    int start = -1;
    for (int i = 0; i < m_lines.size(); ++i) {
        const QString trimmed = m_lines.at(i).trimmed();
        if (trimmed.startsWith(QLatin1Char('[')) && trimmed.endsWith(QLatin1Char(']'))) {
            if (trimmed.mid(1, trimmed.size() - 2).trimmed() == section)
                start = i;
            else if (start >= 0)
                break;
        }
    }
    if (start < 0)
        return {};

    QString fallback;
    for (int i = start + 1; i < m_lines.size(); ++i) {
        const QString& line = m_lines.at(i);
        const QString trimmed = line.trimmed();
        if (trimmed.startsWith(QLatin1Char('[')) && trimmed.endsWith(QLatin1Char(']')))
            break;

        const bool comment = isCommentLine(line);
        const QString body = (comment ? trimmed.mid(1) : trimmed);
        const QVector<Token> tokens = scanBodyTokens(body);
        for (const auto& t : tokens) {
            if (t.key != key)
                continue;
            if (!comment) {
                if (ok)
                    *ok = true;
                return t.value;  // 实际键优先
            }
            fallback = t.value;
        }
    }

    if (!fallback.isNull()) {
        if (ok)
            *ok = true;
        return fallback;  // 回退注释行默认值
    }
    return {};
}

bool Fcitx4ConfigFile::setValue(const QString& section, const QString& key, const QString& value)
{
    if (key.isEmpty())
        return false;

    // 定位段范围 [start, end)
    int start = -1;
    int end = m_lines.size();
    for (int i = 0; i < m_lines.size(); ++i) {
        const QString trimmed = m_lines.at(i).trimmed();
        if (trimmed.startsWith(QLatin1Char('[')) && trimmed.endsWith(QLatin1Char(']'))) {
            if (trimmed.mid(1, trimmed.size() - 2).trimmed() == section) {
                start = i;
            } else if (start >= 0) {
                end = i;
                break;
            }
        }
    }
    if (start < 0)
        return false;

    const QString newToken = key + QLatin1Char('=') + value;

    // 1) 实际键行：就地替换
    for (int i = start + 1; i < end; ++i) {
        const QString& line = m_lines.at(i);
        if (isCommentLine(line))
            continue;
        const QVector<Token> tokens = scanBodyTokens(line.trimmed());
        if (tokens.isEmpty())
            continue;
        for (const auto& t : tokens) {
            if (t.key != key)
                continue;
            if (tokens.size() == 1) {
                m_lines[i] = newToken;
            } else {
                QString body = line.trimmed();
                body.replace(t.pos, t.len, newToken);
                m_lines[i] = body;
            }
            return true;
        }
    }

    // 2) 注释默认键行：转实际行（多键注释行则拆出目标键）
    for (int i = start + 1; i < end; ++i) {
        const QString& line = m_lines.at(i);
        if (!isCommentLine(line))
            continue;
        const QString body0 = line.trimmed().mid(1);
        const QVector<Token> tokens = scanBodyTokens(body0);
        if (tokens.isEmpty())
            continue;
        for (const auto& t : tokens) {
            if (t.key != key)
                continue;
            if (tokens.size() == 1) {
                m_lines[i] = newToken;
            } else {
                const QString newBody = removeTokenFromBody(body0, t.pos, t.len);
                m_lines.insert(i, newToken);
                m_lines[i + 1] = QLatin1Char('#') + (newBody.isEmpty() ? QStringLiteral(" ") : newBody);
            }
            return true;
        }
    }

    // 3) 段内无此键：段尾追加
    m_lines.insert(end, newToken);
    return true;
}

bool Fcitx4ConfigFile::save()
{
    // load 后文件被外部改动则放弃，防覆盖并发编辑
    const QFileInfo cur(m_filePath);
    if (cur.exists() && m_loadedMTime != 0 && cur.lastModified().toMSecsSinceEpoch() != m_loadedMTime)
        return false;

    QString content = m_lines.join(QLatin1Char('\n'));
    if (!content.isEmpty())
        content += QLatin1Char('\n');

    QSaveFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    file.write(content.toUtf8());
    if (!file.commit())
        return false;

    m_loadedMTime = QFileInfo(m_filePath).lastModified().toMSecsSinceEpoch();
    return true;
}
