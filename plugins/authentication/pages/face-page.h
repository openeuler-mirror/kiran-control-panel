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
#include <QSet>
#include <QWidget>

class KiranAuthDBusProxy;
class GeneralBioPage;
class ImageEnrollProgressBar;
class QLabel;
class QStackedWidget;
class FacePage : public QWidget
{
    Q_OBJECT
public:
    FacePage(KiranAuthDBusProxy* proxy, QWidget* parent = nullptr);
    ~FacePage();

private:
    void initUI();
    QWidget* initFeatureManagerPage();
    QWidget* initFeatureEnrollPage();

private slots:
    void updateEnrollStatus(const QString& iid, bool isComplete,
                              int progress, const QString& message);
    void startEnrollFeature();
    void enrollCancel();
    void finishEnroll(bool isSuccess, const QString& message, const QString& iid);

private:
    KiranAuthDBusProxy* m_proxy;
    QSet<QString> m_featureNameSet;
    QStackedWidget* m_stackedWidget;
    GeneralBioPage* m_featureManager;
    ImageEnrollProgressBar* m_enrollProgress;
    QLabel* m_enrollTips;

    bool m_inEnroll = false;
    QString m_inErollFeatureName;
};
