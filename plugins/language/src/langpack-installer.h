/**
 * Copyright (c) 2022 ~ 2023 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
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


#ifndef __LANGPACK_INSTALLER_H__
#define __LANGPACK_INSTALLER_H__

#include <QWidget>
#include <Daemon>
#include "langpack-thread.h"

class QProgressDialog;
using namespace PackageKit;
class LangpackInstaller : public QWidget
{
    Q_OBJECT

public:
    explicit LangpackInstaller(QWidget* parent = nullptr);
    ~LangpackInstaller();

    void searchLangpack(const QString& langCode);
    void installLangpack(const QString& langCode);
    bool checkLangpackInstalled(const QString& langCode);

private slots:
    void showProcessDialog(uint percentage);
    
private:
    QProgressDialog* m_processDialog = nullptr;
    static Daemon* m_daemon;
    QString m_langpackName;
    uint m_langpackState;
    LangpackThread* m_langpackThread = nullptr;
};

#endif