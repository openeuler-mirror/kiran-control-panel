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
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */
#ifndef FONT_INFO_H
#define FONT_INFO_H

#include <QMetaType>
#include <QString>
#include <QStringList>

// 用于存储显示在表格中的字体信息
struct FontInfo
{
    QString family;
    QString style;      // styles of this row, e.g. "Regular, Bold" or "Bold Italic"
    QStringList files;  // files that provide the same style set
    // Sorted styleValue list joined by ','; used to merge duplicate providers.
    QString styleSig;
    bool isSystem = true;
    bool enabled = true;
    bool selected = false;

    QString identity() const
    {
        return QString("%1|%2|%3").arg(family.toLower()).arg(isSystem ? 1 : 0).arg(styleSig);
    }

    QString primaryFile() const
    {
        return files.isEmpty() ? QString() : files.first();
    }
};

Q_DECLARE_METATYPE(FontInfo)

#endif  // FONT_INFO_H
