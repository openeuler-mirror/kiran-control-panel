kiran-cpanel-mouse
- 编译

    安装编译依赖
    sudo yum install qt5-qtbase-devel  kiranwidgets-qt5-devel qt5-linguist kiran-cc-daemon-devel kiran-cc-daemon cmake zlog-devel
    源码根目录下创建build目录mkdir build
    进行build目录,执行cmake -DCMAKE_INSTALL_PREFIX=/usr .. 生成Makefile
    执行make进行编译，生成可执行文件位于build下的kiran-cpanel-mouse

- 安装
    在build目录下执行sudo make install

- 卸载
    在build目录下执行sudo make uninstall

- 运行
编译安装运行后执行kiran-cpanel-mouse

