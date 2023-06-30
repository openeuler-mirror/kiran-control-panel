/**
 * Copyright (c) 2023 ~ 2024 KylinSec Co., Ltd.
 * ks-sc is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */

#ifndef DEFAULTAPPFRAME_H
#define DEFAULTAPPFRAME_H

#include <QFileInfo>
#include <QWidget>
#include "libqtxdg.h"
#include "utils.h"

class DefaultAppFrame
{
public:
    DefaultAppFrame();
    ~DefaultAppFrame();

    void getAppsForMimeType();
    QVector<XdgDesktopFile*> getApplications(EnumMimeType enumMimeType);

    void sortApplication(EnumMimeType& enumMimeType, QVector<XdgDesktopFile*>& mimeInfoCache, XdgDesktopFile* defaultApp);

    void slotTextChanged(EnumMimeType enumMimeType, XdgDesktopFile* desktopFile);

private:
    QMap<EnumMimeType, QVector<XdgDesktopFile*>> appInfos;
};

#endif  // DEFAULTAPPFRAME_H
