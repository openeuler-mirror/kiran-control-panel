## 编译成APP
1.  安装编译依赖  
   `sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtbase-gui qt5-qtx11extras qt5-qtx11extras-devel qt5-qtsvg glibc glibc-devel libX11 libX11-devel kiranwidgets-qt5 kiranwidgets-qt5-devel`
2. **源码根目录**下创建**build**目录`mkdir build`  
3. 进行**build**目录,执行`cmake -DCMAKE_INSTALL_PREFIX=/usr ..`生成**Makefile**  
4. 执行`make`进行编译

## 编译成LIB
1.  安装编译依赖  
   `sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtbase-gui qt5-qtx11extras qt5-qtx11extras-devel qt5-qtsvg glibc glibc-devel libX11 libX11-devel kiran-control-center`
2. **源码根目录**下创建**build**目录`mkdir build`  
3. 进行**build**目录,执行`cmake -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_PLUGIN=on ..`生成**Makefile**.  
4. 执行`make`进行编译  

## 安装
1. 在**build**目录下执行`sudo make install`

## 卸载
1. 在**build**目录下执行`sudo make uninstall`

## app的使用
[root@localhost ~]# kiran-display-tools
