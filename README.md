# kiran-timedate-manager

## 编译

1. 安装编译运行依赖

   ` sudo yum install gcc-c++ libX11 libX11-devel libXrandr libXrandr-devel qt5-qtbase qt5-qtbase-devel qt5-qtsvg qt5-qtsvg-devel gsettings-qt gsettings-qt-devel`

2. **源码根**目录下创建**build**目录`mkdir build`

3. 进入build目录，`qmake-qt5`,生成makefile

4. 执行**make**进行编译，生成可执行文件位于**build**下的**kiran-timedate-manager**

## 安装

1.在**build**目录下执行`sudo make install`

## 卸载

1.在**build**目录下执行`sudo make install`

## 运行

在控制面板中即可看到日期时间设置工具，也可执行kiran-timedate-manager