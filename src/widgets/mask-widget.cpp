 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
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
    return QObject::eventFilter(watched, event);
}
