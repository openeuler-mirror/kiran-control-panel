/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
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

#include "json-parser.h"

#include <qt5-log-i.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSharedPointer>

bool JsonParser::parserAuthItems(const QString& jsonDoc, BiometricInfos& biometricList)
{
    auto errorPtr = new QSharedPointer<QJsonParseError>(new QJsonParseError);
    QJsonDocument doc = QJsonDocument::fromJson(jsonDoc.toUtf8(), errorPtr->data());
    if (errorPtr->data()->error != QJsonParseError::NoError)
    {
        return false;
    }
    if (!doc.isArray())
    {
        return false;
    }
    biometricList.clear();
    if (doc.isEmpty())
    {
        return true;
    }
    QJsonArray array = doc.array();
    for (auto iter : array)
    {
        QJsonObject obj = iter.toObject();
        if (!obj.contains("data_id") || !obj.contains("name"))
        {
            KLOG_WARNING() << "format error,leak data_id/name element.";
            continue;
        }

        BiometricInfo authInfo{.name = obj.value("name").toString(),
                               .dataID = obj.value("data_id").toString()};

        biometricList.append(authInfo);
    }
    return true;
}
