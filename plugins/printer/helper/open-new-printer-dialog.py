#!/usr/bin/python3
# -*- coding: utf-8 -*-
# Copyright (c) 2026 KylinSec Co., Ltd.
# Author:     gaobo <gaobo@kylinsec.com.cn>
# Launch system-config-printer generic add wizard (init("printer")).
# Bootstrap matches system-config-printer.py main(), not newprinter.py __main__
# (the latter overwrites SYSTEM_CONFIG_PRINTER_UI to relative "ui" and enables debug).
#
# 本脚本 import system-config-printer 的模块（newprinter/config/ppdippstr）与 gi/cups/dbus，
# 必须与 SCP 用同一个 python 大版本运行（SCP 有 py3 的 1.5.x 与 py2 的更老版本）。下面的自举读
# SCP 自带脚本的 shebang，若与本脚本解释器不同就换成该解释器重新执行自己；因此不需要打包期传参，
# 也不必单独声明 python*-cups/gobject/dbus（这些随 system-config-printer 一起引入）。
# 本文件保持 py2/py3 都能解析的语法。

import os
import sys

_SCP_NEWPRINTER = "/usr/share/system-config-printer/newprinter.py"


def bootstrap_interpreter():
    """把自身切到 system-config-printer 使用的解释器上"""
    if os.environ.get("KCP_HELPER_BOOTSTRAPPED"):
        return
    try:
        fp = open(_SCP_NEWPRINTER, "rb")
        try:
            line = fp.readline().decode("utf-8", "replace").strip()
        finally:
            fp.close()
    except (IOError, OSError):
        return
    if not line.startswith("#!"):
        return
    parts = line[2:].split()
    if not parts:
        return
    exe = parts[0]
    if not exe or os.path.realpath(exe) == os.path.realpath(sys.executable):
        return
    env = os.environ.copy()
    env["KCP_HELPER_BOOTSTRAPPED"] = "1"
    try:
        os.execve(exe, [exe, os.path.abspath(__file__)] + sys.argv[1:], env)
    except OSError:
        pass


bootstrap_interpreter()

import locale  # noqa: E402

import gi  # noqa: E402

try:
    gi.require_version("Gdk", "3.0")
    gi.require_version("Gtk", "3.0")
    gi.require_version("GdkPixbuf", "2.0")
except (AttributeError, ValueError):
    # 老 pygobject（python2 版）没有 gi.require_version，或版本号不同
    pass

from gi.repository import Gdk  # noqa: E402
from gi.repository import Gtk  # noqa: E402

Gtk.init(sys.argv)

SCP_PKGDATA = os.environ.get("SYSTEM_CONFIG_PRINTER_HOME", "/usr/share/system-config-printer")
sys.path.insert(0, SCP_PKGDATA)

import config  # noqa: E402
import cups  # noqa: E402

cups.ppdSetConformance(cups.PPD_CONFORM_RELAXED)

try:
    locale.setlocale(locale.LC_ALL, "")
except locale.Error:
    os.environ["LC_ALL"] = "C"
    locale.setlocale(locale.LC_ALL, "")
try:
    locale.bind_textdomain_codeset("system-config-printer", "UTF-8")
except (locale.Error, AttributeError):
    pass

import gettext  # noqa: E402
gettext.install(domain=config.PACKAGE, localedir=config.localedir)

import ppdippstr  # noqa: E402
ppdippstr.init()

Gdk.threads_init()
from dbus.glib import DBusGMainLoop  # noqa: E402
DBusGMainLoop(set_as_default=True)

import newprinter  # noqa: E402


def _quit(*_args):
    Gtk.main_quit()


def main():
    gui = newprinter.NewPrinterGUI()
    gui.connect("printer-added", _quit)
    gui.connect("printer-modified", _quit)
    gui.connect("dialog-canceled", _quit)
    if not gui.init("printer"):
        return 1
    Gdk.threads_enter()
    try:
        Gtk.main()
    finally:
        Gdk.threads_leave()
    return 0


if __name__ == "__main__":
    sys.exit(main())
