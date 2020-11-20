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
