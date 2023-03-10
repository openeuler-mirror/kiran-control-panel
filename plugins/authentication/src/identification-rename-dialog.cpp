#include "identification-rename-dialog.h"
#include <kiran-color-block.h>
#include <style-property.h>
#include <QBoxLayout>
#include <QEventLoop>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <qt5-log-i.h>
#include "utils/kiran-auth-dbus-proxy.h"

IdentificationRenameDialog::IdentificationRenameDialog(const QString& iid, KiranAuthDBusProxy* proxy, QWidget* parent)
    : KiranTitlebarWindow(parent,Qt::Dialog),
      m_iid(iid),
      m_proxy(proxy),
      m_success(false)
{
    setAttribute(Qt::WA_ShowModal, true);
    initUI();
}

IdentificationRenameDialog::~IdentificationRenameDialog()
{
}

int IdentificationRenameDialog::exec()
{
    QEventLoop loop;
    connect(this, &IdentificationRenameDialog::completed, &loop, &QEventLoop::quit);
    this->show();
    loop.exec(QEventLoop::DialogExec);
    return m_success;
}

void IdentificationRenameDialog::closeEvent(QCloseEvent* event)
{
    emit completed(QPrivateSignal());
    return KiranTitlebarWindow::closeEvent(event);
}

void IdentificationRenameDialog::onConfirmClicked()
{
    QString text = m_edit->text();
    if (text.isEmpty())
    {
        return;
    }
    m_proxy->renameIdentification(m_iid, text);
    m_success = true;
    emit completed(QPrivateSignal());
}

void IdentificationRenameDialog::initUI()
{
    setTitlebarColorBlockEnable(true);
    setButtonHints(TitlebarCloseButtonHint);
    setResizeable(false);
    setTitle(tr("Rename Feature"));

    auto container = new QWidget(this);
    auto containerLayout = new QBoxLayout(QBoxLayout::TopToBottom, container);
    containerLayout->setContentsMargins(4, 4, 4, 4);

    auto colorBlock = new KiranColorBlock(container);
    colorBlock->setDrawBackground(true);
    containerLayout->addWidget(colorBlock);

    auto layout = new QBoxLayout(QBoxLayout::TopToBottom, colorBlock);
    layout->setContentsMargins(24, 24, 24, 24);

    auto label = new QLabel(tr("Please enter the renamed feature name"), this);
    layout->addWidget(label);

    layout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_edit = new QLineEdit(this);
    m_edit->setMaxLength(30);
    layout->addWidget(m_edit);

    layout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto boxlayout = new QBoxLayout(QBoxLayout::LeftToRight);
    boxlayout->setContentsMargins(0, 0, 0, 0);

    boxlayout->addStretch();

    auto confirmButton = new QPushButton(this);
    confirmButton->setFixedSize(QSize(110, 40));
    confirmButton->setText(tr("Confirm"));
    Kiran::StylePropertyHelper::setButtonType(confirmButton, Kiran::BUTTON_Default);
    connect(confirmButton, &QPushButton::clicked, this, &IdentificationRenameDialog::onConfirmClicked);
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
