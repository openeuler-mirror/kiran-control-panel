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