/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "fonts.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <kiran-session-daemon/appearance-i.h>
#include <QDebug>
#include <QFontDatabase>
#include "dbus-interface/appearance-global-info.h"
#include "ui_fonts.h"

using namespace std;
Fonts::Fonts(QWidget* parent) : QWidget(parent),
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
    //统一QComboBox样式，并初始化可选值列表
    QList<QComboBox*> comboBoxList = this->findChildren<QComboBox*>();
    foreach (QComboBox* comboBox, comboBoxList)
    {
        comboBox->setStyleSheet("QComboBox {combobox-popup: 0;}");
    }

    QStringList sizes;
    sizes << "7"
          << "8"
          << "9"
          << "10"
          << "11"
          << "12"
          << "13"
          << "14";

    ui->cbox_application_font_size->addItems(sizes);
    ui->cbox_monospace_font_size->addItems(sizes);
    ui->cbox_titlebar_font_size->addItems(sizes);

    QFontDatabase database;
    m_appFamilies = database.families();
    m_windowTitleFamilies = m_appFamilies;
    m_monospaceFamilies = m_appFamilies;

    foreach (const QString& family, m_appFamilies)
    {
        ui->cbox_application_font_name->addItem(family);
    }
    foreach (const QString& family, m_windowTitleFamilies)
    {
        ui->cbox_titlebar_font_name->addItem(family);
    }
    foreach (const QString& family, m_monospaceFamilies)
    {
        ui->cbox_monospace_font_name->addItem(family);
    }

    // 获取各个类型的字体值
    getCurrentFontInfo(APPEARANCE_FONT_TYPE_APPLICATION);
    getCurrentFontInfo(APPEARANCE_FONT_TYPE_WINDOW_TITLE);
    getCurrentFontInfo(APPEARANCE_FONT_TYPE_MONOSPACE);

    connectSignals();

    return true;
}

/**
 * @brief Fonts::getCurrentFontInfo 获取当前字体信息，并设置在界面中
 * @param fontType 字体类型
 */
void Fonts::getCurrentFontInfo(int fontType)
{
    QStringList fontInfoList;
    switch (fontType)
    {
    case APPEARANCE_FONT_TYPE_APPLICATION:
        fontInfoList = AppearanceGlobalInfo::instance()->getFont(APPEARANCE_FONT_TYPE_APPLICATION);
        if (!fontInfoList.isEmpty())
        {
            m_applicationFontInfo = fontInfoList;

            ui->cbox_application_font_name->blockSignals(true);
            ui->cbox_application_font_size->blockSignals(true);
            foreach (QString fontFamily, m_appFamilies)
            {
                //if (!QString::compare(fontInfoList.at(0), fontFamily))
                if (fontFamily == fontInfoList.at(0))
                {
                    KLOG_INFO() << fontFamily << "==" << fontInfoList.at(0);
                    ui->cbox_application_font_name->setCurrentText(fontInfoList.at(0));
                    break;
                }
            }
            if (ui->cbox_application_font_name->currentText() != fontInfoList.at(0))
            {
                ui->cbox_application_font_name->insertItem(ui->cbox_application_font_name->count(), fontInfoList.at(0));
                ui->cbox_application_font_name->setCurrentText(fontInfoList.at(0));
            }

            ui->cbox_application_font_size->setCurrentText(fontInfoList.at(1));
            ui->cbox_application_font_name->blockSignals(false);
            ui->cbox_application_font_size->blockSignals(false);
        }
        break;
    case APPEARANCE_FONT_TYPE_WINDOW_TITLE:
        fontInfoList = AppearanceGlobalInfo::instance()->getFont(APPEARANCE_FONT_TYPE_WINDOW_TITLE);
        if (!fontInfoList.isEmpty())
        {
            m_windowTitleFontInfo = fontInfoList;

            ui->cbox_titlebar_font_name->blockSignals(true);
            ui->cbox_titlebar_font_size->blockSignals(true);
            foreach (QString fontFamily, m_windowTitleFamilies)
            {
                if (fontFamily == fontInfoList.at(0))
                {
                    ui->cbox_titlebar_font_name->setCurrentText(fontInfoList.at(0));
                    break;
                }
            }
            if (ui->cbox_titlebar_font_name->currentText() != fontInfoList.at(0))
            {
                ui->cbox_titlebar_font_name->insertItem(-1, fontInfoList.at(0));
                ui->cbox_titlebar_font_name->setCurrentText(fontInfoList.at(0));
            }
            ui->cbox_titlebar_font_size->setCurrentText(fontInfoList.at(1));
            ui->cbox_titlebar_font_name->blockSignals(false);
            ui->cbox_titlebar_font_size->blockSignals(false);
        }
        break;
    case APPEARANCE_FONT_TYPE_MONOSPACE:
        fontInfoList = AppearanceGlobalInfo::instance()->getFont(APPEARANCE_FONT_TYPE_MONOSPACE);
        if (!fontInfoList.isEmpty())
        {
            m_monospaceFontInfo = fontInfoList;
            ui->cbox_monospace_font_name->blockSignals(true);
            ui->cbox_monospace_font_size->blockSignals(true);
            foreach (QString fontFamily, m_monospaceFamilies)
            {
                if (fontFamily == fontInfoList.at(0))
                {
                    ui->cbox_monospace_font_name->setCurrentText(fontInfoList.at(0));
                    break;
                }
            }
            if (ui->cbox_monospace_font_name->currentText() != fontInfoList.at(0))
            {
                ui->cbox_monospace_font_name->insertItem(-1, fontInfoList.at(0));
                ui->cbox_monospace_font_name->setCurrentText(fontInfoList.at(0));
            }
            ui->cbox_monospace_font_size->setCurrentText(fontInfoList.at(1));
            ui->cbox_monospace_font_name->blockSignals(false);
            ui->cbox_monospace_font_size->blockSignals(false);
        }
        break;
    default:
        break;
    }
}

