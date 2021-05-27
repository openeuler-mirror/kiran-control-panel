#kiran控制中心(kiran-control-panel)
## 如何使用
### 编译
1.  安装编译依赖  
   `sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtbase-gui qt5-qtx11extras qt5-qtx11extras-devel qt5-qtsvg glibc glibc-devel libX11 libX11-devel kiranwidgets-qt5 kiranwidgets-qt5-devel`
2. **源码根目录下创建**build**目录`mkdir build`
3. 进入**build**目录,执行`cmake -DCMAKE_INSTALL_PREFIX=/usr ..`生成**Makefile**
4. 执行`make`进行编译
### 安装
### 运行
## 参与开发
### Category(主分类)
主分类位于控制面框显示的左侧列表,提供第一层大分类，插件可指定相应的主分类做为父节点,也可安装未提供的主分类到相应的目录下
#### 目前控制面板自带的主分类
以下是控制面板目前自带的分类，插件开发可根据需求自行选择相应的主分类进行插入
- about-systems    
  关于系统
- account-management    
  账户管理
- datetime    
  日期时间
- display    
  显示设置
- hardware    
  硬件相关
- individuation    
  个性化
- login-settings    
  登录设置
- network    
  网络相关
- power-manager    
  电源相关
- regional-language    
  区域语言
#### 主分类Desktop文件格式
```ini
[Desktop Entry]
#用作显示的主分类名，需提供翻译，控制中心根据当前语言环境选择合适的翻译形成左侧的列表
Name=About Systems
Name[zh_CN]=关于系统
#用作主分类节点的悬浮显示，控制中心根据当前语言环境选择合适的翻译形成左侧的列表节点悬浮提示框
Comment=About Systems
Comment[zh_CN]=关于系统
#用作主分类节点的图标，控制中心通过该图标形成左侧分类的图标，图标按规范放置的话可以为图标名，也可以为绝对路径
Icon=about-systems.png

#该组(Group)下均为提供给控制面板或其启动器的相关信息
[Kiran Control Panel Category]
#主分类的ID，插件需指定主分类的ID
Category=about-system
#主分类权重，控制面板将会根据该值的大小决定哪项在第一个
Weight=10
#主分类关键词,提供给控制面板用于检索主分类，与语言有关，需提供翻译
Keywords[zh_CN]=关于系统
Keywords=About Systems
```
#### 主分类相关位置
主分类相关位置存储于/usr/lib64/pkgconfig/kiran-control-panel.pc下
- 主分类信息描述Desktop文件路径(category_desktop_location)
- 主分类图标文件路径(category_icon_location)

### 插件(Plugin)和子功能项(SubItem)
####插件
#####插件Desktop文件格式
```ini
[Desktop Entry]
#插件名，启动器根据语言环境获取该值作为窗口标题，同时也做快捷方式的显示名
Name=Plugin Demo
Name[zh_CN]=插件demo
Comment=Plugin Demo
Comment[zh_CN]=插件demo
#插件图标,同时也是快捷方式图标
Icon=plugin-demo.svg
# 配置通过启动器单独执行，需传入插件的desktop文件名，启动器获取该dekstop文件位置加载相关信息 
# <kiran-cpanel-launcher -cpanel-plugin 插件的desktop文件名>
Exec=kiran-cpanel-launcher --cpanel-plugin=kiran-cpanel-demo
Categories=Settings;System;X-Common-Features;X-Common-Features;
Terminal=false
Type=Application
X-KIRAN-NoDisplay=true

#插件信息存储的组（Group)
[Kiran Control Panel Plugin]
#插件排序
Weight=3
#插件分类
Category=individuation
#插件共享库位置，若以'/'开头则为绝对路径，也可支持通过共享库名去插件安装目录去查找
Library=lib@PROJECT_NAME@.so
#功能子项，控制面板会根据该值对下列的功能子项Group进行解析
SubItems=Dialog001,Dialog002

#功能子项名
[Dialog001]
#功能子项名 该值提供给控制中心形成功能子项列表
Name[zh_CN]=弹窗001
#插件Icon
Icon=dialog001.svg
#插件关键字 该值提供给控制中心形成搜索列表
Keywords[zh_CN]=Dialog,Dialog001,001

[Dialog002]
Name[zh_CN]=弹窗002
Icon=dialog002.svg
Keywords[zh_CN]=Dialog,Dialog001,001
```
#####插件接口定义
```c++
//插件抽象接口
class KcpPluginInterface
{
public:
    virtual KcpPluginInterface};

public:
    /**
     * 插件需提供的初始化方法，在其中加载翻译文件或做其他初始化操作
     * \return 初始化返回值 返回0标志成功，其他值标志失败！
     */
    virtual int init() = 0;

    /**
     * 插件需提供取消初始化的操作，在其中对翻译文件进行卸载或取消其他初始化操作
     */
    virtual void uninit() = 0;

    /**
     * \brief 通过插件功能项名称(PluginSubItem->name)获取显示控件
     * \param id 功能项ID
     * \return 该功能项的显示控件
     */
    virtual QWidget* getSubItemWidget(QString id) = 0;

    /**
     * 插件实现该方法用于判断是否存在未保存的设置项,用于提供切换页面时做检查
     * \return 是否存在未保存项
     */
    virtual bool haveUnsavedOptions() = 0;
};

#define CPanelPluginInterface_iid "com.kylinsec.Kiran.ControlPanelInterface"
Q_DECLARE_INTERFACE(KcpPluginInterface,CPanelPluginInterface_iid)
```

