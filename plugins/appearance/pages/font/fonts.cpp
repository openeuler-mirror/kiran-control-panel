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
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#include "fonts.h"
#include "appearance-global-info.h"
#include "ui_fonts.h"

#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <kiran-session-daemon/appearance-i.h>
#include <QDebug>
#include <QFontDatabase>

using namespace std;

QStringList Fonts::fontSizes = {"7", "8", "9", "10", "11", "12", "13", "14"};

Fonts::Fonts(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::Fonts)
{
    ui->setupUi(this);
    initUI();
}

Fonts::~Fonts()
{
    delete ui;
}

bool Fonts::initUI()
{
    // 统一QComboBox样式，并初始化可选值列表
    QList<QComboBox*> comboBoxList = this->findChildren<QComboBox*>();
    foreach (QComboBox* comboBox, comboBoxList)
    {
        comboBox->setStyleSheet("QComboBox {combobox-popup: 0;}");
    }

    m_fontTypeComboBoxMap = {
        {APPEARANCE_FONT_TYPE_APPLICATION, {ui->cbox_application_font_name, ui->cbox_application_font_size}},
        {APPEARANCE_FONT_TYPE_WINDOW_TITLE, {ui->cbox_titlebar_font_name, ui->cbox_titlebar_font_size}},
        {APPEARANCE_FONT_TYPE_MONOSPACE, {ui->cbox_monospace_font_name, ui->cbox_monospace_font_size}}};

    QFontDatabase database;
    auto fontFamilys = database.families();
    for (auto fontComboBoxs : m_fontTypeComboBoxMap.values())
    {
        fontComboBoxs.first->addItems(fontFamilys);
        fontComboBoxs.second->addItems(fontSizes);
    }

    for (auto fontType : m_fontTypeComboBoxMap.keys())
    {
        updateUIFontInfo(fontType);
    }

    initConnections();

    return true;
}

void Fonts::updateUIFontInfo(int fontType)
{
    QStringList fontInfoList;
    auto appearanceInterface = AppearanceGlobalInfo::instance();

    QString fontName;
    int fontSize;
    if (!appearanceInterface->getFont(fontType, fontName, fontSize))
    {
        KLOG_WARNING(qLcAppearance) << "update ui font info failed, type:" << fontType;
        return;
    }

    KLOG_DEBUG(qLcAppearance) << "update ui font info,type:" << fontType
                              << ",name:" << fontName
                              << ",size:" << fontSize;

    setUIFontInfo(fontType, fontName, fontSize);
}

void Fonts::setUIFontInfo(int fontType, const QString& name, const int size)
{
    auto fontComboBoxs = m_fontTypeComboBoxMap.find(fontType);
    if (fontComboBoxs == m_fontTypeComboBoxMap.end())
    {
        KLOG_ERROR(qLcAppearance) << "set ui font info failed,can't find font type:" << fontType;
        return;
    }

    auto fontNameComboBox = fontComboBoxs.value().first;
    auto fontSizeComboBox = fontComboBoxs.value().second;

    int idx = fontNameComboBox->findText(name);
    if (idx == -1)
    {
        KLOG_ERROR(qLcAppearance) << "can't find font name action" << name << "int combobox";
    }
    else
    {
        fontNameComboBox->setCurrentIndex(idx);
    }

    idx = fontSizeComboBox->findText(QString::number(size));
    if (idx == -1)
    {
        KLOG_ERROR(qLcAppearance) << "can't find font size action:" << idx << "in combobox";
    }
    else
    {
        fontSizeComboBox->setCurrentIndex(idx);
    }
}

void Fonts::initConnections()
{
    connect(AppearanceGlobalInfo::instance(), &AppearanceGlobalInfo::fontChanged, this, &Fonts::handleFontChanged);

    for (auto fontTypeCombos : m_fontTypeComboBoxMap.values())
    {
        auto fontNameComboBox = fontTypeCombos.first;
        auto fontSizeComboBox = fontTypeCombos.second;

        connect(fontNameComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &Fonts::onComboBoxIdxChanged);
        connect(fontSizeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &Fonts::onComboBoxIdxChanged);
    }
}

void Fonts::onComboBoxIdxChanged(int idx)
{
    auto senderComboBox = qobject_cast<QComboBox*>(sender());

    int fontType = -1;
    QString fontName;
    int fontSize = 0;
    for (auto iter = m_fontTypeComboBoxMap.begin();
         iter != m_fontTypeComboBoxMap.end();
         iter++)
    {
        auto pair = iter.value();
        if (senderComboBox != pair.first && senderComboBox != pair.second)
        {
            continue;
        }

        fontType = iter.key();
        fontName = pair.first->currentText();
        fontSize = pair.second->currentText().toInt();
    }

    if (fontType == -1)
    {
        KLOG_ERROR(qLcAppearance) << "current font combobox idx changed,can't find font type!";
        return;
    }

    QString fontInfo = QString("%1 %2").arg(fontName).arg(fontSize);
    if (!AppearanceGlobalInfo::instance()->setFont(fontType, fontInfo))
    {
        KLOG_ERROR(qLcAppearance) << "combobox idx changed,set font" << fontType << fontInfo << "failed!";
        KiranMessageBox::message(nullptr, QObject::tr("Failed"),
                                 QObject::tr("Set font failed!"),
                                 KiranMessageBox::Ok);
    }

    KLOG_INFO(qLcAppearance) << "ui font settings changed,set font" << fontType << fontInfo;
}

void Fonts::handleFontChanged(int type, QString fontInfo)
{
    updateUIFontInfo(type);
}

QSize Fonts::sizeHint() const
{
    return {500, 657};
}
