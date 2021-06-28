//
// Created by lxh on 2020/11/10.
//

#ifndef KIRAN_ACCOUNT_MANAGER_ACCOUNT_VALIDATOR_H
#define KIRAN_ACCOUNT_MANAGER_ACCOUNT_VALIDATOR_H

#include <QValidator>

class UserNameValidator : public QValidator
{
    Q_OBJECT
public:
    explicit UserNameValidator(QObject *parent = nullptr);
    State validate(QString &input, int &pos) const Q_DECL_OVERRIDE;
};

#endif  //KIRAN_ACCOUNT_MANAGER_ACCOUNT_VALIDATOR_H
