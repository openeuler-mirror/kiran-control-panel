#include "user-license-agreement.h"
#include "ui_user-license-agreement.h"
#include <QIcon>
#include <QFile>
#include <QDebug>
#include <QDesktopWidget>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include <kiranwidgets-qt5/kiran-style-public-define.h>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QFileDialog>
#include <QtPrintSupport/QPrinter>
#include <QTextDocument>
#include <QStandardPaths>
#define EULAFILE "/usr/share/kylin-release/EULA"

using namespace Kiran::WidgetPropertyHelper;
using namespace Kiran;

UserlicenseAgreement::UserlicenseAgreement() :
    KiranTitlebarWindow(),
    ui(new Ui::UserlicenseAgreement)
{
    ui->setupUi(getWindowContentWidget());
    initUI();

    addText();
    connect(ui->btn_eula_close,&QPushButton::clicked,
            [this]{
        this->close();
    });

    connect(ui->btn_eula_export,&QPushButton::clicked,this ,&UserlicenseAgreement::exportEULA);
}

UserlicenseAgreement::~UserlicenseAgreement()
{
    delete ui;
}

QString UserlicenseAgreement::getEulaText()
{
    QString text = ui->text_eula->toPlainText();
    return text;
}

void UserlicenseAgreement::exportEULA()
{
    QString eulaText = ui->text_eula->toPlainText();
    QString currentHomePath = "/" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) +"/EULA.pdf" ;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save"),
                                                    currentHomePath,
                                                    tr("PDF(*.pdf)"));
    if(fileName.isNull())
    {
        return;
    }
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        KiranMessageBox::KiranStandardButton button = KiranMessageBox::message(nullptr,QString(tr("Export EULA")),QString(tr("Export EULA failed!")),KiranMessageBox::Ok);
        if(button == KiranMessageBox::Ok)
        {
            return;
        }
    }
    else
    {
        //将EULA文字转化为PDF
        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPaperSize(QPrinter::A4);
        printer.setOutputFileName(fileName);

        QTextDocument doc;
        doc.setPlainText(eulaText); /* 可替换为文档内容 */
        doc.setPageSize(printer.pageRect().size());
        doc.print(&printer);
    }
}

void UserlicenseAgreement::initUI()
{
    setTitle(UserlicenseAgreement::tr("User End License Agreement"));
    setIcon(QIcon(":/images/kylin-about.png"));
    setResizeable(false);
    setButtonHints(TitlebarMinimizeButtonHint | TitlebarCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);

    setButtonType(ui->btn_eula_export,BUTTON_Default);
    setButtonType(ui->btn_eula_close,BUTTON_Normal);
}

void UserlicenseAgreement::addText()
{
    QFile file(EULAFILE);

    if(!file.exists())
    {
        qInfo() << "Can't find EULA file" <<endl;
        ui->text_eula->setText(tr("None"));
        return;
    }

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        qInfo() << "Can't open EULA file" << endl;
        ui->text_eula->setText(tr("None"));
        return ;
    }
    QTextStream textStream(&file);

    ui->text_eula->setText(textStream.readAll());
}


