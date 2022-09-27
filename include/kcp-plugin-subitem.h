#pragma once
#include <QList>
#include <QSharedPointer>
#include <QString>

class QWidget;

/// @brief 控制中心新版本功能项接口
class KcpPluginSubItem
{
public:
    virtual QString getID() = 0;
    virtual QString getName() = 0;
    virtual QString getCategory() = 0;
    virtual QString getDesc() = 0;
    virtual QString getIcon() = 0;
    virtual int getWeight() = 0;
    /// @brief 获取自定义搜索关键词
    /// @return QVector< 显示文本(已翻译)，搜索跳转标识ID >
    virtual QVector<QPair<QString, QString>> getSearchKeys() = 0;

    virtual QWidget* createWidget() = 0;
    virtual bool jumpToSearchEntry(const QString& key) = 0;
    virtual bool haveUnSavedOptions() = 0;
};
typedef QSharedPointer<KcpPluginSubItem> KcpPluginSubItemPtr;