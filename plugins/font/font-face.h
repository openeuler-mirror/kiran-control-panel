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
#ifndef FONT_FACE_H
#define FONT_FACE_H

#include <QString>
#include <QtGlobal>

// 用于存储所有字体文件中的每个face信息
struct FontFace
{
    QString family;
    QString style;
    QString file;
    // (weight<<16)|(width<<8)|slant，用于表示字体样式
    quint32 styleValue = 0;
    int faceIndex = 0;
    bool isSystem = true;
    bool enabled = true;

    QString faceKey() const
    {
        return QString("%1|%2").arg(file).arg(faceIndex);
    }

    QString familyStyleKey() const
    {
        return QString("%1|%2").arg(family.toLower()).arg(styleValue);
    }
};

#endif  // FONT_FACE_H
