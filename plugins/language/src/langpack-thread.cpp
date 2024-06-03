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

#include "langpack-thread.h"
#include <qt5-log-i.h>

typedef enum
{
    PACKAGE_UNKNOW,
    PACKAGE_INSTALLED,
    PACKAGE_AVAILABLE
} PackageState;

LangpackThread::LangpackThread(QObject* parent,  Transaction* transaction) : QThread(parent), m_transaction(transaction)
{

}

LangpackThread::~LangpackThread()
{
}


void LangpackThread::run()
{
    connect(m_transaction, &Transaction::errorCode,
            [this](uint errorcode, QString details) {
                KLOG_WARNING() << "errorcode:" << errorcode << ", details:" << details;
                return;
            });
    connect(m_transaction, &Transaction::package,
            [this](uint info, const QString &pid, const QString &summary) {
                uint percentage = m_transaction->percentage();
                KLOG_INFO() << "info:" << info << ", pid:" << pid << ", summary:" << summary << ", percentage:" << percentage;
                emit showProcessDialogSignal(percentage);
            });
    connect(m_transaction, &Transaction::finished,
            [this](uint status, uint runtime) {
                if (status == PACKAGE_INSTALLED)
                {
                    uint percentage = m_transaction->percentage();
                    KLOG_INFO() << "percentage:" << percentage;
                    emit showProcessDialogSignal(percentage);
                    this->quit();
                }
            });
}
