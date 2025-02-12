/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     youzhengcai <youzhengcai@kylinsec.com.cn>
 */

#pragma once

#include <QPixmap>

#define RIGHT_ARROW "ksvg-arrow"

class FlagPixmap
{
public:
    // 默认展开图标
    static QPixmap expansionFlagPixmap();
    static QPixmap expansionFlagPixmapDark();
    // 默认折叠图标
    static QPixmap collapseFlagPixmap();
    static QPixmap collapseFlagPixmapDark();

private:
    static QImage rotateImage(const QImage& image, qreal angle);
    static void invertColors(QImage& image);
};
