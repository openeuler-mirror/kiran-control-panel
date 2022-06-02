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
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#ifndef KIRANSEARCHLINEEDIT_H
#define KIRANSEARCHLINEEDIT_H

#include <QLineEdit>

class QListView;
class KiranSearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit KiranSearchLineEdit(const QStringList &completerKeys, QWidget *parent = nullptr);

private:
    void init();
    void initUI();
    void initConnection();
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void sigSearchConfirmed(const QString &reg);

private slots:
    void handleTextEdited(QString text);
    void handleEditingFinished();

private:
    QStringList m_completerKeys;
    QListView *m_listView;
};

#endif // KIRANSEARCHLINEEDIT_H
