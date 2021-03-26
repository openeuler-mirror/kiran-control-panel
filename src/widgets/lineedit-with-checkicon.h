#ifndef LINEEDITWITHCHECKICON_H
#define LINEEDITWITHCHECKICON_H

#include <QWidget>
#include <QLineEdit>

namespace Ui
{
    class LineEditWithCheckIcon;
}

class LineEditWithCheckIcon : public QWidget
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
    explicit LineEditWithCheckIcon (QWidget *parent = nullptr);
    ~LineEditWithCheckIcon ();

    void setEchoMode (QLineEdit::EchoMode echoMode);
    void setMaxLength (int length);
    void setPlaceholderText (const QString &placeholder);

    void resetVerificationStatus ();
    void setVerificationStatus (bool isPassed);

    QString text ();

    void clear ();

private:
    void initUI ();
    ///@biref 设置普通字符间距
    void setNormalLetterSpacing ();
    ///@biref 设置密码字符间距
    void setPasswdLetterSpacing ();
    ///@biref 设置输入密码样式
    void setShowPasswordModeStyle (bool showPasswordModeStyle);

signals:
    void editTextChanged (const QString &text);

protected:
    bool eventFilter (QObject *obj, QEvent *event) override;
    virtual void paintEvent (QPaintEvent *event) override;

private:
    Ui::LineEditWithCheckIcon *ui;
    VerificationStatusEnum m_verificationStatus;
    bool m_showPasswordModeStyle;
};

#endif // LINEEDITWITHCHECKICON_H
