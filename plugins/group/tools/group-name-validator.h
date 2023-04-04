/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-group is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     wangshichang <shichang@isrc.iscas.ac.cn>
 */
#ifndef KIRAN_GROUP_NAME_VALIDATOR_H
#define KIRAN_GROUP_NAME_VALIDATOR_H

#include<QValidator>

class GroupNameValidator : public QValidator
{
    Q_OBJECT
public:
    explicit GroupNameValidator(QObject *parent = nullptr);
    State validate(QString &input, int &pos) const Q_DECL_OVERRIDE;
};

#endif  //KIRAN_GROUP_NAME_VALIDATOR_H