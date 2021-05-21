#include "user-license-agreement.h"
#include "ui_user-license-agreement.h"
#include <QIcon>
#include <QFile>
#include <QDebug>

#define EULAFILE "/usr/share/kylin-release/EULA"

UserlicenseAgreement::UserlicenseAgreement() :
    KiranTitlebarWindow(),
    ui(new Ui::UserlicenseAgreement)
{
    ui->setupUi(getWindowContentWidget());
    initUI();
    connect(ui->btn_eula_close,&QPushButton::clicked,[this]{
            this->close();
        });
    addText();
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

void UserlicenseAgreement::initUI()
{
    setTitle(UserlicenseAgreement::tr("User End License Agreement"));
    setIcon(QIcon(":/images/kylin-about.png"));
    setResizeable(false);
    setButtonHints(TitlebarMinimizeButtonHint | TitlebarCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);
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


