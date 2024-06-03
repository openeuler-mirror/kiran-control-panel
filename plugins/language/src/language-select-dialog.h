/**
 * Copyright (c) 2022 ~ 2023 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */
#ifndef __LANGUAGE_SELECT_DIALOG_H__
#define __LANGUAGE_SELECT_DIALOG_H__

#include <QDialog>
#include <QWidget>

namespace Ui
{
    class LanguageSelectDialog;
}

class QStandardItemModel;
class KiranSearchBox;
class QCompleter;
class SearchDelegate;
class LanguageSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LanguageSelectDialog(QWidget* parent = nullptr);
    ~LanguageSelectDialog();
    void initCompleter();
    void addLanguageItem(const QString &languageName, const QString &locale);

signals:
    void languageAddBtnClicked(QString locale);

private:
    Ui::LanguageSelectDialog* ui = nullptr;
    KiranSearchBox* m_kiranSearchBox = nullptr;

    QStandardItemModel* model = nullptr;
    QCompleter* m_completer = nullptr;
    SearchDelegate* m_searchDelegate = nullptr;
};

#endif