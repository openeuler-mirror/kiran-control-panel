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


#ifndef __LANGPACK_THREAD_H__
#define __LANGPACK_THREAD_H__

#include <QThread>
#include <Daemon>
#include <Transaction>

using namespace PackageKit;
class LangpackThread : public QThread
{
    Q_OBJECT

public:
    explicit LangpackThread(QObject* parent = nullptr, Transaction* transaction = nullptr);
    ~LangpackThread();
    void run();

signals:
    void showProcessDialogSignal(uint percentage);

private:
    QString m_langCode;
    Transaction* m_transaction = nullptr;
};

#endif