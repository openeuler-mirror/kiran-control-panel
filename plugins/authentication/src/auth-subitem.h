#pragma once

#include <QObject>
#include <functional>
#include <QVector>
#include "panel-interface.h"
#include "plugin-subitem-interface.h"

class AuthSubItem : public QObject,
                    public KiranControlPanel::PluginSubitemInterface
{
    Q_OBJECT
public:
    AuthSubItem(std::function<QWidget*(void)> createWidgetFunc, QObject* parent = nullptr);
    ~AuthSubItem();

public:
    // 功能项ID,用于区分功能项,应确保其唯一
    void setID(const QString& subitemID);
    QString getID() override;

    // 功能项名称，用于显示在启动器标题栏之中
    void setName(const QString& name);
    QString getName() override;

    // 获取功能项分类ID，该功能项属于哪个分类
    void setCategory(const QString& category);
    QString getCategory() override;

    // 获取功能项目状态描述，显示在功能项侧边栏右边状态文本描述
    void setDesc(const QString& desc);
    QString getDesc() override;

    // 获取功能项图标显示，用于形成功能项侧边栏的左侧图标
    void setIcon(const QString& icon);
    QString getIcon() override;

    // 获取功能项权重，用于多个功能项排序
    void setWeight(int weight);
    int getWeight() override;

    // 创建显示控件
    QWidget* createWidget() override;

    // 获取自定义搜索关键词
    //  QVector< 显示文本(已翻译)，搜索跳转标识ID >
    QVector<QPair<QString, QString>> getSearchKeys() override { return {}; };
    // 跳转至自定义搜索项
    bool jumpToSearchEntry(const QString& key) override { return false; };
    // 该功能项是否存在未保存配置
    bool haveUnSavedOptions() override { return false; };

private:
    QString m_id;
    QString m_name;
    QString m_category;
    QString m_desc;
    QString m_icon;
    int m_weight;
    std::function<QWidget*(void)> m_createFunc = nullptr;
};