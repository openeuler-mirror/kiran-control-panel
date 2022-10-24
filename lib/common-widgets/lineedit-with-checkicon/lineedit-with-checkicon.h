/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

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
    bool m_showPasswordModeStyle;
};

#endif  // LINEEDITWITHCHECKICON_H
