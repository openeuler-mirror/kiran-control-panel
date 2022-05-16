/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-account is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#ifndef KIRAN_CPANEL_ACCOUNT_SRC_JSON_PARSER_H_
#define KIRAN_CPANEL_ACCOUNT_SRC_JSON_PARSER_H_

#include <QString>
#include <QDebug>

struct BiometricInfo
{
    QString name;
    QString dataID;
    bool operator==(const BiometricInfo what) const
    {
        return what.name == name &&
               what.dataID == dataID;
    }
    bool operator!=(const BiometricInfo what) const
    {
        return what.name != name ||
               what.dataID != dataID;
    }
    friend QDebug operator<<(QDebug argument, const BiometricInfo& authInfo)
    {
        QString desc = QString("(name:%1,dataID:%2)").arg(authInfo.name, authInfo.dataID);
        argument << desc;
        return argument;
    }
};
typedef QList<BiometricInfo> BiometricInfos;

namespace JsonParser
{
bool parserAuthItems(const QString& jsonDoc, BiometricInfos& biometricList);
};

#endif  //KIRAN_CPANEL_ACCOUNT_SRC_JSON_PARSER_H_
