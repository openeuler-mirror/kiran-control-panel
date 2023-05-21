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
#include <kiran-authentication-service/kas-authentication-i.h>
#include <QSet>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QStackedWidget;
class QLabel;
class QComboBox;
QT_END_NAMESPACE

class ImageEnrollProgressBar;
class KiranAuthDBusProxy;
class GeneralBioPage;
class FingerPage : public QWidget
{
    Q_OBJECT
public:
    enum FingerAuthType
    {
        FINGER_TYPE_FINGER_PRINT,
        FINGER_TYPE_FINGER_VEIN
    };
    FingerPage(KiranAuthDBusProxy* proxy, FingerAuthType type, QWidget* parent = nullptr);
    ~FingerPage();

private:
    void initUI();
    QString autoGenerateFeatureName();

    QWidget* initFeatureManager();
    QWidget* initFeatureEnroll();

private slots:
    void onAddIdentificationClicked();
    void onEnrollCancelClicked();
    void onEnrollStatusNotify(const QString& iid, bool isComplete,
                              int progress, const QString& message);
    void onEnrollComplete(bool isSuccess, const QString& message, const QString& iid);

private:
    KADAuthType m_authType;

    KiranAuthDBusProxy* m_proxy;

    QStackedWidget* m_stackedWidget;
    GeneralBioPage* m_featureManager;
    ImageEnrollProgressBar* m_enrollProgress;
    QLabel* m_enRollTips;

    bool m_inEnroll;
    QString m_inErollFeatureName;
};
