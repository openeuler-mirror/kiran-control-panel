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
#ifndef PRIVILEGED_HELPER_H
#define PRIVILEGED_HELPER_H

#include <QString>
#include <QStringList>

class PrivilegedHelper
{
public:
    static bool installSystemFonts(const QStringList &files, QString *error);
    static bool removeSystemFonts(const QStringList &files, QString *error);

private:
    static bool run(const QStringList &args, QString *error);
};

#endif  // PRIVILEGED_HELPER_H
