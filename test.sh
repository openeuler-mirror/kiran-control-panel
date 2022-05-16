#!/bin/sh

cd `dirname $0`
shell_dir=`pwd`
build_dir=$shell_dir"/build"

#if [ -d $build_dir ]; then
#  rm -rf $build_dir
#fi
#
#mkdir $build_dir

cd $build_dir
cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=debug ..
#make -j4 -B
make -j4
echo 123123 |sudo -S make install

#gammaray /usr/local/bin/kiran-cpanel-launcher --cpanel-plugin kiran-cpanel-account -style=fusion
#/usr/local/bin/kiran-cpanel-launcher --cpanel-plugin kiran-cpanel-account -style=fusion

#gammaray /usr/local/bin/kiran-cpanel-launcher --cpanel-plugin kiran-cpanel-account
/usr/local/bin/kiran-cpanel-launcher --cpanel-plugin kiran-cpanel-account

#/usr/local/bin/kiran-control-panel