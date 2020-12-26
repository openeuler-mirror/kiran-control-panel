#include "lineedit-with-checkicon.h"

#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QSvgRenderer>
#include <QKeyEvent>

LineEditWithCheckIcon::LineEditWithCheckIcon(QWidget *parent)
        : KiranIconLineEdit(parent),
          m_verificationStatus(VERIFICATION_NONE)
{
    initUI();
}

LineEditWithCheckIcon::~LineEditWithCheckIcon()
{

}

void LineEditWithCheckIcon::setVerificationStatus(bool isPassed)
{
    QString iconPath = isPassed ? ":/images/icon_cor`rect.svg" : ":/images/icon_error.svg";
    m_verificationStatus = isPassed ? VERIFICATION_PASSED:VERIFICATION_ERROR;
    setIcon(QIcon(iconPath));
}

void LineEditWithCheckIcon::initUI()
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setIconSize(QSize(16,16));
    setIconPosition(Kiran::ICON_POSITION_RIGHT);
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    connect(this, &QLineEdit::textChanged, [this](const QString &text) {
        ///密码框输入密码不为空的情况下调整字体和字间距
        if (echoMode() == QLineEdit::Password && text.isEmpty()) {
            setShowPasswordModeStyle(false);
            setNormalLetterSpacing();
        } else if (echoMode() == QLineEdit::Password && !text.isEmpty()) {
            setShowPasswordModeStyle(true);
            setPasswdLetterSpacing();
        }
    });
}

void LineEditWithCheckIcon::setNormalLetterSpacing()
{
    QFont currentFont = font();
    currentFont.setLetterSpacing(QFont::PercentageSpacing, 0);
    setFont(currentFont);
}

void LineEditWithCheckIcon::setPasswdLetterSpacing()
{
    QFont currentFont = font();
    currentFont.setLetterSpacing(QFont::AbsoluteSpacing, 4);
    setFont(currentFont);
}

void LineEditWithCheckIcon::setShowPasswordModeStyle(bool showPasswordModeStyle)
{
    if (m_showPasswordModeStyle == showPasswordModeStyle) {
        return;
    }
    m_showPasswordModeStyle = showPasswordModeStyle;
    style()->polish(this);
}

void LineEditWithCheckIcon::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && echoMode() == QLineEdit::Password) {
        clear();
    }
    KiranIconLineEdit::keyPressEvent(event);
}

void LineEditWithCheckIcon::focusInEvent(QFocusEvent *event)
{
    resetVerificationStatus();
    KiranIconLineEdit::focusInEvent(event);
}

void LineEditWithCheckIcon::resetVerificationStatus()
{
    m_verificationStatus = VERIFICATION_NONE;
    setIcon(QIcon());
}
