#是否构建控制中心示例
OPTION(BUILD_EXAMPLE "Whether to build a control center plugin example" OFF)
#是否构建为服务器版本
OPTION(SERVER_MODE "Whether to build a server version" OFF)
#账户信息页面，密码过期策略是否显示
option(PASSWD_EXPIRATION_POLICY_VISIBLE "Is password expiration policy visible" OFF)
OPTION(ENABLE_USER_GROUP "Enable user group" OFF)
OPTION(ENABLE_NETWORK "Enable network plugin" ON)
OPTION(ENABLE_AUDIO "Enable audio plugin" ON)
OPTION(ENABLE_APPLICATION "Enable application plugin" ON)
#个性化，是否显示屏保界面
OPTION(ENABLE_APPEARANCE_SCREENSAVER "Whether to enable the personalized screensaver page" OFF)
#系统信息，是否显示隐私协议
OPTION(ENABLE_SYSTEM_PRIVACY_POLICY "Whether to show the system privacy policy" ON)
#系统信息，是否显示跳转系统监视器按钮
OPTION(ENABLE_SYSTEM_MONITOR "Whether to show the system monitor jump button" OFF)
#硬件信息页，是否显示磁盘管理入口
OPTION(ENABLE_DISK_MANAGEMENT "Whether to show disk management entry in hardware info page" OFF)