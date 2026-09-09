/**
 * Copyright (c) 2020 ~ 2026 KylinSec Co., Ltd.
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

#ifndef FCITX4_IM_SUBITEM_H
#define FCITX4_IM_SUBITEM_H

#include <QObject>

#include "fcitx4/fcitx4-im-page.h"
#include "im-candidate-filter.h"
#include "panel-interface.h"
#include "plugin-subitem-interface.h"

class Fcitx4ImSubItem : public QObject,
                        public KiranControlPanel::PluginSubitemInterface
{
    Q_OBJECT
public:
    Fcitx4ImSubItem(const ImCandidateFilter& filter, QObject* parent = nullptr)
        : QObject(parent),
          m_candidateFilter(filter)
    {
    }
    ~Fcitx4ImSubItem()
    {
    }

public:
    QString getID() override
    {
        return "InputMethod";
    }

    QString getName() override
    {
        return tr("Input Method");
    }

    QString getCategory() override
    {
        return "hardware";
    }

    QString getDesc() override
    {
        return "";
    }

    QString getIcon() override
    {
        return "ksvg-kcp-input-method";
    }

    int getWeight() override
    {
        // 介于键盘布局(0)与鼠标(1)之间；同权重组内按插件加载顺序排列
        return 1;
    }

    QWidget* createWidget() override
    {
        return new Fcitx4ImPage(m_candidateFilter);
    }

    QVector<QPair<QString, QString>> getSearchKeys() override
    {
        return {};
    }

    bool jumpToSearchEntry(const QString& key) override
    {
        Q_UNUSED(key);
        return false;
    }

    bool haveUnSavedOptions() override
    {
        return false;
    }

private:
    ImCandidateFilter m_candidateFilter;
};

#endif
