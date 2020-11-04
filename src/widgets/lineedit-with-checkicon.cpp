#include "lineedit-with-checkicon.h"
#include "ui_lineedit-with-checkicon.h"

#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QSvgRenderer>
#include <QKeyEvent>

LineEditWithCheckIcon::LineEditWithCheckIcon(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LineEditWithCheckIcon),
    m_verificationStatus(VERIFICATION_NONE)
{
    ui->setupUi(this);
    initUI();
}

LineEditWithCheckIcon::~LineEditWithCheckIcon()
{
    delete ui;
}

void LineEditWithCheckIcon::setEchoMode(QLineEdit::EchoMode echoMode)
{
    ui->lineEdit->setEchoMode(echoMode);
}

void LineEditWithCheckIcon::setMaxLength(int length)
{
    ui->lineEdit->setMaxLength(length);
}

void LineEditWithCheckIcon::setPlaceholderText(const QString &placeholder)
{
    ui->lineEdit->setPlaceholderText(placeholder);
}

void LineEditWithCheckIcon::resetVerificationStatus()
{
    ui->checkIndicator->setPixmap(QPixmap());
    m_verificationStatus = VERIFICATION_NONE;
}

void LineEditWithCheckIcon::setVerificationStatus(bool isPassed)
{
    QString iconPath = isPassed?":/images/icon_cor`rect.svg":":/images/icon_error.svg";
    m_verificationStatus = isPassed?VERIFICATION_PASSED:VERIFICATION_ERROR;

    ui->checkIndicator->setPixmap(iconPath);
}

QString LineEditWithCheckIcon::text()
{
    return ui->lineEdit->text();
}

void LineEditWithCheckIcon::clear()
{
    ui->lineEdit->clear();
}

void LineEditWithCheckIcon::initUI()
{
    ui->lineEdit->installEventFilter(this);
    ui->lineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    connect(ui->lineEdit,&QLineEdit::textChanged,[this](const QString &text){
        emit editTextChanged(text);

        ///密码框输入密码不为空的情况下调整字体和字间距
        if(ui->lineEdit->echoMode()==QLineEdit::Password && text.isEmpty()){
            setShowPasswordModeStyle(false);
            setNormalLetterSpacing();
        }else if(ui->lineEdit->echoMode()==QLineEdit::Password && !text.isEmpty() ){
            setShowPasswordModeStyle(true);
            setPasswdLetterSpacing();
        }
    });
}

void LineEditWithCheckIcon::setNormalLetterSpacing()
{
    QFont font = ui->lineEdit->font();
    font.setLetterSpacing(QFont::PercentageSpacing,0);
    ui->lineEdit->setFont(font);
}

void LineEditWithCheckIcon::setPasswdLetterSpacing()
{
    QFont font = ui->lineEdit->font();
    font.setLetterSpacing(QFont::AbsoluteSpacing,4);
    ui->lineEdit->setFont(font);
}

void LineEditWithCheckIcon::setShowPasswordModeStyle(bool showPasswordModeStyle)
{
    if(m_showPasswordModeStyle==showPasswordModeStyle){
        return;
    }
    m_showPasswordModeStyle = showPasswordModeStyle;
    style()->polish(this);
    style()->polish(ui->lineEdit);
}

bool LineEditWithCheckIcon::eventFilter(QObject *obj, QEvent *event)
{
    if( obj == ui->lineEdit ){
        switch (event->type()) {
        case QEvent::FocusIn:
            resetVerificationStatus();
            break;
        case QEvent::KeyPress:
        {
            QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
            if(keyEvent->key()==Qt::Key_Escape&&ui->lineEdit->echoMode()==QLineEdit::Password){
                ui->lineEdit->clear();
            }
        }
        default:
            break;
        }
    }
    return false;
}

void LineEditWithCheckIcon::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    QPainter painter(this);

    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
