#include "shortcut.h"
#include <kiran-log/qt5-log-i.h>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include "custom-line-edit.h"
#include "shortcut-item.h"
#include "ui_shortcut.h"

Shortcut::Shortcut(QWidget *parent) : QWidget(parent),
                                      ui(new Ui::Shortcut)
{
    ui->setupUi(this);
    initUI();
}

Shortcut::~Shortcut()
{
    delete ui;
}

QSize Shortcut::sizeHint() const
{
    return QSize(650, 730);
}

void Shortcut::initUI()
{
    ui->lineEdit_search->setPlaceholderText(tr("Please enter a search keyword..."));
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_shortcut_add, Kiran::BUTTON_Default);
    ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
    ui->stackedWidget_search->setCurrentWidget(ui->page_shortcut_list);

    QList<QLineEdit *> lineEdits = ui->page_add->findChildren<QLineEdit *>();
    foreach (QLineEdit *lineEdit, lineEdits)
    {
        lineEdit->setPlaceholderText(tr("Required"));
    }

    QHBoxLayout *hLayoutCustomApp = new QHBoxLayout(ui->lineEdit_custom_app);
    m_btnCustomApp = new QToolButton;
    m_btnCustomApp->setObjectName("btn_custom_app");
    m_btnCustomApp->setText("Add");
    m_btnCustomApp->setFixedSize(56, 30);
    hLayoutCustomApp->addStretch();
    hLayoutCustomApp->addWidget(m_btnCustomApp);
    ui->lineEdit_custom_app->setTextMargins(0, 0, m_btnCustomApp->width(), 0);

    QHBoxLayout *hLayoutModifyApp = new QHBoxLayout(ui->lineEdit_modify_app);
    m_btnModifyApp = new QToolButton;
    m_btnModifyApp->setObjectName("btn_modify_app");
    m_btnModifyApp->setText("Add");
    m_btnModifyApp->setFixedSize(56, 30);
    hLayoutModifyApp->addStretch();
    hLayoutModifyApp->addWidget(m_btnModifyApp);
    ui->lineEdit_modify_app->setTextMargins(0, 0, m_btnModifyApp->width(), 0);

    connect(ui->btn_shortcut_add, &QPushButton::clicked,
            [this] {
                KLOG_INFO() << "btn_shortcut_add clicked!";
                ui->stackedWidget->setCurrentWidget(ui->page_add);
                connect(ui->btn_page_add, &QPushButton::clicked,
                        [=] {
                            //dbus ->AddCustomShortcut
                        });
            });

    connect(ui->btn_edit, &QToolButton::clicked,
            [this] {
                m_isEditMode = !m_isEditMode;
                if (m_isEditMode)
                    ui->btn_edit->setText(tr("Finished"));
                else
                    ui->btn_edit->setText(tr("Edit"));

                foreach (ShortcutItem *item, m_shortcutItem)
                    item->setEditMode(m_isEditMode);
            });

    connect(ui->btn_cancel, &QPushButton::clicked,
            [this] {
                ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
            });

    connect(ui->btn_return, &QPushButton::clicked,
            [this] {
                ui->stackedWidget->setCurrentWidget(ui->page_shortcut);
            });

    getAllCustomShortcut();
}

void Shortcut::createShortcutItem(QVBoxLayout *parent, ShortcutInfo *shortcutInfo)
{
    shortcutInfo->name = "screen";
    shortcutInfo->keyCombination = "Alt+A";

    ShortcutItem *item = new ShortcutItem(SHORTCUT_TYPE_CUSTOM, shortcutInfo);
    parent->addWidget(item);
    m_shortcutItem.append(item);

    connect(item, &ShortcutItem::sigClicked,
            [=](int type, QString uid, QString name, QString keyCombination, QString action) {
                ui->stackedWidget->setCurrentWidget(ui->page_modify);
                if (type == SHORTCUT_TYPE_SYSTEM)
                {
                }
                connect(ui->btn_save, &QPushButton::clicked,
                        [=] {
                            //dbus -> ModifyCustomShortcut/ModifySystemShortcut
                        });
            });
}

void Shortcut::getAllSystemShortcut()
{
}

void Shortcut::getAllCustomShortcut()
{
    ShortcutInfo *shortcutInfo = new ShortcutInfo;
    shortcutInfo->name = "screen";
    shortcutInfo->keyCombination = "Alt+A";
    createShortcutItem(ui->vlayout_custom, shortcutInfo);
}

void Shortcut::getJsonValueFromString(QString jsonString, int type)
{
}

void Shortcut::deleteShortcut(QString uid)
{
}

void Shortcut::editShortcut(QString uid, QString name, QString keyCombination, QString action)
{
}

void Shortcut::addShortcut()
{
}
