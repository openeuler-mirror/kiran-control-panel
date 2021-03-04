# kiran-control-center
# kiran桌面环境的控制中心

###### 控制中心框架编译
1.  安装编译依赖  
   `sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtbase-gui qt5-qtx11extras qt5-qtx11extras-devel qt5-qtsvg glibc glibc-devel libX11 libX11-devel kiranwidgets-qt5 kiranwidgets-qt5-devel`
2. **源码根目录下创建**build**目录`mkdir build`
3. 进入**build**目录,执行`cmake -DCMAKE_INSTALL_PREFIX=/usr ..`生成**Makefile**
4. 执行`make`进行编译

###### 安装
1. 在**build**目录下执行`sudo make install`

###### 卸载
1. 在**build**目录下执行`sudo make uninstall`

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
[root@localhost ~]# /usr/bin/kiran-control-center-single-plugin -m-name=plugin-demo     `// -m-name指定插件名称，对应于插件(模块)的desktop文件中Name字段的内容。注意：控制中心在此编译下，不会生成插件接口头文件和pc文件。开发插件时请按照文章开头的方式编译控制中心。`   
