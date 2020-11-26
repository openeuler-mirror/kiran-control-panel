# kiran-control-panel
kiran桌面环境的控制面板

## 编译
1.  安装编译依赖  
   `sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtx11extras qt5-qtx11extras-devel libX11 libX11-devel kiranwidgets-qt5`
2. **源码根目录**下创建**build**目录`mkdir build`
3. 进行**build**目录,执行`qmake-qt5 ..`生成**Makefile**
4. 执行`make`进行编译

## 安装
1. 在**build**目录下执行`sudo make install`

## 卸载
1. 在**build**目录下执行`sudo make uninstall`

## 具体控件的使用
控制面板：
[root@localhost ~]# /usr/bin/kiran-control-panel
模块独立运行:
[root@localhost ~]# /usr/bin/kiran-control-panel-model-runalone
## 模块接口文档
doc下"模块接口文档.doc"
## 模块demo
example目录下模块model01。  
注意事项：   
1、接口头文件名："kiran-control-panel-module-interface.h"   
2,getSubitemWidget是通过getSubitems给出的name创建窗口,所以这两处要对应.   
3、getSubitems返回的nameList,iconList,keyList中的元素个数须保证一样。keyList中的元素可以存在空字符。   

## Desktop文件
分类描述文件的格式如下:
[Desktop Entry]  
Name=accounts  
Name[zh_CN]=账户设置  
Comment=manage user accounts  
Comment[zh_CN]=管理用户账户  
Icon=avatar-default  
Keywords=账户，用户，帐号  
Weight=2  
  
模块描述文件示例如下:  
[Desktop Entry]  
Name=Fonts  
Name[zh_CN]=字体  
Comment=font settings  
Comment[zh_CN]=系统字体设置  
Icon=fonts  
Weight=1  
Category=accounts      #Category对应分类描述文件中的Name字段。  
PluginFile=/usr/share/kiran-control-center/plugins/libaccounts.so  

## 注意事项:
1,模块的desktop文件中模块的Weight权重在同一个分类中不能相同.  
2,分类的desktop文件存放的目录 "/usr/share/applications/kiran-control-panel/class"  
  模块的desktop文件存放的目录  "/usr/share/applications/kiran-control-panel/module"  
  图标存放的目录 "/usr/share/icons/Kiran/emblems/scalable/"  


