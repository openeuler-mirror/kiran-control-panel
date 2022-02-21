/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
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
#include "active-guide-widget.h"
#include <kiran-log/qt5-log-i.h>
#include <QDesktopWidget>
#include <QJsonDocument>
#include <QThread>
#include <QTime>
#include <QToolButton>
#include <QWidgetAction>
#include <iostream>
#include "../dbus-wrapper/license-dbus.h"
#include "ui_active-guide-widget.h"

using namespace std;

#define ALMIGHTY_AUTHORIZED_POINTS "almighty_authorized_points"
#define ALMIGHTY_REMAINING_POINTS "almighty_remaining_points"
#define DESKTOP_AUTHORIZED_POINTS "desktop_authorized_points"
#define DESKTOP_REMAINING_POINTS "desktop_remaining_points"
#define DEVELOPMENT_AUTHORIZED_POINTS "development_authorized_points"
#define DEVELOPMENT_REMAINING_POINTS "development_remaining_points"
#define INDUSTRIAL_AUTHORIZED_POINTS "industrial_authorized_points"
#define INDUSTRIAL_REMAINING_POINTS "industrial_remaining_points"
#define SERVER_AUTHORIZED_POINTS "server_authorized_points"
#define SERVER_REMAINING_POINTS "server_remaining_points"

