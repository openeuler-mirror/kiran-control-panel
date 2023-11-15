#ifndef __HARDWARE_SUBITEM_H__
#define __HARDWARE_SUBITEM_H__

#include <QObject>

#include "pages/hardware-information/hardware-information.h"
#include "panel-interface.h"
#include "plugin-subitem-interface.h"

class HardwareSubItem : public QObject,
                        public KiranControlPanel::PluginSubitemInterface
{
    Q_OBJECT
public:
    HardwareSubItem(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
    ~HardwareSubItem()
    {
    }

public:
    // 功能项ID,用于区分功能项,应确保其唯一
    QString getID() override
    {
        return "HardwareInformation";
    }

    // 功能项名称，用于显示在启动器标题栏之中
    QString getName() override
    {
        return tr("Hardware Information");
    }

    // 获取功能项分类ID，该功能项属于哪个分类
    QString getCategory() override
    {
        return "about-system";
    }

    // 获取功能项目状态描述，显示在功能项侧边栏右边状态文本描述
    QString getDesc() override
    {
        return "";
    }

    // 获取功能项图标显示，用于形成功能项侧边栏的左侧图标
    QString getIcon() override
    {
        return "kcp-page-hardware-information";
    }

    // 获取功能项权重，用于多个功能项排序
    int getWeight() override
    {
        return 1;
    }

    // 创建显示控件
    QWidget* createWidget() override
    {
        return new HardwareInformation();
    }

    // 获取自定义搜索关键词
    //  QVector< 显示文本(已翻译)，搜索跳转标识ID >
    QVector<QPair<QString, QString>> getSearchKeys() override { return {}; };
    // 跳转至自定义搜索项
    bool jumpToSearchEntry(const QString& key) override { return false; };
    // 该功能项是否存在未保存配置
    bool haveUnSavedOptions() override { return false; };
};

#endif