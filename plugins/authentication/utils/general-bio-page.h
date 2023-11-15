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

#include <QWidget>
#include <kiran-authentication-service/kas-authentication-i.h>
#include <QSet>

class QLabel;
class QComboBox;
class AuthSettingContainer;
class KiranAuthDBusProxy;
class GeneralBioPage:public QWidget
{
    Q_OBJECT
public:
    GeneralBioPage(KiranAuthDBusProxy* proxy,KADAuthType authType,QWidget* parent = nullptr);
    ~GeneralBioPage();

    void setFeatureNamePrefix(const QString& prefix);
    void setDefaultDeviceLabelDesc(const QString& text);
    void setDeviceFeatureListDesc(const QString& text);

    QString autoGenerateFeatureName();

    void refreshFeature();
    void refreshDeviceComboBox();
    int getDeviceCount();

signals:
    void enrollFeature();

private slots:
    void updateDefaultDevice(int defaultDeviceIdx);
    void renameFeature();
    void removeFeature();
    void startEnrollFeature();

private:
    void initUI();
    void addFeature(const QString& featureName,const QString& featureIID);
    int addDeviceItem(const QString& deviceName,const QString& deviceID);

private:
    KiranAuthDBusProxy* m_proxy;
    KADAuthType m_authType;
    QLabel* m_labelDefaultDevice;
    QComboBox* m_comboDefaultDevice;
    QLabel* m_labelFeatureList;
    AuthSettingContainer* m_featureContainer;
    QSet<QString> m_featureNameSet;
    QString m_featureNamePrefix;
};