#include "shortcut.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include <QClipboard>
#include <QKeyEvent>
#include "custom-line-edit.h"
#include "key-map.h"
#include "shortcut-item.h"
#include "thread-object.h"
#include "ui_shortcut.h"
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
    m_btnCustomApp->setCursor(Qt::PointingHandCursor);
    hLayoutCustomApp->addStretch();
    hLayoutCustomApp->addWidget(m_btnCustomApp);
    ui->lineEdit_custom_app->setTextMargins(0, 0, m_btnCustomApp->width(), 0);

    QHBoxLayout *hLayoutModifyApp = new QHBoxLayout(ui->lineEdit_modify_app);
    m_btnModifyApp = new QToolButton;
    m_btnModifyApp->setObjectName("btn_modify_app");
    m_btnModifyApp->setText("Add");
    m_btnModifyApp->setFixedSize(56, 30);
    m_btnModifyApp->setCursor(Qt::PointingHandCursor);
    hLayoutModifyApp->addStretch();
    hLayoutModifyApp->addWidget(m_btnModifyApp);
    ui->lineEdit_modify_app->setTextMargins(0, 0, m_btnModifyApp->width(), 0);

    m_lECustomKey = new CustomLineEdit;
    m_lECustomKey->setPlaceholderText(tr("Please press the new shortcut key"));
    m_lECustomKey->installEventFilter(this);
    ui->vlayout_custom_key->addWidget(m_lECustomKey);
    connect(m_lECustomKey, &CustomLineEdit::inputKeyCodes, this, &Shortcut::handleInputKeycode);

    m_lEModifyKey = new CustomLineEdit;
    m_lEModifyKey->setPlaceholderText(tr("Please press the new shortcut key"));
    m_lEModifyKey->installEventFilter(this);
    ui->vlayout_modify_key->addWidget(m_lEModifyKey);
    connect(m_lEModifyKey, &CustomLineEdit::inputKeyCodes, this, &Shortcut::handleInputKeycode);

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

bool Shortcut::isConflict(QString keyStr)
{
    foreach (ShortcutInfo *shortcut, m_shortcuts)
    {
        if (!QString::compare(shortcut->keyCombination, keyStr, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

QString Shortcut::convertToString(QList<int> keyCode)
{
    QStringList keyStr;
    QString str;
    foreach (int keycode, keyCode)
    {
        str = m_keyMap->keycodeToString(keycode);
        KLOG_INFO() << str.toLower();
        if (keycode >= 0x30 && keycode <= 0x39)  //数字转化
        {
            keyStr.append(str.split("Key_").at(1));
        }
        else if (keyCode.size() > 1 && SpecialKeyMap.contains(str.toLower()))  //特殊字符转化
        {
            keyStr.append(SpecialKeyMap.value(str.toLower()));
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
            if (shortcutInfo->kind == SHORTCUT_KIND_SOUND)
                createShortcutItem(ui->vlayout_sound, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == SHORTCUT_KIND_WINDOW_MANAGE)
                createShortcutItem(ui->vlayout_marco, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == SHORTCUT_KIND_SYSTEM)
                createShortcutItem(ui->vlayout_system, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == SHORTCUT_KIND_ACCESSIBILITY)
                createShortcutItem(ui->vlayout_accessibility, shortcutInfo, shortcutInfo->type);
            else if (shortcutInfo->kind == SHORTCUT_KIND_DESKTOP)
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
    CustomLineEdit *senderLineEdit = qobject_cast<CustomLineEdit *>(sender());
    //转化成字符串列表
    QString keyStr = convertToString(keycodes);

    //判断单个key是否在ignoreKey中
    if (keycodes.size() == 1)
    {
        KLOG_INFO() << keyStr;
        if (ignoreKeys.contains(keyStr, Qt::CaseInsensitive) ||
            keyStr.contains(QRegExp("[A-Z]")) ||
            keyStr.contains(QRegExp("[0-9]")))
        {
            KiranMessageBox::message(nullptr,
                                     tr("Failed"),
                                     QString(tr("Cannot use shortcut \"%1\", Because you cannot enter with this key."
                                                "Please try again using Ctrl, alt, or shift at the same time."))
                                         .arg(keyStr),
                                     KiranMessageBox::Ok);
            senderLineEdit->clear();
            senderLineEdit->clearFocus();
            return;
        }
    }
    //    //判断是否重复
    //    if (isConflict(keyStr))
    //    {
    //        //        KiranMessageBox::message(nullptr,
    //        //                                 QString(tr("Shortcut keys %1 are already used in %2,")).arg(),
    //        //                                 QString(tr("If you reassign shortcut keys to %1, %2 Shortcut keys for will be disabled.")))
    //        KLOG_INFO() << "isConflict";
    //        m_lECustomKey->clear();
    //        return;
    //    }
    //
    KLOG_INFO() << "ok: " << keyStr;
    senderLineEdit->setText(keyStr);

    //显示在输入框中
    senderLineEdit->clearFocus();
}

//解决输入Ctrl+v会显示剪切板中的内容
bool Shortcut::eventFilter(QObject *target, QEvent *event)
{
    if (target == m_lECustomKey || target == m_lEModifyKey)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->matches(QKeySequence::Paste))
            {
                KLOG_INFO() << "Ctrl + V";
                return true;
            }
        }
    }
    return QWidget::eventFilter(target, event);
}
