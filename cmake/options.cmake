#是否构建控制中心示例
OPTION(BUILD_EXAMPLE "Whether to build a control center plugin example" OFF)
#账户信息页面，密码过期策略是否显示
option(PASSWD_EXPIRATION_POLICY_VISIBLE "Is password expiration policy visible" OFF)
OPTION(ENABLE_USER_GROUP "Enable user group" OFF)
OPTION(ENABLE_NETWORK "Enable network plugin" ON)
OPTION(ENABLE_AUDIO "Enable audio plugin" ON)
OPTION(ENABLE_APPLICATION "Enable application plugin" ON)