/**
 * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http: //www.gnu.org/licenses/>.
 */
#ifndef INTERFACE_H
#define INTERFACE_H
#include <kiran-control-panel/kcp-plugin-interface.h>

class QTranslator;
class KcpInterface: public QObject,public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    KcpInterface();
    ~KcpInterface(){};

public:
    /**
     * 插件需提供的初始化方法，在其中加载翻译文件或做其他初始化操作
     * \return 初始化返回值 返回0标志成功，其他值标志失败！
     */
    virtual int init() override;

    /**
     * 插件需提供取消初始化的操作，在其中对翻译文件进行卸载或取消其他初始化操作
     */
    virtual void uninit() override;

    /**
     * \brief 通过插件功能项名称(PluginSubItem->name)获取显示控件
     * \param id 功能项ID
     * \return 该功能项的显示控件
     */
    virtual QWidget* getSubItemWidget(QString subItemName) override;

    /**
     * 插件实现该方法用于判断是否存在未保存的设置项,用于提供切换页面时做检查
     * \return 是否存在未保存项
     */
    virtual bool haveUnsavedOptions()  override;

    QStringList visibleSubItems() override;

private:
    QWidget* m_currentWidget = nullptr;
    QTranslator* m_translator = nullptr;
};

#endif // INTERFACE_H