void Fonts::setFont(int fontType, QStringList fontInfoList)
{
    if (!AppearanceGlobalInfo::instance()->setFont(fontType, fontInfoList))
    {
        KiranMessageBox::message(nullptr, QObject::tr("Failed"),
                                 QObject::tr("Set font  failed!"),
                                 KiranMessageBox::Ok);
    }
}

void Fonts::connectSignals()
{
    connect(AppearanceGlobalInfo::instance(), &AppearanceGlobalInfo::fontChanged, this, &Fonts::handleFontChanged);
    connect(ui->cbox_application_font_name, &QComboBox::currentTextChanged, [=](QString text) {
        m_applicationFontInfo.replace(0, text);
        KLOG_INFO() << "select applicationFont name = " << m_applicationFontInfo.at(0);
        KLOG_INFO() << "select applicationFont size = " << m_applicationFontInfo.at(1);
        setFont(APPEARANCE_FONT_TYPE_APPLICATION, m_applicationFontInfo);
    });
    connect(ui->cbox_application_font_size, &QComboBox::currentTextChanged, [=](QString text) {
        m_applicationFontInfo.replace(1, text);
        KLOG_INFO() << "select applicationFont name = " << m_applicationFontInfo.at(0);
        KLOG_INFO() << "select applicationFont size = " << m_applicationFontInfo.at(1);
        setFont(APPEARANCE_FONT_TYPE_APPLICATION, m_applicationFontInfo);
    });
    connect(ui->cbox_monospace_font_name, &QComboBox::currentTextChanged, [=](QString text) {
        m_monospaceFontInfo.replace(0, text);
        KLOG_INFO() << "monospaceFontInfo name = " << m_monospaceFontInfo.at(0);
        KLOG_INFO() << "monospaceFontInfo size = " << m_monospaceFontInfo.at(1);
        setFont(APPEARANCE_FONT_TYPE_MONOSPACE, m_monospaceFontInfo);
    });
    connect(ui->cbox_monospace_font_size, &QComboBox::currentTextChanged, [=](QString text) {
        m_monospaceFontInfo.replace(1, text);
        KLOG_INFO() << "monospaceFontInfo name = " << m_monospaceFontInfo.at(0);
        KLOG_INFO() << "monospaceFontInfo size = " << m_monospaceFontInfo.at(1);
        setFont(APPEARANCE_FONT_TYPE_MONOSPACE, m_monospaceFontInfo);
    });
    connect(ui->cbox_titlebar_font_name, &QComboBox::currentTextChanged, [=](QString text) {
        m_windowTitleFontInfo.replace(0, text);
        KLOG_INFO() << "windowTitleFontInfo name = " << m_windowTitleFontInfo.at(0);
        KLOG_INFO() << "windowTitleFontInfo size = " << m_windowTitleFontInfo.at(1);
        setFont(APPEARANCE_FONT_TYPE_WINDOW_TITLE, m_windowTitleFontInfo);
    });
    connect(ui->cbox_titlebar_font_size, &QComboBox::currentTextChanged, [=](QString text) {
        m_windowTitleFontInfo.replace(1, text);
        KLOG_INFO() << "windowTitleFont name = " << m_windowTitleFontInfo.at(0);
        KLOG_INFO() << "windowTitleFont size = " << m_windowTitleFontInfo.at(1);
        setFont(APPEARANCE_FONT_TYPE_WINDOW_TITLE, m_windowTitleFontInfo);
    });
}

void Fonts::handleFontChanged(int type, QString fontInfo)
{
    QStringList fontInfoList = fontInfo.split(" ", QString::SkipEmptyParts);
    QString fontSize = fontInfoList.takeLast();
    QString fontName = fontInfoList.join(" ");
    KLOG_INFO() << "font changed : " << type << ",name: " << fontName << ",size: " << fontSize;

    switch (type)
    {
    case APPEARANCE_FONT_TYPE_APPLICATION:
        //        if (ui->cbox_application_font_name->currentText() != fontName)
        //        {
        //            foreach (QString fontFamily, m_appFamilies)
        //            {
        //                if (fontFamily == fontName)
        //                {
        //                    ui->cbox_application_font_name->setCurrentText(fontInfoList.at(0));
        //                    break;
        //                }
        //            }
        //            if (ui->cbox_application_font_name->currentText() != fontInfoList.at(0))
        //            {
        //                ui->cbox_application_font_name->removeItem(ui->cbox_application_font_name->count() - 1);
        //                ui->cbox_application_font_name->addItem(-1, fontInfoList.at(0));
        //                ui->cbox_application_font_name->setCurrentText(fontInfoList.at(0));
        //            }
        //        }
        break;
    case APPEARANCE_FONT_TYPE_WINDOW_TITLE:
        break;
    case APPEARANCE_FONT_TYPE_MONOSPACE:
        break;
    default:
        break;
    }
}
