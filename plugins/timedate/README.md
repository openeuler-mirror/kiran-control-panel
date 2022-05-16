# kiran-timedate-manager

## 编译

### 编译成可执行文件

1. 安装编译运行依赖
   ` sudo yum install gcc-c++ libX11 libX11-devel libXrandr libXrandr-devel qt5-qtbase qt5-qtbase-devel qt5-qtsvg qt5-qtsvg-devel gsettings-qt gsettings-qt-devel kiranwidgets-qt5`
   若需要编译成控制面板插件则需要安装控制面板，暂时控制面板未编包，需要git clone到本地，执行编译并安装
2. **源码根**目录下创建**build**目录`mkdir build`
3. 进入build目录，`cmake -DCMAKE_INSTALL_PREFIX=/usr`,生成makefile
4. 执行`make`进行编译，生成可执行文件位于**build**下的**kiran-timedate-manager**
5. 执行`make install`执行安装，安装可执行文件位于/usr/bin/kiran-timedate-manager

### 编译成控制面板插件

1. 安装编译运行依赖
   ` sudo yum install gcc-c++ libX11 libX11-devel libXrandr libXrandr-devel qt5-qtbase qt5-qtbase-devel qt5-qtsvg qt5-qtsvg-devel gsettings-qt gsettings-qt-devel kiranwidgets-qt5`
   若需要编译成控制面板插件则需要安装控制面板，暂时控制面板未编包，需要git clone到本地，执行编译并安装
2. 执行`/usr/bin/kiran-control-center --print-plugins-dir`获取插件desktop安装位置
3. 进入build目录，`cmake -DCMAKE_INSTALL_PREFIX=/usr -D<上一步获取的控制中心输出的desktop安装位置>`,生成makefile
4. 执行`make`进行编译，生成插件位于**build**下的**libkiranwidgets-qt5.so**
5. 执行`make install`进行编译安装，安装插件位于系统共享库目录，安装插件desktop文件位于控制中心所输出的目录
6. 执行/usr/bin/kiran-control-center,即可看到控制中心加载插件