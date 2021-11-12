kiran-cpanel-mouse
- 编译

    安装编译依赖
    sudo yum install qt5-qtbase-devel  kiran-widgets-qt5-devel qt5-linguist kiran-cc-daemon-devel  cmake kiran-log-qt5-devel kiran-control-panel-devel kiran-qdbusxml2cpp
    源码根目录下创建build目录mkdir build
    进行build目录,执行cmake -DCMAKE_INSTALL_PREFIX=/usr .. 生成Makefile
    执行make进行编译，生成控制中心插件libkiran-cpanel-mouse.so

- 安装
    在build目录下执行sudo make install

- 卸载
    在build目录下执行sudo make uninstall

- 运行
编译安装运行后执行kiran-cpanel-launcher --cpanel-plugin kiran-cpanel-mouse

