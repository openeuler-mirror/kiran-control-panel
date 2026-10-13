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
 * Author:     gaobo <gaobo@kylinsec.com.cn>
 */
#ifndef USERLOADER_H
#define USERLOADER_H

#include <QAtomicInt>
#include <QObject>
#include <QStringList>
#include <QVariantMap>

/// @brief 在后台线程逐个加载用户，每加载完一个用户发一次 userLoaded
///        这里不创建用户代理：每个代理都会各自占一条总线订阅，用户数上千时
///        会撞上 dbus-daemon 的 match rule 上限，超出的静默失效
class UserLoader : public QObject
{
    Q_OBJECT
public:
    UserLoader();

    /// @brief 读取单个用户的展示值，内部是一次 Properties.GetAll,会阻塞调用线程
    static QVariantMap fetchUserValues(const QString &objectPath);

    /// @brief 请求停止，正在进行的批量加载会在下一个用户之前结束
    void requestStop();

public slots:
    /// @brief 在加载器所在线程执行：逐个加载这批用户的展示值
    void load(const QStringList &objectPaths);

signals:
    /// @brief 一个用户加载完成
    void userLoaded(const QString &objectPath, const QVariantMap &values);

private:
    QAtomicInt m_stopRequested{0};
};

#endif  // USERLOADER_H
