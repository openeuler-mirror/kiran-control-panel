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

/usr/local/bin/kiran-control-panel
exit 0

ui_debug=0
specify_style=""
run_plugin_name="kiran-cpanel-keyboard"

cmd=""

if [ "$ui_debug" -gt "0" ]; then
    cmd="gammaray"
fi

cmd=$cmd" /usr/local/bin/kiran-cpanel-launcher"

if [ -n "$specify_style" ]; then
  cmd=$cmd" -style="$specify_style
fi

cmd=$cmd" --cpanel-plugin "$run_plugin_name

echo "cmd:$cmd"
$cmd

