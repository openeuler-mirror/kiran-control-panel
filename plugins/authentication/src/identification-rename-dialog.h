/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
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
#pragma once
#include <kiranwidgets-qt5/kiran-titlebar-window.h>

class KiranAuthDBusProxy;
class QLineEdit;
class IdentificationRenameDialog : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    IdentificationRenameDialog(const QString& iid, KiranAuthDBusProxy* proxy, QWidget* parent = nullptr);
    ~IdentificationRenameDialog();

    int exec();

signals:
    void completed(QPrivateSignal);

private:
    virtual void closeEvent(QCloseEvent* event) override;
    Q_INVOKABLE void onConfirmClicked();

private:
    void initUI();

private:
    bool m_success;
    QLineEdit* m_edit;
    QString m_iid;
    KiranAuthDBusProxy* m_proxy;
};