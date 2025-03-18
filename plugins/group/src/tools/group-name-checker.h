/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
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

#ifndef GROUP_NAME_CHECKER_H
#define GROUP_NAME_CHECKER_H

#include <QObject>

class GroupNameChecker : public QObject
{
    Q_OBJECT
public:
    /// @brief 检测用户组名是否合规
    /// @param name 用户组名
    /// @param errorMessage 不合规则返回错误信息
    /// @return 是否合规
    static bool isValid(const QString& name, QString& errorMessage);
};

#endif  // GROUP_NAME_CHECKER_H