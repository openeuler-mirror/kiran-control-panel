#include "ukey-pin-input-dialog.h"
#include <kiran-color-block.h>
#include <kiran-passwd-edit.h>
#include <style-property.h>
#include <QBoxLayout>
#include <QEventLoop>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <qt5-log-i.h>
#include "utils/kiran-auth-dbus-proxy.h"

UKeyPinCodeDialog::UKeyPinCodeDialog(QWidget* parent)
    : KiranTitlebarWindow(parent,Qt::Dialog),
      m_success(false)
{
    setAttribute(Qt::WA_ShowModal, true);
    initUI();
}

UKeyPinCodeDialog::~UKeyPinCodeDialog()
{
}

QString UKeyPinCodeDialog::getPinCode()
{
    return m_edit->lineEdit()->text();
}

int UKeyPinCodeDialog::exec()
{
    QEventLoop loop;
    connect(this, &UKeyPinCodeDialog::completed, &loop, &QEventLoop::quit);
    this->show();
    loop.exec(QEventLoop::DialogExec);
    return m_success;
}

void UKeyPinCodeDialog::closeEvent(QCloseEvent* event)
{
    emit completed(QPrivateSignal());
    return KiranTitlebarWindow::closeEvent(event);
}

void UKeyPinCodeDialog::onConfirmClicked()
{
    QString text = m_edit->lineEdit()->text();
    if (text.isEmpty())
    {
        return;
    }
    m_success = true;
    emit completed(QPrivateSignal());
}

void UKeyPinCodeDialog::initUI()
{
    setTitlebarColorBlockEnable(true);
    setButtonHints(TitlebarCloseButtonHint);
    setResizeable(false);
    setTitle(tr("UKey Enroll"));

    auto container = new QWidget(this);
    auto containerLayout = new QBoxLayout(QBoxLayout::TopToBottom, container);
    containerLayout->setContentsMargins(4, 4, 4, 4);

    auto colorBlock = new KiranColorBlock(container);
    colorBlock->setDrawBackground(true);
    containerLayout->addWidget(colorBlock);

    auto layout = new QBoxLayout(QBoxLayout::TopToBottom, colorBlock);
    layout->setContentsMargins(24, 24, 24, 24);

    auto label = new QLabel(tr("Please enter the ukey pin code"), this);
    layout->addWidget(label);

    layout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_edit = new KiranPasswdEdit(this);
    m_edit->setEchoMode(QLineEdit::Password);
    m_edit->lineEdit()->setMaxLength(32);
    layout->addWidget(m_edit);

    layout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto boxlayout = new QBoxLayout(QBoxLayout::LeftToRight);
    boxlayout->setContentsMargins(0, 0, 0, 0);

    boxlayout->addStretch();

    auto confirmButton = new QPushButton(this);
    confirmButton->setFixedSize(QSize(110, 40));
    confirmButton->setText(tr("Confirm"));
    Kiran::StylePropertyHelper::setButtonType(confirmButton, Kiran::BUTTON_Default);
    connect(confirmButton, &QPushButton::clicked, this, &UKeyPinCodeDialog::onConfirmClicked);
    boxlayout->addWidget(confirmButton);

    boxlayout->addSpacerItem(new QSpacerItem(40, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));

    auto cancelButton = new QPushButton(this);
    cancelButton->setFixedSize(QSize(110, 40));
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, &QPushButton::clicked, this, [this]()
            { emit completed(QPrivateSignal()); });
    boxlayout->addWidget(cancelButton);

    boxlayout->addStretch();

    layout->addLayout(boxlayout);

    setWindowContentWidget(container);
}
