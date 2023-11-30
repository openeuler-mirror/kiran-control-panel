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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include "mask-widget.h"

#include <QEvent>
#include <QResizeEvent>

MaskWidget::MaskWidget(QWidget *parent)
    : QWidget(parent),
      m_maskedWidget(parent)
{
    m_maskedWidget->installEventFilter(this);
}

bool MaskWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_maskedWidget)
    {
        switch (event->type())
        {
        case QEvent::Resize:
        {
            auto resizeEvent = dynamic_cast<QResizeEvent *>(event);
            //TODO:后续添加功能,是否不遮住自定义标题栏
            resize(resizeEvent->size());
            break;
        }
        default:
            break;
        }
    }
    return QWidget::eventFilter(watched, event);
}
