//
// Created by liuxinhao on 2022/6/16.
//

#ifndef KIRAN_CONTROL_PANEL_SEARCH_MODEL_H
#define KIRAN_CONTROL_PANEL_SEARCH_MODEL_H

#include <QStandardItemModel>

class SearchModel : public  QStandardItemModel
{
    Q_OBJECT
public:
    explicit SearchModel(QObject* parent = nullptr);
    ~SearchModel();

    static const int roleCategoryID;
    static const int roleSubItemID;
private:
    void init();
    void loadSearchModel();
    void appendItem(const QString& text,const QString& category,const QString& subItem);

private slots:
    void handlePluginVisibleSubItemChanged();

private:

};

#endif  // KIRAN_CONTROL_PANEL_SEARCH_MODEL_H
