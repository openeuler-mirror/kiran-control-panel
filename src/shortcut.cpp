#include "shortcut.h"
#include <kiran-log/qt5-log-i.h>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include "custom-line-edit.h"
#include "key-map.h"
#include "shortcut-item.h"
#include "thread-object.h"
#include "ui_shortcut.h"

QStringList ignoreKeys = {
    "Tab",
    "Return",
    "Enter",
    "Return"
    "Space",
    "Esc"
    "Home",
    "End",
    "PgUp"
    "PgDown"
    "Up",
    "Down",
    "Left",
    "Right"};

Shortcut::Shortcut(QWidget *parent) : QWidget(parent),
                                      ui(new Ui::Shortcut)
{
    ui->setupUi(this);
    m_keyMap = new KeyMap;
    initUI();
}

Shortcut::~Shortcut()
{
    delete ui;
    m_thread->quit();
    m_thread->wait();
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
                            //successful ->ui->widget_custom->show();
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

    getAllShortcuts();

    connect(ui->lineEdit_add_key, &CustomLineEdit::inputKeyCodes, this, &Shortcut::handleInputKeycode);
}

void Shortcut::createShortcutItem(QVBoxLayout *parent, ShortcutInfo *shortcutInfo, int type)
{
    ShortcutItem *item = new ShortcutItem(type, shortcutInfo);
    parent->addWidget(item);
    m_shortcutItem.append(item);

    connect(item, &ShortcutItem::sigClicked,
            [=](int type, QString uid, QString name, QString keyCombination, QString action) {
                ui->stackedWidget->setCurrentWidget(ui->page_modify);
                if (type == SHORTCUT_TYPE_SYSTEM)
                {
                    ui->widget_modify_app->hide();
                    ui->lineEdit_modify_name->setText(name);
                    ui->lineEdit_modify_name->setDisabled(true);
                }
                connect(ui->btn_save, &QPushButton::clicked,
                        [=] {
                            //dbus -> ModifyCustomShortcut/ModifySystemShortcut
                        });
            });
    connect(item, &ShortcutItem::sigDelete,
            [=](QString uid) {
                //dbus -> delete
                //if(m_shortcutItem.size == 0)  ---->ui->widget_custom->hide();
            });
}

void Shortcut::getAllShortcuts()
{
    m_thread = new QThread;
    m_threadObject = new ThreadObject;

    m_threadObject->moveToThread(m_thread);

    connect(m_threadObject, SIGNAL(getShortcutInfo(QList<ShortcutInfo *>)), this,
            SLOT(handleShortcutInfo(QList<ShortcutInfo *>)));
    //终止线程deleteLater
    connect(m_thread, SIGNAL(finished()), m_threadObject, SLOT(deleteLater()));
    connect(m_thread, SIGNAL(started()), m_threadObject, SLOT(loadShortcutInfo()));

    m_thread->start();
}

bool Shortcut::isIgnoreKey()
{
}

QString Shortcut::convertToString(QList<int> keyCode)
{
    QStringList keyStr;
    foreach (int keycode, keyCode)
    {
        if (keycode >= 0x30 && keycode <= 0x39)
        {
            keyStr.append(m_keyMap->keycodeToString(keycode).split("Key_").at(1));
        }
        else
            keyStr.append(m_keyMap->keycodeToString(keycode));
    }
    return keyStr.join("+");
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

void Shortcut::handleShortcutInfo(QList<ShortcutInfo *> shortcutInfoList)
{
    int customCount = 0;
    m_shortcuts = shortcutInfoList;

    foreach (ShortcutInfo *shortcutInfo, shortcutInfoList)
    {
        if (shortcutInfo->type == SHORTCUT_TYPE_SYSTEM)
        {
            if (shortcutInfo->kind == "Sound")
                createShortcutItem(ui->vlayout_sound, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == "窗口管理")
                createShortcutItem(ui->vlayout_marco, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == "System")
                createShortcutItem(ui->vlayout_system, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == "Accessibility")
                createShortcutItem(ui->vlayout_accessibility, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == "桌面")
                createShortcutItem(ui->vlayout_desktop, shortcutInfo, shortcutInfo->type);
        }
        else
        {
            createShortcutItem(ui->vlayout_custom, shortcutInfo, shortcutInfo->type);
            customCount++;
        }
    }

    if (customCount == 0)
    {
        ui->widget_custom->hide();
    }
}

void Shortcut::handleInputKeycode(QList<int> keycodes)
{
    //转化成字符串列表
    QString keyStr = convertToString(keycodes);
    KLOG_INFO() << keyStr;
    //判断单个key是否在ignoreKey中
    //转化成字符串（+拼接）
    //显示在输入框中
}
