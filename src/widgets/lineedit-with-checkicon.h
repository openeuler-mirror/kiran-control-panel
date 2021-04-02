#ifndef LINEEDITWITHCHECKICON_H
#define LINEEDITWITHCHECKICON_H

#include <kiran-icon-line-edit.h>
#include <QWidget>

class LineEditWithCheckIcon : public KiranIconLineEdit
{
    Q_OBJECT

public:
    enum VerificationStatusEnum
    {
        VERIFICATION_PASSED,
        VERIFICATION_ERROR,
        VERIFICATION_NONE
    };
    Q_ENUM(VerificationStatusEnum)

public:
    explicit LineEditWithCheckIcon(QWidget *parent = nullptr);
    ~LineEditWithCheckIcon();

    void resetVerificationStatus();
    void setVerificationStatus(bool isPassed);

private:
    void initUI();
    ///@biref 设置普通字符间距
    void setNormalLetterSpacing();
    ///@biref 设置密码字符间距
    void setPasswdLetterSpacing();
    ///@biref 设置输入密码样式
    void setShowPasswordModeStyle(bool showPasswordModeStyle);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;

private:
    VerificationStatusEnum m_verificationStatus;
    bool                   m_showPasswordModeStyle;
};

#endif  // LINEEDITWITHCHECKICON_H
