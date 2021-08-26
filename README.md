# kiran-account-manager
## 编译

- 安装编译依赖  
`sudo yum install kiranwidgets-qt5-devel qt5-qtbase-devel qt5-qtsvg-devel kiran-qdbusxml2cpp pam-devel libgcrypt-devel zeromq-devel`
- 在源码根目录下创建**build**目录  
`mkdir build`
- 生成**Makefile**  
`cmake3 -DCMAKE_INSTALL_PREFIX=/usr -DSHARE_INSTALL_PREFIX=/usr/share   ../`
- 编译  
`make -j4`

## 安装

`make install`

## 卸载

`make uninstall`

### 运行

`$ /usr/bin/kiran-account-manager`