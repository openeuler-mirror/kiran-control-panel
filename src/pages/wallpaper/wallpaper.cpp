#include "wallpaper.h"
#include "ui_wallpaper.h"
/*
背景设置设计思路
一、总体：
分两个页面：主题设置主界面(Wallpaper)、壁纸选择界面(ImageSelector)（多线程创建，参考/home/yuanxing/kiranwidgets-qt5/src/widgets/kiran-image-selector/kiran-image-load-manager.cpp代码）
关联：用户点击选择壁纸widget，切换到壁纸选择界面
     用户点击壁纸，返回主界面，并且根据选择的壁纸类型（桌面/锁屏），更新预览图中的背景

前期工作：
枚举定义两种壁纸类型，供参数传递
1、创建壁纸预览控件类，分为桌面壁纸预览和锁屏壁纸预览两类，每个类包括预览图片以及预览类型名字
2、创建壁纸选择btn，提供setIcon、getIconPath接口，以及根据按钮的鼠标选择、悬浮状态，改变样式，
        其中鼠标悬浮时，在自定义图片btn中添加删除btn，删除btn分三种状态：选中、悬浮、普通

二、主界面
1、获取当前的桌面壁纸path、锁屏壁纸path
2、创建两个预览控件，将当前的path对应的图片绘至预览控件，当做背景
3、将预览控件添加至主界面
4、创建壁纸选择控件，创建两个widget，监听clicked信号，（信号中传入壁纸类型）跳转至壁纸选择界面，同时根据当前的壁纸名字，设置对应的壁纸btn的 icon

三、壁纸选择界面
1、创建一个壁纸选择控件（QWidget），放到scrollArea里面
2、在壁纸选择控件中添加FlowLayout
3、加载系统中的壁纸图片，获取绝对路径，加载用户目录下的.config/kylinsec/background.xml文件（先判断有没有，只有在用户添加了自己的图片进去才会有该文件，若有的话，就加载）
4、创建自定义的btn类，设置btn的icon
5、将btn添加进btnGroup
6、添加+btn
7、若点击+btn，则弹出文件管理器，供用户选择，选择后，将对应的图片添加进*******路径，并创建壁纸btn，添加进btnGroup
                               确保+btn在界面的最后
8、若点击删除按钮，则弹出删除确认消息框，用户点击确认后，删除图片（删除图片文件，删除btn）,如果删除的图片刚好是当前设置的壁纸，则×××××××
9、若选中壁纸，则获取壁纸btn的path，以及壁纸名字，发送设置是否成功信号，传入path，返回主界面
10、主界面壁纸设置监听信号，成功后，更新选择控件的name，根据传来的path，与壁纸类型，更新对应预览控件背景


问题：
1、获取和设置锁屏壁纸的接口！(后续会添加锁屏壁纸设置接口)
2、从哪里获取壁纸图片(系统路径和用户自定义的图片存储的路径（～/.config/kylinsec/background.xml）)
3、添加或删除壁纸图片，要进行什么操作，比如将图片从哪里删除了，或者添加到了哪里 (只能删除用户自添加的图片，更新～/.config/kylinsec/background.xml文件)
4、如果删除的刚好是正设置的壁纸，要默认选择哪个壁纸返回（默认default.jpg）
5、如果只是添加或删除壁纸，那么界面如何返回，是否要加返回按钮（直接点击左侧列表中的背景设置）
*/

#include "dbus-interface/appearance-global-info.h"
#include "widget/preview-label.h"

Wallpaper::Wallpaper(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Wallpaper)
{
    ui->setupUi(this);
    initUI();
}

Wallpaper::~Wallpaper()
{
    delete ui;
}

void Wallpaper::initUI()
{
    //TODO:后续会改接口：获取桌面壁纸和锁屏壁纸
    m_currDesktopBgPath = AppearanceGlobalInfo::instance()->getDesktopBackground();
    m_currLockScreenBgPath = AppearanceGlobalInfo::instance()->getLockScreenBackground();

    QLayout *layout_desktop = ui->widget_desktop_preview->layout();
    PreviewLabel *desktopPreview =  new PreviewLabel(DESKTOP,m_currDesktopBgPath,this);
    layout_desktop->addWidget(desktopPreview);
    layout_desktop->setAlignment(desktopPreview,Qt::AlignHCenter);

    QLayout *layout = ui->widget_lockscreen_preview->layout();
    PreviewLabel *lockScreenPreview = new PreviewLabel(SCREENSAVER,m_currLockScreenBgPath,this);
    layout->addWidget(lockScreenPreview);
    layout->setAlignment(lockScreenPreview,Qt::AlignHCenter);
}














