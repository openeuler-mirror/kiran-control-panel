/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
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
#include <QComboBox>

namespace Kiran
{
namespace Network
{
// 跳过QComboBox的wheelEvent处理，避免ScrollArea中ComboBox接收滚轮事件获取焦点
class DisableWheelComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit DisableWheelComboBox(QWidget* parent = nullptr);
    ~DisableWheelComboBox();

protected:
    void wheelEvent(QWheelEvent* event) override;
};
}  // namespace Network
}  // namespace Kiran