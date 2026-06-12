# kiran-control-panel

## Usage

### Build

1. Install build dependencies.

   ```bash
   $ sudo yum install gcc-c++ qt5-qtbase qt5-qtbase-devel qt5-qtbase-gui qt5-qtx11extras qt5-qtx11extras-devel qt5-qtsvg glibc glibc-devel libX11 libX11-devel kiranwidgets-qt5 kiran-widgets-qt5-devel qt5-qtbase-static qt5-qtbase-private-devel group-service libxkbcommon-devel
   ```

2. Create and navigate to the `build` directory at the root of the source tree.

   ```bash
   $ mkdir build
   ```

3. Generate the Makefiles for a debug build.

   ```bash
   $ cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=debug ..
   ```

4. Run `make` to compile the project.

   ```bash
   $ make -j4
   ```

### Installation

After compilation, run the following command in the `build` directory to install the project:

```bash
$ sudo make install
```

### Execution

- Launch the main control panel (load all plugins):

```bash
$ kiran-control-panel
```

- Launch the launcher (load specific plugins):

```bash
$ kiran-cpanel-launcher --cpanel-plugin kiran-cpanel-xxx
```

## Categories, Plugins, and Subitems

### Category

A category refers to a configuration file that populates the functional list on the leftmost pane of the control panel. It serves as a parent node for plugins (every plugin must point to a specific category). Currently, category configuration files are located in `/usr/share/kiran-control-panel/category/desktop/`. (Recommended: Retrieve the path dynamically from the `pkgconfig` file provided by `kiran-control-panel-devel`).

The category configuration file format is as follows:

```ini
[Desktop Entry]
# The primary category name displayed in the UI (requiring localization). The control panel selects the appropriate locale-specific string to populate the left-hand pane.
Name=About Systems
Name[zh_CN]=关于系统
# The tooltip text displayed when hovering over the primary category node. The control panel selects the appropriate string based on the current locale.
Comment=About Systems
Comment[zh_CN]=关于系统
# The icon for the primary category node, displayed on the left-hand side pane. This can be an icon name (if a standard icon path is followed) or an absolute path.
Icon=about-systems.png

# This group contains metadata provided specifically for the control panel or its launcher.
[Kiran Control Panel Category]
# The unique identifier (ID) of the primary category. Plugins must specify this ID to register under it.
Category=about-system
# The sorting weight of the primary category. The control panel determines the display order based on this value (higher/lower values dictate the priority).
Weight=10
# Search keywords for the primary category, used by the control panel's search feature. These are locale-dependent and require localization.
Keywords[zh_CN]=关于系统
Keywords=About Systems
```

The pre-installed default categories currently provided by the control panel are as follows:

- about-system
- account-management
- timedate
- display
- hardware
- individuation
- login-settings
- network
- power-manager
- regional-language

### Plugin

A plugin is a shared library that implements the control panel interface. A complete plugin must provide at least a desktop file and the compiled shared library implementation.

A single plugin can offer multiple subitems under a specific category. If the total number of subitems across all plugins within a given category is less than or equal to 1, the subitem list pane will be hidden automatically.

```ini
[Desktop Entry]
# Plugin name. The launcher retrieves this value based on the current locale to use it as the window title and the shortcut display name.
Name=Plugin Demo
Name[zh_CN]=插件demo
Comment=Plugin Demo
Comment[zh_CN]=插件demo
# Plugin icon, also used as the shortcut icon.
Icon=plugin-demo.svg
# Command to execute the plugin independently via the launcher. It requires the plugin's desktop file name, which the launcher uses to locate and load the relevant metadata.
# <kiran-cpanel-launcher -cpanel-plugin <plugin-desktop-filename>>
Exec=kiran-cpanel-launcher --cpanel-plugin=kiran-cpanel-demo
Categories=Settings;System;X-Common-Features;X-Common-Features;
Terminal=false
Type=Application
X-KIRAN-NoDisplay=true

# Group that stores metadata specific to the control panel plugin.
[Kiran Control Panel Plugin]
# Sorting weight of the plugin, determining its display priority.
Weight=3
# Category to which this plugin belongs.
Category=individuation
# Path to the plugin's shared library. If it starts with '/', it is treated as an absolute path; otherwise, the control panel searches for the library by name in the plugin installation directory.
Library=lib@PROJECT_NAME@.so
# List of functional subitems. The control panel uses these IDs to parse the corresponding subitem groups defined below.
SubItems=Dialog001,Dialog002

# Configuration group for a functional subitem.
[Dialog001]
# Subitem name, used by the control panel to populate the subitem list.
Name[zh_CN]=弹窗001
# Subitem icon.
Icon=dialog001.svg
# Search keywords for the subitem, used by the control panel to index and generate the search results list.
Keywords[zh_CN]=Dialog,Dialog001,001

[Dialog002]
Name[zh_CN]=弹窗002
Icon=dialog002.svg
Keywords[zh_CN]=Dialog,Dialog001,001
```

### Subitem

A subitem describes an individual functional entry provided by a plugin. The final list of subitems under a single category can be aggregated and displayed from multiple different plugins.

Subitem metadata is stored directly within the plugin's desktop file, making it readily accessible for global search indexing.

## How to Create a Plugin

### 1. Configuring the Plugin's Desktop Metadata File

