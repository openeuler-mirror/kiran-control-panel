#!/usr/bin/python3
# Copyright (c) 2026 KylinSec Co., Ltd.
# Author:     gaobo <gaobo@kylinsec.com.cn>
# Launch system-config-printer generic add wizard (init("printer")).
# Bootstrap matches system-config-printer.py main(), not newprinter.py __main__
# (the latter overwrites SYSTEM_CONFIG_PRINTER_UI to relative "ui" and enables debug).

import locale
import os
import sys

import gi
gi.require_version("Gdk", "3.0")
gi.require_version("Gtk", "3.0")
gi.require_version("GdkPixbuf", "2.0")

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
