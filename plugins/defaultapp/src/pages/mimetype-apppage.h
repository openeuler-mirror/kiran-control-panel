
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

#ifndef __MIMETYPE_APPPAGE_H__
#define __MIMETYPE_APPPAGE_H__

#include <QWidget>
#include "kiran-setting-item.h"
#include "libqtxdg.h"
#include "utils.h"

class AppManager;
class MimeTypeAppPage : public QWidget
{
    Q_OBJECT

public:
    explicit MimeTypeAppPage(EnumMimeType enumMimeType, QWidget* parent = nullptr);
    ~MimeTypeAppPage();
    void initUI();

private slots:
    void onClickedSlot();

private:
    AppManager* m_appManager;
    EnumMimeType m_enumMimeType;
    QMap<QString, XdgDesktopFilePtr> m_application;
    KiranSettingItem* m_beforeItem = nullptr;
};

#endif  // __MIMETYPE_APPPAGE_H__