```ini
[Desktop Entry]
# Plugin name. The launcher retrieves this value based on the current locale to use it as the window title and the shortcut display name.
Name=Plugin Demo
Name[zh_CN]=插件demo
Comment=Plugin Demo
Comment[zh_CN]=插件demo
# Plugin icon, also used as the shortcut icon.
Icon=plugin-demo.svg
# Command to execute the plugin independently via the launcher. It requires the plugin's desktop file name, which the launcher uses to locate and load the relevant metadata.
# <kiran-cpanel-launcher -cpanel-plugin <plugin-desktop-filename>>
Exec=kiran-cpanel-launcher --cpanel-plugin=kiran-cpanel-demo
Categories=Settings;System;X-Common-Features;X-Common-Features;
Terminal=false
Type=Application
X-KIRAN-NoDisplay=true

# Group that stores metadata specific to the control panel plugin.
[Kiran Control Panel Plugin]
# Sorting weight of the plugin.
Weight=3
# Category to which this plugin belongs.
Category=individuation
# Path to the plugin's shared library. If it starts with '/', it is treated as an absolute path; otherwise, the control panel searches for the library by name in the plugin installation directory.
Library=lib@PROJECT_NAME@.so
# List of functional subitems. The control panel uses these IDs to parse the corresponding subitem groups defined below
SubItems=SubItem1

# Configuration group for a functional subitem.
[SubItem1]
# Subitem name, used by the control panel to populate the subitem list.
Name[zh_CN]=SubItem1
#插件Icon
Icon=kcp-demo-item1
# Search keywords for the subitem, used by the control panel to index and generate the search results list.
Keywords[zh_CN]=SubItem1
```

### 2. Implementing the Control Panel Plugin Interface

Tip: The plugin and the control panel (including the launcher) binaries must be compiled using matching interface versions. The interface version is defined by `KcpPluginInterface_iid` in the `kcp-plugin-interface.h` header file. The control panel and launcher will refuse to load any plugin with a mismatched interface version.

**Interface definition:**

```c++
#ifndef KIRAN_CONTROL_PANEL_INCLUDE_KCP_PLUGIN_INTERFACE_H_
#define KIRAN_CONTROL_PANEL_INCLUDE_KCP_PLUGIN_INTERFACE_H_

#include <QString>
#include <QWidget>

// Abstract interface for control panel plugins
class KcpPluginInterface
{
public:
virtual ~KcpPluginInterface(){};
public:
/**
* Initialization method provided by the plugin. Use this to load localization files or perform other initialization operations.
* \return Initialization status code. 0 indicates success; any other value indicates failure.
*/
virtual int init() = 0;

/**
* Uninitialization method provided by the plugin. Use this to unload localization files or release other initialized resources.
*/
virtual void uninit() = 0;

/**
* \brief Retrieve the display widget associated with a specific functional subitem ID.
* \param id Subitem ID.
* \return Display widget associated with the subitem.
*/
virtual QWidget* getSubItemWidget(QString id) = 0;

/**
* Check whether the plugin contains any unsaved settings. The control panel invokes this to prompt the user or handle dirty states when switching pages.
* \return Whether there are any unsaved options.
*/
virtual bool haveUnsavedOptions() = 0;

/**
* Retrieve the list of subitems that should currently be visible.
* \param id Subitem ID.
* \return IDs of visible subitems.
*/
virtual QStringList visibleSubItems() = 0;
};

#define KcpPluginInterface_iid "com.kylinsec.Kiran.ControlPanelInterface/1.0"
Q_DECLARE_INTERFACE(KcpPluginInterface,KcpPluginInterface_iid)

#endif  //KIRAN_CONTROL_PANEL_INCLUDE_KCP_PLUGIN_INTERFACE_H_
```

Interface implementation example:

```c++
#ifndef INTERFACE_H
#define INTERFACE_H

#include <kiran-control-panel/kcp-plugin-interface.h>

class PluginDemoInterface : public QObject,public KcpPluginInterface
{
    // Define Q_OBJECT to enable the moc (Meta-Object Compiler) build process
    Q_OBJECT
    // Define plugin metadata and declare the interface identifier (IID) implemented by this class.
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    // Declare which interfaces this Qt class implements.
    Q_INTERFACES(KcpPluginInterface)
public:
    ~PluginDemoInterface(){};
    int init() override;
    void uninit() override;

    QWidget* getSubItemWidget(QString subItemName) override;
    bool haveUnsavedOptions() override;
    QStringList visibleSubItems() override;
};
#endif  // INTERFACE_H
```

### 3. Locating Plugin Installation Paths Using the pkg-config File

```cmake
# Retrieve the installation paths for the plugin desktop files and shared libraries via the pkg-config configuration file provided by the kiran-control-panel-devel package.
find_package(PkgConfig REQUIRED)
pkg_search_module(KIRAN_CONTROL_PANEL_PKG REQUIRED kiran-control-panel)
pkg_get_variable(CPANEL_PLUGIN_DIR kiran-control-panel plugin_location)
pkg_get_variable(CPANEL_DESKTOP_DIR kiran-control-panel plugin_desktop_location)
```

### 4. Running via Control Panel or Launcher

Launch the control panel:

```bash
$ kiran-control-panel
```

Launch via the launcher:

```bash
$ kiran-cpanel-launcher --cpanel-plugin <plugin-desktop-filename>
```