#ifdef DISABLE_KIRANWIDGETS
ActGuideWidget::ActGuideWidget(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::ActGuideWidget),
                                                  showQRCode(nullptr)
{
    ui->setupUi(this);
    setWindowTitle(tr("Activation Guide"));
    setWindowIcon(QIcon(":/images/kylin-about.png"));
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
#else
ActGuideWidget::ActGuideWidget(QWidget *parent) : KiranTitlebarWindow(parent),
                                                  ui(new Ui::ActGuideWidget),
                                                  showQRCode(nullptr)
{
    ui->setupUi(getWindowContentWidget());
    setButtonHints(TitlebarMinimizeButtonHint | TitlebarCloseButtonHint);
    setTitle(tr("Activation Guide"));
    setIcon(QIcon(":/images/kylin-about.png"));
    setResizeable(false);
#endif
    iniUI();
    if (!getMachineCode())
    {
        ui->label_mc_key->setText(tr("unknow"));
        ui->label_mc_manual->setText(tr("unknow"));
        ui->label_mc_online->setText(tr("unknow"));
    }
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    this->move(screenGeometry.x() + (screenGeometry.width() - this->width()) / 2,
               screenGeometry.y() + (screenGeometry.height() - this->height()) / 2);

    listItemMode = createActiveItem(tr("Activation Mode"));
    listItemActive = createActiveItem(tr("Start Activation"));
    listItemComplete = createActiveItem(tr("Activation Complete"));

    ui->listWidget->setCurrentRow(0);

    loadAnimation = new LoadAnimationWidget(this);
    ui->animation_layout->addWidget(loadAnimation);

    ui->btn_qrcode_manual->installEventFilter(this);
    ui->btn_qrcode_key->installEventFilter(this);
    ui->btn_qrcode_online->installEventFilter(this);

    connect(ui->btn_next, SIGNAL(clicked()), this, SLOT(handleNextButton()));

    connect(ui->btn_active_key, SIGNAL(clicked()), this, SLOT(handleActiveButton()));
    connect(ui->btn_active_manual, SIGNAL(clicked()), this, SLOT(handleActiveButton()));
    connect(ui->btn_active_online, SIGNAL(clicked()), this, SLOT(handleActiveButton()));

    connect(ui->btn_back_key, SIGNAL(clicked()), this, SLOT(handleBackButton()));
    connect(ui->btn_back_manual, SIGNAL(clicked()), this, SLOT(handleBackButton()));
    connect(ui->btn_back_online, SIGNAL(clicked()), this, SLOT(handleBackButton()));
    connect(ui->btn_back_failed, SIGNAL(clicked()), this, SLOT(handleBackButton()));

    connect(ui->btn_close_failde, SIGNAL(clicked()), this, SLOT(handleCloseButton()));
    connect(ui->btn_close_success, SIGNAL(clicked()), this, SLOT(handleCloseButton()));

    connect(this, SIGNAL(systemIsActived(bool)), this, SLOT(getLicenseStatus(bool)));
    connect(this, SIGNAL(sigShowqrcodewgt(QPoint)), this, SLOT(popupQRCode(QPoint)));

    connect(ui->lineEdit_lc, SIGNAL(textChanged(QString)), SLOT(setManualLineEditStatus()));
    connect(ui->lineEdit_address, SIGNAL(textChanged(QString)), SLOT(setKeyLineEditStatus()));
}

void ActGuideWidget::iniUI()
{
    setWindowModality(Qt::ApplicationModal);
    hideLabelPointsDefault();

    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setEnabled(false);

    ui->rbtn_manual->setChecked(true);

    ui->btn_active_manual->setEnabled(false);
    ui->btn_active_online->setEnabled(false);

    ui->lineEdit_lc->setFocus();
    ui->lineEdit_address->setPlaceholderText(tr("Server IP address or Domain name"));

    ui->label_failed->setWordWrap(true);
}

/**
 * @brief 设置授权点数是否可见
 * @param isVisible:授权点数是否可见
 * @param almightyPointsVisible:通用授权点数是否可见
 */

void ActGuideWidget::setLabelPointsVisible(bool almightyPointsVisible)
{
    if (almightyPointsVisible)
    {
        ui->label_authorized_points->show();
        ui->label_remaining_points->show();
        ui->authorized_points->show();
        ui->remaining_points->show();
    }
    else
    {
        ui->label_desktop_points->show();
        ui->label_development_points->show();
        ui->label_industrial_points->show();
        ui->label_server_points->show();
        ui->desktop_points->show();
        ui->development_points->show();
        ui->industrial_points->show();
        ui->server_points->show();
    }
}

/**
 * @brief  显示二维码的回调函数
 * @param  oPoint  二维码显示的位置
 */
void ActGuideWidget::popupQRCode(QPoint oPoint)
{
    if (showQRCode == nullptr)
    {
        showQRCode = new ShowQRCode;
    }
    showQRCode->setWindowFlag(Qt::X11BypassWindowManagerHint);
    showQRCode->setQRCode(mc_code, true);
    showQRCode->move(oPoint);
    showQRCode->raise();
    showQRCode->show();
}

/**
 * @brief  事件监听，事件监听，监听鼠标进入或离开二维码按钮事件 ，
 *         进入按钮则发出弹出二维码信号，离开则隐藏二维码窗口，并释放二维码窗口内存
 * @param  obj  事件对象
 * @param  obj  事件
 * @return 是否过滤
 */
bool ActGuideWidget::eventFilter(QObject *target, QEvent *e)
{
    if (target == ui->btn_qrcode_manual || target == ui->btn_qrcode_key || target == ui->btn_qrcode_online)
    {
        if (e->type() == QEvent::Enter)
        {
            QPoint oPoint = this->mapToGlobal(QPoint(610, 70));
            emit sigShowqrcodewgt(oPoint);
        }
        else if (e->type() == QEvent::Leave)
        {
            showQRCode->hide();
            delete showQRCode;
            showQRCode = nullptr;
        }
    }
    return false;
}

/**
 * @brief  创建自定义的列表项
 * @param  text 列表项的内容
 * @return ActListItem 返回设计好的激活列表项
 */
ActListItem *ActGuideWidget::createActiveItem(const QString &text)
{
    QListWidgetItem *newItem = nullptr;
    ActListItem *customItem = nullptr;

    newItem = new QListWidgetItem(ui->listWidget);
    customItem = new ActListItem(ui->listWidget);

    newItem->setSizeHint(QSize(165, 60));
    newItem->setTextAlignment(Qt::AlignVCenter);

    customItem->setItemText(text);
    ui->listWidget->addItem(newItem);
    ui->listWidget->setItemWidget(newItem, customItem);

    return customItem;
}

bool ActGuideWidget::getMachineCode()
{
    //get machine code
    QString machineCode;
    if (!InfoDbus::KylinLicense::getMachineCode(machineCode))
    {
        KLOG_DEBUG() << "get Machine Code failed";
        return false;
    }
    KLOG_INFO() << "machineCode" << machineCode;
    mc_code = machineCode;
    ui->label_mc_key->setText(mc_code);
    ui->label_mc_manual->setText(mc_code);
    ui->label_mc_online->setText(mc_code);
    return true;
}

/**
 * @brief  槽函数 ,用来处理用户点击激活按钮后触发的是否激活成功信号
 * @param  isRegister 是否激活成功
 */
void ActGuideWidget::getLicenseStatus(bool isRegister)
{
    if (isRegister)
    {
        KLOG_INFO() << "get activation status: is register";
        ui->stackedWidget->setCurrentIndex(Page_Successful);
        loadAnimation->stopAnimation();
    }
    else
    {
        KLOG_INFO() << "get activation status: not register";
        ui->stackedWidget->setCurrentIndex(Page_Failed);
        loadAnimation->stopAnimation();
    }
}

/**
 * @brief  槽函数 ,当用户选择手动输入授权码激活时,
 *         监听行编辑栏是否有输入，根据状态设置激活按钮的样式以及是否使能
 */
void ActGuideWidget::setManualLineEditStatus()
{
    if (ui->lineEdit_lc->text().isEmpty())
    {
        ui->btn_active_manual->setEnabled(false);
        ui->btn_active_manual->setStyleSheet("QPushButton#btn_active_manual {color: rgba(255,255,255,0.3);}");
    }
    else
    {
        ui->btn_active_manual->setEnabled(true);
        ui->btn_active_manual->setStyleSheet("QPushButton#btn_active_manual { color:#ffffff;}");
    }
}

/**
 * @brief  槽函数 ,当用户选择在线激活时,
 *         监听行编辑栏是否有输入，根据状态设置激活按钮的样式以及是否使能
 */
void ActGuideWidget::setKeyLineEditStatus()
{
    if (ui->lineEdit_address->text().isEmpty())
    {
        ui->btn_active_online->setEnabled(false);
        ui->btn_active_online->setStyleSheet("QPushButton#btn_active_online { color: rgba(255,255,255,0.3);}");
    }
    else
    {
        ui->btn_active_online->setEnabled(true);
        ui->btn_active_online->setStyleSheet("QPushButton#btn_active_online { color:#ffffff;}");
    }
}

/**
 * @brief  槽函数 ,当用户点击下一步按钮时，切换到对应的界面
 */
void ActGuideWidget::handleNextButton()
{
    if (ui->rbtn_manual->isChecked())
    {
        ui->stackedWidget->setCurrentIndex(Page_Manual);
        ui->listWidget->setCurrentRow(1);
        ui->lineEdit_lc->setFocus();
    }
    else if (ui->rbtn_offline->isChecked())
    {
        ui->stackedWidget->setCurrentIndex(Page_Key);
        ui->listWidget->setCurrentRow(1);
    }
    else if (ui->rbtn_online->isChecked())
    {
        ui->stackedWidget->setCurrentIndex(Page_Online);
        ui->listWidget->setCurrentRow(1);
        ui->lineEdit_address->setFocus();
    }
}

void ActGuideWidget::sleep(int sec)
{
    QTime dieTime = QTime::currentTime().addSecs(sec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

/**
 * @brief 默认将授权点数标签隐藏
 */
void ActGuideWidget::hideLabelPointsDefault()
{
    ui->label_authorized_points->hide();
    ui->label_remaining_points->hide();
    ui->authorized_points->hide();
    ui->remaining_points->hide();

    ui->label_desktop_points->hide();
    ui->label_development_points->hide();
    ui->label_industrial_points->hide();
    ui->label_server_points->hide();
    ui->desktop_points->hide();
    ui->development_points->hide();
    ui->industrial_points->hide();
    ui->server_points->hide();
}

/**
 * @brief 解析用UsbKey激活时Dbus后端传入的授权点数Json字符串
 * @param jsonString: 授权点数Json字符串
 */
void ActGuideWidget::getJsonValueFromString(const QString jsonString)
{
    int authorized_points = 0;
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data(), &jsonError);
    if (jsonDocument.isNull() || jsonError.error != QJsonParseError::NoError)
    {
        KLOG_DEBUG() << " please check the string " << jsonString.toLocal8Bit().data();
        return;
    }
    if (jsonDocument.isObject())
    {
        QJsonObject obj = jsonDocument.object();

        if (obj.contains(ALMIGHTY_AUTHORIZED_POINTS))
        {
            QJsonValue value = obj.take(ALMIGHTY_AUTHORIZED_POINTS);
            if (value.isDouble())
            {
                authorized_points = value.toVariant().toInt();
            }
        }

        ///正常显示全部点数和剩余点数
        if (authorized_points > 0)
        {
            if (obj.contains(ALMIGHTY_REMAINING_POINTS))
            {
                QJsonValue value_remianing_points = obj.take(ALMIGHTY_REMAINING_POINTS);
                if (value_remianing_points.isDouble())
                {
                    int remaining_points = value_remianing_points.toVariant().toInt();
                    ui->authorized_points->setNum(authorized_points);
                    ui->remaining_points->setNum(remaining_points);
                    setLabelPointsVisible(true);
                }
            }
        }
        else
        {
            setLabelPointsVisible(false);
            ///获取各个安装形态的授权点数
            if (obj.contains(DESKTOP_AUTHORIZED_POINTS) && obj.contains(DESKTOP_REMAINING_POINTS))
            {
                QJsonValue value_authorized_points = obj.take(DESKTOP_AUTHORIZED_POINTS);
                int desktop_authorized_points = value_authorized_points.toVariant().toInt();
                QJsonValue value_remaining_points = obj.take(DESKTOP_REMAINING_POINTS);
                int desktop_remaining_points = value_remaining_points.toVariant().toInt();
                ui->desktop_points->setText(QString("%1/%2").arg(desktop_authorized_points).arg(desktop_remaining_points));
            }
            else
            {
                ui->desktop_points->setNum(0);
            }

            if (obj.contains(DEVELOPMENT_AUTHORIZED_POINTS) && obj.contains(DEVELOPMENT_REMAINING_POINTS))
            {
                QJsonValue value_authorized_points = obj.take(DEVELOPMENT_AUTHORIZED_POINTS);
                int development_authorized_points = value_authorized_points.toVariant().toInt();

                QJsonValue value_remaining_points = obj.take(DEVELOPMENT_REMAINING_POINTS);
                int development_remaining_points = value_remaining_points.toVariant().toInt();
                ui->development_points->setText(QString("%1/%2").arg(development_authorized_points).arg(development_remaining_points));
            }
            else
            {
                ui->development_points->setNum(0);
            }

            if (obj.contains(INDUSTRIAL_AUTHORIZED_POINTS) && obj.contains(INDUSTRIAL_REMAINING_POINTS))
            {
                QJsonValue value_authorized_points = obj.take(INDUSTRIAL_AUTHORIZED_POINTS);
                int industrial_authorized_points = value_authorized_points.toVariant().toInt();

                QJsonValue value_remaining_points = obj.take(INDUSTRIAL_REMAINING_POINTS);
                int industrial_remaining_points = value_remaining_points.toVariant().toInt();
                ui->industrial_points->setText(QString("%1/%2").arg(industrial_authorized_points).arg(industrial_remaining_points));
            }
            else
            {
                ui->industrial_points->setNum(0);
            }

            if (obj.contains(SERVER_AUTHORIZED_POINTS) && obj.contains(SERVER_REMAINING_POINTS))
            {
                QJsonValue value_authorized_points = obj.take(SERVER_AUTHORIZED_POINTS);
                int server_authorized_points = value_authorized_points.toVariant().toInt();

                QJsonValue value_remaining_points = obj.take(SERVER_REMAINING_POINTS);
                int server_remaining_points = value_remaining_points.toVariant().toInt();
                ui->server_points->setText(QString("%1/%2").arg(server_authorized_points).arg(server_remaining_points));
            }
            else
            {
                ui->server_points->setNum(0);
            }
        }
    }
}

/**
 * @brief  槽函数 ,当用户点击激活按钮时，根据激活方式，开始激活，并且发射激活状态信号
 */
void ActGuideWidget::handleActiveButton()
{
    int current_page = ui->stackedWidget->currentIndex();
    QString inputLicenseCode;
    QString inputIp;
    QString licenseRegisterError;
    QString keyRegisterError;
    QString onlineRegisterError;
    QString ukey_json;

    switch (current_page)
    {
    case Page_Manual:
        //get EditLine text
        inputLicenseCode = ui->lineEdit_lc->text();
        KLOG_INFO() << "user input activation code :" << inputLicenseCode;

        //active
        ui->listWidget->setCurrentRow(2);
        ui->stackedWidget->setCurrentIndex(Page_Activating);
        byManual = true;
        byKey = false;
        byOnline = false;
        //sleep(3);
        activedFlag = InfoDbus::KylinLicense::registerByLicenseCode(inputLicenseCode, licenseRegisterError);
        if (!activedFlag)
        {
            KLOG_DEBUG() << licenseRegisterError;
            QString error = QString("<p align=\"center\"> %1 </p>").arg(licenseRegisterError);
            ui->error_text->setText(error);
            emit systemIsActived(false);
        }
        else
        {
            KLOG_DEBUG() << "manual register successful\n";
            emit systemIsActived(true);
        }
        break;

    case Page_Key:
        byManual = false;
        byKey = true;
        byOnline = false;
        ui->listWidget->setCurrentRow(2);
        ui->stackedWidget->setCurrentIndex(Page_Activating);
        loadAnimation->resumeAnimation();
        sleep(2);

        if (InfoDbus::KylinLicense::registerByUsbKey(ukey_json, keyRegisterError))
        {
            KLOG_DEBUG() << "key register successful \n";

            ///解析DBus后台传来的json字符串
            KLOG_INFO() << ukey_json;
            getJsonValueFromString(ukey_json);
            emit systemIsActived(true);
        }
        else
        {
            KLOG_DEBUG() << keyRegisterError;
            QString error = QString("<p align=\"center\"> %1 </p>").arg(keyRegisterError);
            ui->error_text->setText(error);
            emit systemIsActived(false);
        }
        break;

    case Page_Online:
        byKey = false;
        byManual = false;
        byOnline = true;
        ui->listWidget->setCurrentRow(2);
        ui->stackedWidget->setCurrentIndex(Page_Activating);
        loadAnimation->resumeAnimation();

        sleep(2);

        inputIp = ui->lineEdit_address->text();
        KLOG_INFO() << "input ip is: " << inputIp;

        if (InfoDbus::KylinLicense::registerOnline(inputIp, onlineRegisterError))
        {
            KLOG_DEBUG() << "OnLine register successful \n";
            emit systemIsActived(true);
        }
        else
        {
            KLOG_DEBUG() << onlineRegisterError;
            QString error = QString("<p align=\"center\"> %1 </p>").arg(onlineRegisterError);
            ui->error_text->setText(error);
            emit systemIsActived(false);
        }
        break;
    default:
        break;
    }
}

/**
 * @brief  槽函数 ,当用户点击返回按钮时，切换到对应的界面
 */
void ActGuideWidget::handleBackButton()
{
    int current_page = ui->stackedWidget->currentIndex();
    switch (current_page)
    {
    case Page_Manual:
    case Page_Online:
    case Page_Key:
        ui->stackedWidget->setCurrentIndex(Page_Start);
        ui->listWidget->setCurrentRow(0);
        break;
    case Page_Failed:
        //error message determin
        if (byManual)
        {
            ui->stackedWidget->setCurrentIndex(Page_Manual);
            ui->listWidget->setCurrentRow(1);
        }
        else if (byKey)
        {
            ui->stackedWidget->setCurrentIndex(Page_Key);
            ui->listWidget->setCurrentRow(1);
        }
        else if (byOnline)
        {
            ui->stackedWidget->setCurrentIndex(Page_Online);
            ui->listWidget->setCurrentRow(1);
        }

        break;
    default:
        break;
    }
    ui->lineEdit_address->setFocus();
    ui->lineEdit_lc->setFocus();
}

void ActGuideWidget::handleCloseButton()
{
    this->close();
}

ActGuideWidget::~ActGuideWidget()
{
    if (showQRCode != nullptr)
    {
        delete showQRCode;
    }
    delete listItemActive;
    delete listItemComplete;
    delete listItemMode;
    delete ui;
}

bool ActGuideWidget::getLineEditStatus()
{
    if (ui->lineEdit_address->text() != NULL || ui->lineEdit_lc->text() != NULL)
        return true;
    else
        return false;
}
