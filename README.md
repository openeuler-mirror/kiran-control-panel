kiran-cpanel-Keyboard

- 编译

    1、安装编译依赖 sudo yum install qt5-qtbase-devel kiranwidgets-qt5-devel qt5-linguist kiran-cc-daemon-devel kiran-cc-daemon cmake kiran-log-qt5-devel kiran-qdbusxml2cpp kiran-control-panel-devel
    2、源码根目录下创建build目录mkdir build 
	3、进行build目录,执行cmake -DCMAKE_INSTALL_PREFIX=/usr .. 生成Makefile 
	4、执行make进行编译
	5、生成添加进控制中心的库文件: libkiran-cpanel-keyboard.so

- 安装 在build目录下执行sudo make install

- 卸载 在build目录下执行sudo make uninstall

- 运行 编译安装运行后执行: kiran-cpanel-launcher --cpanel-plugin kiran-cpanel-keyboard