###### 具体控件的使用
控制中心框架：   
[root@localhost ~]# /usr/bin/kiran-control-center   
# 模块开发接口文档
doc下"模块接口文档.doc"

###### 模块开发注意事项：   
1、接口头文件名：`#include "module-interface.h"`   
2、QList<SubItem> getSubitems() 接口返回值中SubItem结构体的icon字段，可以为图标的全路径，或只包含图标名称(将使用系统图标)。   
3、`getSubitemWidget是通过getSubitems给出的name创建窗口,所以这两处要一致.`   
4、 `getTranslationPath 只需返回全路径+翻译文件名，无需添加翻译文件后缀。   
	如：‘/usr/share/kiran-control-center/module/kiran-display-tools/kiran-display-tools.*_*.qm’ 只需返回 ‘/usr/share/kiran-control-center/module/kiran-display-tools/kiran-display-tools’即可，主程序将根据当前翻译类型选择合适的翻译文件。` 
5、pkgconfig的pc文件名kiran-control-center.pc

# Desktop文件
###### 分类描述文件(模块开发者无需关注)的格式如下:
[Desktop Entry]  
Name=Individuation  
Name[zh_CN]=个性化  
Comment=Individuation  
Comment[zh_CN]=个性化  
Icon=individuation.png  
Weight=1  
Keywords=个性化  
  
###### 模块描述文件示例如下:  
[Desktop Entry]  
Name=Start Menu Set  
Name[zh_CN]=开始菜单设置  
Comment=Start Menu Set  
Comment[zh_CN]=开始菜单设置  
Icon=start-menu-set.svg     #插件图标。可以为图标的全路径，或只包含图标名称(将使用系统图标)。   
Weight=1  
Category=Individuation      #`分类名称：Individuation 个性化，About Systems 关于系统，Regional Language 区域语言，Datetime 日期时间，Network 网络` 
								#`Display 显示，Hardware 硬件，Account Management 账户管理，Power Management 电源管理，Login Settings 登录设置`           
PluginFile=/usr/lib/libkiran-start-menu-settings.so     #插件路径，主程序将从此路径加载插件。    

###### Desktop文件注意事项:
1、desktop文件中Weight字段用于排序权重，在同一个层级中不能出现重复 。  

###### 模块desktop文件的安装目录
执行 `kiran-control-center --print-plugins-dir` 打印模块desktop文件的安装目录.   
eg:  
`[root@localhost ~]# /usr/bin/kiran-control-center --print-plugins-dir  
输出： /usr/share/kiran-control-center/plugins/desktop  //将插件(模块)的desktop文件安装到此处。控制面板主程序将从这里读取插件的desktop，根据desktop描述加载插件。  

# 模块demo
example目录下模块plugin-demo。

## 模块编译成APP
1.  安装编译依赖  
   `sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtbase-gui qt5-qtx11extras qt5-qtx11extras-devel qt5-qtsvg glibc glibc-devel glib2 glib2-devel libX11 libX11-devel kiranwidgets-qt5 kiranwidgets-qt5-devel`
2. **源码根目录**下创建**build**目录`mkdir build`   
3. 进行**build**目录,执行`cmake -DCMAKE_INSTALL_PREFIX=/usr ..`生成**Makefile**   
4. 执行`make`进行编译

## 模块编译成LIB
1.  安装编译依赖  
   `sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtbase-gui qt5-qtx11extras qt5-qtx11extras-devel qt5-qtsvg glibc glibc-devel glib2 glib2-devel libX11 libX11-devel kiran-control-center`
2. **源码根目录**下创建**build**目录`mkdir build`   
3. 进行**build**目录,执行`cmake -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_PLUGIN=on ..`生成**Makefile**.   
4. 执行`make`进行编译

## 模块安装
1. 在**build**目录下执行`sudo make install`

## 模块卸载
1. 在**build**目录下执行`sudo make uninstall`

## 模块作为app的使用
[root@localhost ~]# /usr/bin/plugin-demo

## 模块作为plugin的使用
插件编译安装后，运行控制中心程序，在控制中心程序中加载插件:    
[root@localhost ~]# /usr/bin/kiran-control-center

# kiran桌面环境的控制中心单独运行指定的插件
###### 控制中心框架编译
1.  安装编译依赖  
   `sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtbase-gui qt5-qtx11extras qt5-qtx11extras-devel qt5-qtsvg glibc glibc-devel libX11 libX11-devel kiranwidgets-qt5 kiranwidgets-qt5-devel`
2. **源码根目录下创建**build**目录`mkdir build`
3. 进入**build**目录,执行`cmake -DCMAKE_INSTALL_PREFIX=/usr -DSINGAL_PLUGIN=on ..`生成**Makefile**
4. 执行`make`进行编译

###### 安装
1. 在**build**目录下执行`sudo make install`

###### 卸载
1. 在**build**目录下执行`sudo make uninstall`

###### 具体控件的使用
[root@localhost ~]# /usr/bin/kiran-control-center-single-plugin -m-name='Plugin Demo'     `// -m-name指定插件名称，对应于插件(模块)的desktop文件中Name字段的内容。注意：控制中心在此编译下，不会生成插件接口头文件和pc文件。开发插件时请按照文章开头的方式编译控制中心。`   
