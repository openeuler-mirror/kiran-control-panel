/**
 * Copyright (c) 2020 ~ 2024 KylinSec Co., Ltd.
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
#include <QString>

namespace Kiran
{
namespace Network
{
class Prefs
{
public:
    ~Prefs();
    static Prefs *instance();

    bool getCheckConnectivity() const;
    QString getConnectivityAddress() const;
    qint64 getConnectivityPort() const;
    bool getCheckWiredCarrier() const;

private:
    Prefs();
    bool m_checkConnectivity = true;
    QString m_connectivityAddress = "";
    qint64 m_connectivityPort = 0;
    bool m_checkWiredCarrier = false;
};
}  // namespace Network
}  // namespace Kiran