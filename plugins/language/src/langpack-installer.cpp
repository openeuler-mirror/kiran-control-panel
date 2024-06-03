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

#include <QProgressDialog>
#include <Transaction>
#include <QEventLoop>
#include <QTimer>
#include <kiran-log/qt5-log-i.h>
#include "langpack-installer.h"

typedef enum
{
    PACKAGE_UNKNOW,
    PACKAGE_INSTALLED,
    PACKAGE_AVAILABLE
} PackageState;

Daemon* LangpackInstaller::m_daemon = nullptr;
LangpackInstaller::LangpackInstaller(QWidget* parent) : QWidget(parent), m_langpackName(QString()), m_langpackState(PACKAGE_UNKNOW)
{
    m_daemon = Daemon::global();
}

LangpackInstaller::~LangpackInstaller()
{

}

void LangpackInstaller::searchLangpack(const QString& langCode)
{
    QString searchLangpack = "langpacks-" + langCode;
    Transaction *transaction = m_daemon->searchNames(searchLangpack, Transaction::FilterNone);
    QEventLoop loop;
    connect(transaction, SIGNAL(finished(PackageKit::Transaction::Exit, uint)), &loop, SLOT(quit()));
    connect(transaction, &Transaction::errorCode,
            [this](uint errorcode, QString details) {
                KLOG_WARNING() << "errorcode:" << errorcode << ", details:" << details;
                return;
            });
    connect(transaction, &Transaction::package,
            [this](uint info, const QString &pid, const QString &summary) {
                const QByteArray &pkgInfo = pid.toLatin1();
                QString infoStr = QString("%1").arg(info);
                KLOG_INFO() << "pkgInfo:" << pkgInfo << ", infoStr:" << infoStr;
                m_langpackName = pkgInfo;
                m_langpackState = info;
            });
    loop.exec();
}

bool LangpackInstaller::checkLangpackInstalled(const QString& langCode)
{
    bool installed = false;
    searchLangpack(langCode);
    if (m_langpackState == PACKAGE_INSTALLED)
    {
        installed = true;
    }

    m_langpackName.clear();
    m_langpackState = PACKAGE_UNKNOW;

    return installed;
}


void LangpackInstaller::showProcessDialog(uint percentage)
{
    if (percentage == 100)
    {
        m_processDialog->setLabelText(tr("Install Complete!"));
    }
    m_processDialog->setMinimumDuration(0);
    m_processDialog->setValue(percentage);
}

void LangpackInstaller::installLangpack(const QString& langCode)
{
    searchLangpack(langCode);
    if (m_langpackState != PACKAGE_AVAILABLE)
    {
        KLOG_DEBUG() << langCode << " state is " << m_langpackState;
        return;
    }

    m_processDialog = new QProgressDialog(this->parentWidget());
    // dialog出现需等待的时间
    m_processDialog->setMinimumDuration(1000000000);
    m_processDialog->setValue(0);
    m_processDialog->setWindowTitle(tr("Package Install"));
    // 当进度条显示到 100%时 不隐藏
    m_processDialog->setAutoClose(0);
    // 当进度条显示到 100%时 不调用reset()函数重置进度条
    m_processDialog->setAutoReset(0);
    // 设置进度对话框标签文本，向用户提示该进度条正在做什么
    m_processDialog->setLabelText(tr("Installing..."));
    // 重新设置 Cancel按钮 的文本
    m_processDialog->setCancelButtonText(QString());
    m_processDialog->setRange(0, 100);

    Transaction *transaction = m_daemon->installPackage(m_langpackName, Transaction::TransactionFlagNone);
    m_langpackThread = new LangpackThread(this, transaction);
    connect(m_langpackThread, &LangpackThread::showProcessDialogSignal, this, &LangpackInstaller::showProcessDialog);
    m_langpackThread->start();
}