# kiran-control-panel
# kiran桌面环境的控制面板

###### 编译
1.  安装编译依赖  
   `sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtx11extras qt5-qtx11extras-devel libX11 libX11-devel kiranwidgets-qt5`
2. **源码根目录kiran-control-panel**下创建**build**目录`mkdir build`
3. 进入**build**目录,执行`qmake-qt5 ..`生成**Makefile**
4. 执行`make`进行编译

###### 安装
1. 在**build**目录下执行`sudo make install`

###### 卸载
1. 在**build**目录下执行`sudo make uninstall`

###### 具体控件的使用
控制面板：   
[root@localhost ~]# /usr/bin/kiran-control-panel   
模块独立运行:   
[root@localhost ~]# /usr/bin/kiran-control-panel-module-runalone
# 模块开发接口文档
doc下"模块接口文档.doc"
# 模块demo
example目录下模块module01。
###### 模块demo 编译
1.  安装编译依赖  
   `sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtx11extras qt5-qtx11extras-devel libX11 libX11-devel kiranwidgets-qt5`
2. **源码根目录example/modeule01**下创建**build**目录`mkdir build`
3. 进入**build**目录,执行`qmake-qt5 ..`生成**Makefile**
4. 执行`make`进行编译

###### 模块demo安装
1. 在**build**目录下执行`sudo make install`

###### 模块demo卸载
1. 在**build**目录下执行`sudo make uninstall`

###### 模块开发注意事项：   
1、接口头文件名：`#include "kiran-control-panel/module-interface.h"`   
2、QList<SubItem> getSubitems() 接口返回值中SubItem结构体的icon字段，可以为图标的全路径，或只包含图标名称(将使用系统图标)。   
3、`getSubitemWidget是通过getSubitems给出的name创建窗口,所以这两处要一致.`   
4、 `getTranslationPath 只需返回全路径+翻译文件名，无需添加翻译文件后缀。   
	如：‘/usr/share/kiran-control-panel/module/kiran-display-tools/kiran-display-tools.*_*.qm’ 只需返回 ‘/usr/share/kiran-control-panel/module/kiran-display-tools/kiran-display-tools’即可，主程序将根据当前翻译类型选择合适的翻译文件。`

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
Icon=start-menu-set.svg     #可以为图标的全路径，或只包含图标名称(将使用系统图标)。   
Weight=1  
Category=Individuation      #`分类名称：Individuation 个性化，About Systems 关于系统，Regional Language 区域语言，Datetime 日期时间，Network 网络`
								#`Display 显示，Hardware 硬件，Account Management 账户管理，Power Management 电源管理，Login Settings 登录设置`   
PluginFile=/usr/lib/libkiran-start-menu-settings.so     #插件路径，主程序将从此路径加载插件。    

###### Desktop文件注意事项:
1、desktop文件中Weight字段用于排序权重，在同一个层级中不能出现重复 。  


