#ifndef NETWORKSUBITEM_H
#define NETWORKSUBITEM_H

#include <QObject>
#include "panel-interface.h"
#include "plugin-subitem-interface.h"

class CPanelNetworkWidget;
class NetworkSubItem : public QObject, public KiranControlPanel::PluginSubitemInterface
{
    Q_OBJECT
public:
    explicit NetworkSubItem(KiranControlPanel::PanelInterface* interface, QObject* parent = nullptr);
    ~NetworkSubItem();

    bool eventFilter(QObject* watched, QEvent* event) override;

public:
    // 功能项ID,用于区分功能项,应确保其唯一
    QString getID() override;

    // 功能项名称，用于显示在启动器标题栏之中
    QString getName() override;

    // 获取功能项分类ID，该功能项属于哪个分类
    QString getCategory() override;

    // 获取功能项目状态描述，显示在功能项侧边栏右边状态文本描述
    QString getDesc() override;

    // 获取功能项图标显示，用于形成功能项侧边栏的左侧图标
    QString getIcon() override;

    // 获取功能项权重，用于多个功能项排序
    int getWeight() override;

    // 创建显示控件
    QWidget* createWidget() override;

    // 获取自定义搜索关键词
    //  QVector< 显示文本(已翻译)，搜索跳转标识ID >
    QVector<QPair<QString, QString>> getSearchKeys() override;
    // 跳转至自定义搜索项
    bool jumpToSearchEntry(const QString& key) override;
    // 该功能项是否存在未保存配置
    bool haveUnSavedOptions() override;

private:
    void handleSubItemsChanged();
    QStringList subItemsList();

private:
    KiranControlPanel::PanelInterface* m_interface = nullptr;
    CPanelNetworkWidget* m_subitemWidget = nullptr;
    QStringList m_subItemsList;
};

#endif  // NETWORKSUBITEM_H
