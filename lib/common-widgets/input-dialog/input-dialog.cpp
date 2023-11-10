#include "input-dialog.h"
#include <kiran-color-block.h>
#include <style-property.h>
#include <kiran-passwd-edit.h>
#include <QBoxLayout>
#include <QEventLoop>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <qt5-log-i.h>

InputDialog::InputDialog(QWidget* parent)
    : KiranTitlebarWindow(parent,Qt::Dialog),
      m_success(false)
{
    setAttribute(Qt::WA_ShowModal, true);
    initUI();
}

InputDialog::~InputDialog()
{
}

QString InputDialog::getText()
{
    return m_edit->lineEdit()->text();
}

void InputDialog::setDesc(const QString&desc)
{
    m_labelDesc->setText(desc);
}

void InputDialog::setInputMode(QLineEdit::EchoMode mode,qint32 maxLength)
{
    m_edit->setEchoMode(mode);
    m_edit->lineEdit()->setMaxLength(maxLength);
}

int InputDialog::exec()
{
    QEventLoop loop;
    connect(this, &InputDialog::completed, &loop, &QEventLoop::quit);
    this->show();
    loop.exec(QEventLoop::DialogExec);
    return m_success;
}

void InputDialog::closeEvent(QCloseEvent* event)
{
    emit completed();
    return KiranTitlebarWindow::closeEvent(event);
}

void InputDialog::onConfirmClicked()
{
    QString text = m_edit->lineEdit()->text();
    if (text.isEmpty())
    {
        return;
    }
    m_success = true;
    emit completed();
}

void InputDialog::initUI()
{
    setTitlebarColorBlockEnable(true);
    setButtonHints(TitlebarCloseButtonHint);
    setResizeable(false);

    auto container = new QWidget(this);
    auto containerLayout = new QBoxLayout(QBoxLayout::TopToBottom, container);
    containerLayout->setContentsMargins(4, 4, 4, 4);

    auto colorBlock = new KiranColorBlock(container);
    colorBlock->setDrawBackground(true);
    containerLayout->addWidget(colorBlock);

    auto layout = new QBoxLayout(QBoxLayout::TopToBottom, colorBlock);
    layout->setContentsMargins(24, 24, 24, 24);

    m_labelDesc = new QLabel(this);
    layout->addWidget(m_labelDesc);

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
    connect(confirmButton, &QPushButton::clicked, this, &InputDialog::onConfirmClicked);
    boxlayout->addWidget(confirmButton);

    boxlayout->addSpacerItem(new QSpacerItem(40, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));

    auto cancelButton = new QPushButton(this);
    cancelButton->setFixedSize(QSize(110, 40));
    cancelButton->setText(tr("Cancel"));
    connect(cancelButton, &QPushButton::clicked, this, [this]()
            { emit completed(); });
    boxlayout->addWidget(cancelButton);

    boxlayout->addStretch();

    layout->addLayout(boxlayout);

    setWindowContentWidget(container);
}