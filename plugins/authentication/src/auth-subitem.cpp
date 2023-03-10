#include "auth-subitem.h"

AuthSubItem::AuthSubItem(std::function<QWidget*(void)> func, QObject* parent)
    : m_createFunc(func),
      QObject(parent)
{
}

AuthSubItem ::~AuthSubItem()
{
}

// 功能项ID,用于区分功能项,应确保其唯一
void AuthSubItem::setID(const QString& subitemID)
{
    m_id = subitemID;
}

QString AuthSubItem::getID()
{
    return m_id;
}

// 功能项名称，用于显示在启动器标题栏之中
void AuthSubItem::setName(const QString& name)
{
    m_name = name;
}

QString AuthSubItem::getName()
{
    return m_name;
}

// 获取功能项分类ID，该功能项属于哪个分类
void AuthSubItem::setCategory(const QString& category)
{
    m_category = category;
}

QString AuthSubItem::getCategory()
{
    return m_category;
}

// 获取功能项目状态描述，显示在功能项侧边栏右边状态文本描述
void AuthSubItem::setDesc(const QString& desc)
{
    m_desc = desc;
}

QString AuthSubItem::getDesc()
{
    return m_desc;
}

// 获取功能项图标显示，用于形成功能项侧边栏的左侧图标
void AuthSubItem::setIcon(const QString& icon)
{
    m_icon = icon;
}

QString AuthSubItem::getIcon()
{
    return m_icon;
}

// 获取功能项权重，用于多个功能项排序
void AuthSubItem::setWeight(int weight)
{
    m_weight = weight;
}

int AuthSubItem::getWeight()
{
    return m_weight;
}

// 创建显示控件
QWidget* AuthSubItem::createWidget()
{
    return m_createFunc();
}