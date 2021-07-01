#include "license-info-widget.h"
#include "ui_license-info-widget.h"
#include <iostream>
#include <QDesktopWidget>

using namespace std;

LicenseInfoWidget::LicenseInfoWidget(QString &mc_code, QString & lc_code, QWidget *parent) :
    KiranTitlebarWindow(parent),
    ui(new Ui::LicenseInfoWidget),
    showQRCode(nullptr)
{
    ui->setupUi(getWindowContentWidget());
    iniUI();

    machine_code = mc_code ;
    license_code = lc_code;

    ui->btn_qrcode_lc->installEventFilter(this);
    ui->btn_qrcode_mc->installEventFilter(this);

    connect(this ,SIGNAL(sig_showQRCodeWgt(QPoint,QObject*)),this ,SLOT(popupQRCode(QPoint,QObject*)));

}

void LicenseInfoWidget::iniUI()
{
    setTitle(LicenseInfoWidget::tr("License-Information"));
    setWindowModality(Qt::ApplicationModal);
    setButtonHints(TitlebarMinimizeButtonHint|TitlebarCloseButtonHint);
    setResizeable(false);
    setIcon(QIcon(":/images/kylin-about.png"));
}

/**
 * @brief  显示二维码的回调函数
 * @param  oPoint  二维码显示的位置
 * @param  target  事件对象，用来决定显示机器码还是二维码
 */
void LicenseInfoWidget::popupQRCode(QPoint oPoint ,QObject *target)
{
    if(showQRCode == nullptr)
    {
        showQRCode = new ShowQRCode;
    }
    showQRCode->setWindowFlag(Qt::X11BypassWindowManagerHint);
    if(target == ui->btn_qrcode_lc)
    {
        showQRCode->setQRCode(license_code , false);

    }
    else if(target ==  ui->btn_qrcode_mc)
    {
        showQRCode->setQRCode(machine_code ,true);
    }
    showQRCode->move(oPoint);
    showQRCode->show();
}

/**
 * @brief  事件监听，事件监听，监听鼠标进入或离开二维码按钮事件 ，
 *         进入按钮则发出弹出二维码信号，离开则隐藏二维码窗口，并释放二维码窗口内存
 * @param  obj  事件对象
 * @param  obj  事件
 * @return 是否过滤
 */
bool LicenseInfoWidget::eventFilter(QObject *target, QEvent *e)
{
    if(target == ui->btn_qrcode_lc || target == ui->btn_qrcode_mc)
    {
        if(e->type()==QEvent::Enter)
        {
            if(target == ui->btn_qrcode_mc)
            {
                QPoint oPoint = this->mapToGlobal(QPoint(395,20));
                emit sig_showQRCodeWgt(oPoint,target);
            }
            else
            {
                QPoint oPoint = this->mapToGlobal(QPoint(395,90));
                emit sig_showQRCodeWgt(oPoint,target);
            }
        }
        else if (e->type()==QEvent::Leave)
        {
            showQRCode->hide();
            delete showQRCode;
            showQRCode = nullptr;

        }

    }
    return false;
}


void LicenseInfoWidget::setMachineCode(QString & machine_code)
{
    if(machine_code =="NULL")
    {
        ui->label_mc->setText(tr("Can't get machine code"));
        return;
    }
    ui->label_mc->setText(machine_code);
}

void LicenseInfoWidget::setLicenseCode(QString & license_code)
{
    if(license_code == "NULL")
    {
        ui->label_lc->setText(tr("Can't get license code"));
        return ;
    }
    ui->label_lc->setText(license_code);
}


LicenseInfoWidget::~LicenseInfoWidget()
{
    delete ui;
}
