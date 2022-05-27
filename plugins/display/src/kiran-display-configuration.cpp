/**
* Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
* kiran-cpanel-display is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*
* Author:     yangxiaoqing <yangxiaoqing@kylinos.com.cn>
*/

#include "kiran-display-configuration.h"
#include "ui_kiran-display-configuration.h"
#include "kiranwidgets-qt5/kiran-message-box.h"
#include "widget-property-helper.h"
#include <QTimer>
#include <QButtonGroup>
#include <QResizeEvent>

KiranDisplayConfiguration::KiranDisplayConfiguration(QWidget *parent) :
                                                                       QWidget(parent), m_btnGroup(nullptr), m_dbusPropertiesChangedBlock(false),
                                                                       ui(new Ui::KiranDisplayConfiguration)
{
   ui->setupUi(this);

   Kiran::WidgetPropertyHelper::setButtonType(ui->pushButton_ok, Kiran::BUTTON_Default);
   Kiran::WidgetPropertyHelper::setButtonType(ui->pushButton_cancel, Kiran::BUTTON_Normal);
   Kiran::WidgetPropertyHelper::setButtonType(ui->pushButton_extra_ok, Kiran::BUTTON_Default);
   Kiran::WidgetPropertyHelper::setButtonType(ui->pushButton_extra_cancel, Kiran::BUTTON_Normal);

   ui->scrollAreaWidgetContents->setContentsMargins(0, 0, 10, 0);
   m_btnGroup = new QButtonGroup(this);
   m_btnGroup->addButton(ui->pushButton_copy_display, 0);
   m_btnGroup->addButton(ui->pushButton_extend_display, 1);

   connect(m_btnGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(onTabChanged(int, bool)));
   connect(ui->panel, &KiranDisplayConfigurationPanel::buttonChecked, this, &KiranDisplayConfiguration::onScreenItemChecked);

   refreshWidget();

   QDBusConnection::sessionBus().connect(KIRAN_DBUS_SERVICE_NAME, KIRAN_DBUS_DISPLAY, KIRAN_DBUS_INTREFACE_PROPERTIES, KIRAN_DBUS_PROPERTIES_FUN,
                                         this,
                                         SLOT(onDbusPropertiesChanged(QDBusMessage)));
}

KiranDisplayConfiguration::~KiranDisplayConfiguration()
{
   delete ui;
}

bool KiranDisplayConfiguration::hasUnsavedOptions()
{
   if(ui->stackedWidget->currentIndex() == 0)//复制模式
   {
       if(m_copyModeSavedData != getCopyModeUiData()) return true;
   }
   else//扩展模式
   {
       if(m_extraModeSavedData != getExtraModeUiData()) return true;
   }

   return false;
}

void KiranDisplayConfiguration::on_pushButton_ok_clicked()
{
   m_dbusPropertiesChangedBlock = true;
   QVariantMap map = ui->panel->getData().value(m_curMonitorPath).toMap();
   //添加当前编辑界面的数据
   QPair<QSize, QList<int> > pair = ui->comboBox_resolving->currentData().value<QPair<QSize, QList<int> > >();
   //复制模式，分辨率和刷新率都是一样的。
   map.insert("enabled", true);//复制模式，自动开始显示器
   map.insert("resolving", pair.first);
   map.insert("refreshRate", ui->comboBox_refreshRate->currentData());

   QStringList listMonitors = m_listMonitors;
   foreach (QString monitorPath, listMonitors) {
       setMonitorProperty(monitorPath, map);
   }

   Display("SetWindowScalingFactor", QVariantList() << ui->comboBox_windowScalingFactor->currentIndex());
   Display("ApplyChanges");
   //可能设置失败，界面根据设置实际情况再刷新。
   refreshWidget();

   showMessageBox();

   m_dbusPropertiesChangedBlock = false;
}

void KiranDisplayConfiguration::on_pushButton_extra_ok_clicked()
{
   m_dbusPropertiesChangedBlock = true;
   //添加/更新当前编辑界面的数据
   curExtraData2Cache();

   ///FIXME:此处保存两次只是为了暂时解决修改分辨率时由于未进行屏幕排列导致的内容重叠以及屏幕之中存在间隔
   ///后续重构应该更改为下方参数修改时上方预览应同步修改，以及动态调整屏幕位置
   for( int i=0;i<2;i++ )
   {
       //往屏幕数据中添加剩余的其它数据。
       QVariantMap map = ui->panel->getData();
       QMapIterator<QString, QVariant> iter(map);
       while (iter.hasNext()) {
           iter.next();
           QVariantMap d = iter.value().toMap();
           if(m_extraData.contains(iter.key()))//两个map的key值匹配上。
           {
               QVariantMap extraMap =  m_extraData.value(iter.key());
               //添加
               d.insert("primary", m_primaryMonitorName==d.value("name").toString());
               d.insert("enabled", extraMap.value("enabled"));
               d.insert("resolving", extraMap.value("resolving"));
               d.insert("refreshRate", extraMap.value("refreshRate"));
           }
           setMonitorProperty(iter.key(), d);
       }
       //缩放率所有屏幕都是通用的。
       Display("SetWindowScalingFactor", QVariantList() << ui->comboBox_extra_windowScalingFactor->currentIndex());
       Display("ApplyChanges");
       //可能设置失败，界面根据设置实际情况再刷新。
       refreshWidget();
   }

   showMessageBox();
   m_dbusPropertiesChangedBlock = false;
}

void KiranDisplayConfiguration::on_pushButton_cancel_clicked()
{
   if(Q_UNLIKELY( nullptr == parentWidget() ))
   {
       this->close();
   }
   else
   {
       this->window()->close();
   }
}

void KiranDisplayConfiguration::onTabChanged(int index, const bool &checked)
{
   if(!checked) return;
   if(m_btnGroup && m_btnGroup->checkedId() != index) m_btnGroup->button(index)->setChecked(true);

   if(index == 0)
   {
       initCopeMode();
       m_copyModeSavedData = getCopyModeUiData();
   }
   else
   {
       initExtraMode(ui->stackedWidget->currentIndex()!=index);
       m_extraModeSavedData = getExtraModeUiData();
   }

   ui->stackedWidget->setCurrentIndex(index);
}

void KiranDisplayConfiguration::onScreenItemChecked(QString monitorPath)
{
   int windowScalingFactor = DisplayProperty("window_scaling_factor").toInt();

   if(monitorPath == KIRAN_SCREEN_COPY_MODE_MONITOR_PATH)
   {
       m_extraData.clear();
       m_curMonitorPath = KIRAN_SCREEN_COPY_MODE_MONITOR_PATH;

       QList<DisplayModesStu> list = intersectionMonitorModes();
       QMap<int, QPair<QSize, QList<int> > > map = getResolutionFromModes(list);
       initComboBoxResolution(ui->comboBox_resolving, map);
       if(isCopyMode())//当前实际模式不是复制模式，而是从扩展模式初次切换到复制模式，此时，不获取的那个前模式。
       {
           DisplayModesStu stu = curIntersectionMonitorMode();
           //复制模式没有推荐，直接set text。
           ui->comboBox_resolving->setCurrentText(QString("%1x%2").arg(stu.w).arg(stu.h));
           ui->comboBox_refreshRate->setCurrentText(QString("%1HZ").arg((int)stu.refreshRate));

           ui->comboBox_windowScalingFactor->setCurrentIndex(windowScalingFactor);
       }
   }
   else
   {
       if(m_curMonitorPath != KIRAN_SCREEN_COPY_MODE_MONITOR_PATH && !m_curMonitorPath.isEmpty() && monitorPath != m_curMonitorPath)
       {
           curExtraData2Cache();
       }
       //
       m_curMonitorPath = monitorPath;

       QString clickedName = MonitorProperty(monitorPath, "name").toString();
       QList<DisplayModesStu> list = Monitor<QList<DisplayModesStu> >(monitorPath, "ListModes");
       QMap<int, QPair<QSize, QList<int> > > map = getResolutionFromModes(list);
       initExtraComboBoxResolution(ui->comboBox_extra_resolving, map);

       if(m_extraData.contains(monitorPath))
       {
           QVariantMap map = m_extraData.value(monitorPath);
           ui->pushButton_extra_primary->setChecked(m_primaryMonitorName == clickedName);
           ui->pushButton_enabled->setChecked(map.value("enabled").toBool());
           QSize size = map.value("resolving").toSize();
           selectResolutionComboboxItem(ui->comboBox_extra_resolving, size.width(), size.height());
           selectRefreshRateComboboxItem(ui->comboBox_extra_refreshRate, map.value("refreshRate").toInt());

           ui->comboBox_extra_windowScalingFactor->setCurrentIndex(map.value("windowScalingFactor").toInt());
       }
       else
       {
           QString primaryName = DisplayProperty("primary").toString();
           ui->pushButton_extra_primary->setChecked( primaryName == clickedName );
           ui->pushButton_enabled->setChecked( MonitorProperty(monitorPath, "enabled").toBool() );
           //
           DisplayModesStu stu;
           if(ui->pushButton_enabled->isChecked())
           {
               stu = Monitor<DisplayModesStu>(monitorPath, "GetCurrentMode");
           }

           selectResolutionComboboxItem(ui->comboBox_extra_resolving, stu.w, stu.h);
           selectRefreshRateComboboxItem(ui->comboBox_extra_refreshRate, (int)stu.refreshRate);

           ui->comboBox_extra_windowScalingFactor->setCurrentIndex(windowScalingFactor);
       }
       //多屏幕扩展模式，只有一个屏幕可用时，该屏幕不现实‘关闭’‘设为主屏幕’两项。
       QStringList enablePaths;//可用的屏幕的路径集合
       QStringList listMonitors = m_listMonitors;
       foreach (QString monitorPath, listMonitors) {
           if(m_extraData.contains(monitorPath))
           {
               if(m_extraData.value(monitorPath).value("enabled").toBool()) enablePaths << monitorPath;
           }
           else
           {
               if(MonitorProperty(monitorPath, "enabled").toBool()) enablePaths << monitorPath;
           }
       }
       if(enablePaths.count() <= 1 && enablePaths.contains(m_curMonitorPath))
       {
           ui->pushButton_enabled->setEnabled(false);
           ui->pushButton_extra_primary->setEnabled(extraPrimaryBtnStatus(true, ui->pushButton_enabled->isChecked()));
       }
       else
       {
           ui->pushButton_enabled->setEnabled(true);
           ui->pushButton_extra_primary->setEnabled(extraPrimaryBtnStatus(false, ui->pushButton_enabled->isChecked()));
       }
   }
}

QList<DisplayModesStu> KiranDisplayConfiguration::intersectionMonitorModes()
{
   QMap<int, DisplayModesStu> ret;
   QStringList listMonitors = m_listMonitors;
   int count = listMonitors.count();
   for(int i=0; i<count; ++i)
   {
       QString monitorPath = listMonitors.at(i);
       QList<DisplayModesStu> list = Monitor<QList<DisplayModesStu> >(monitorPath, "ListModes");

       QMap<int, DisplayModesStu> map;
       foreach (DisplayModesStu l, list) {
           map.insert(l.index, l);
       }

       if(i==0)
       {
           ret = map;
       }
       else
       {
           QList<int> retKeys = ret.keys();
           foreach (int key, retKeys) {
               if(!map.contains(key))
               {
                   ret.remove(key);
               }
           }
       }
   }

   return ret.values();
}

QMap<int, QPair<QSize, QList<int> > > KiranDisplayConfiguration::getResolutionFromModes(const QList<DisplayModesStu> &modes)
{
   QMap<int, QPair<QSize, QList<int> > > map;
   QMap<QString, QList<int> *> t_textMap;

   foreach (DisplayModesStu stu, modes) {
       QString text = QString("%1x%2").arg(stu.w).arg(stu.h);
       if(t_textMap.contains(text))
       {
           int refreshRate = stu.refreshRate;
           if(!t_textMap[text]->contains(refreshRate))
               t_textMap[text]->append(refreshRate);
           continue;
       }

       map.insert(stu.w * stu.h, QPair<QSize, QList<int> >(QSize(stu.w, stu.h), QList<int>()<<stu.refreshRate));
       t_textMap.insert(text, &map[stu.w*stu.h].second);
   }

   return map;
}

void KiranDisplayConfiguration::initExtraComboBoxResolution(QComboBox *comboBox, const QMap<int, QPair<QSize, QList<int> > > &map)
{
   comboBox->clear();
   QString recommend;
   QList<DisplayModesStu> list = Monitor<QList<DisplayModesStu> >(m_curMonitorPath, "ListPreferredModes");
   if(!list.isEmpty())
   {
       recommend = QString("%1x%2").arg(list.first().w).arg(list.first().h);
   }

   QMapIterator<int, QPair<QSize, QList<int> > > i(map);
   i.toBack();
   while (i.hasPrevious()) {
       i.previous();
       QPair<QSize, QList<int> > pair = i.value();
       QString text = QString("%1x%2").arg(pair.first.width()).arg(pair.first.height());
       if(text == recommend) text += tr(" (recommended)");
       QVariant var;
       var.setValue(pair);
       comboBox->addItem(text, var);
   }
}

void KiranDisplayConfiguration::initExtraComboBoxRefreshRate(QComboBox *comboBox, const QList<int> &refreshRateList)
{
   comboBox->clear();

   QString recommend;
   QList<DisplayModesStu> list = Monitor<QList<DisplayModesStu> >(m_curMonitorPath, "ListPreferredModes");
   if(!list.isEmpty())
   {
       recommend = QString("%1HZ").arg((int)list.first().refreshRate);
   }

   QList<int> t_refreshRateList = refreshRateList;
   std::sort(t_refreshRateList.begin(), t_refreshRateList.end(), std::greater<int>());
   foreach (int r, t_refreshRateList) {
       QString text = QString("%1HZ").arg(r);
       if(text == recommend) text += tr(" (recommended)");
       comboBox->addItem(text, r);
   }
}

void KiranDisplayConfiguration::curExtraData2Cache()
{
   QVariantMap map;
   if(ui->pushButton_extra_primary->isChecked()) m_primaryMonitorName = ui->panel->getData().value(m_curMonitorPath).toMap().value("name").toString();
   map.insert("enabled", ui->pushButton_enabled->isChecked());
   QPair<QSize, QList<int> > pair = ui->comboBox_extra_resolving->currentData().value<QPair<QSize, QList<int> > >();
   map.insert("resolving", pair.first);
   map.insert("refreshRate", ui->comboBox_extra_refreshRate->currentData());
   map.insert("windowScalingFactor", ui->comboBox_extra_windowScalingFactor->currentIndex());
   m_extraData.insert(m_curMonitorPath, map);
}

void KiranDisplayConfiguration::showMessageBox()
{
   KiranMessageBox box(this);
   box.setTitle(tr("Is the display normal?"));

   QPushButton saveBtn;
   saveBtn.setText(tr("Save current configuration(K)"));
   saveBtn.setFixedSize(QSize(200, box.buttonSize().height()));

   QPushButton cancelBtn;
   cancelBtn.setText(tr("Restore previous configuration(R)"));
   cancelBtn.setFixedSize(QSize(200, box.buttonSize().height()));

   box.addButton(&saveBtn, QDialogButtonBox::AcceptRole);
   box.addButton(&cancelBtn, QDialogButtonBox::RejectRole);
   saveBtn.setShortcut(Qt::CTRL + Qt::Key_K);
   cancelBtn.setShortcut(Qt::CTRL + Qt::Key_R);

   QString text = tr("The display will resume the previous configuration in %1 seconds");
   int countdown = 30;
   QTimer timer;
   timer.setInterval(1000);
   QObject::connect(&timer, &QTimer::timeout, [&](){
                        box.setText(text.arg(countdown--));
                        if(countdown < 0) box.reject();
                    });
   timer.start();

   box.setText(text.arg(countdown--));
   box.exec();
   if( box.clickedButton() == &saveBtn  )
   {
       int flag = 0;
       //后期可能根据var中返回的值来判断异常。
       QVariant var = Display("Save", QVariantList(), &flag);
       if(flag < 0)
       {
           KiranMessageBox box;
           box.setTitle(QObject::tr("Tips"));

           QPushButton btn;
           btn.setText(QObject::tr("OK(K)"));
           btn.setFixedSize(QSize(200, box.buttonSize().height()));
           btn.setShortcut(Qt::CTRL + Qt::Key_K);
           box.addButton(&btn, QDialogButtonBox::AcceptRole);
           box.setText(QObject::tr("Failed to apply display settings!%1").arg(var.toString()));
           box.exec();
       }
   }
   else
   {
       int flag = 0;
       QVariant var = Display("RestoreChanges", QVariantList(), &flag);
       if(flag < 0)
       {
           KiranMessageBox box;
           box.setTitle(QObject::tr("Tips"));

           QPushButton btn;
           btn.setText(QObject::tr("OK(K)"));
           btn.setFixedSize(QSize(200, box.buttonSize().height()));
           btn.setShortcut(Qt::CTRL + Qt::Key_K);
           box.addButton(&btn, QDialogButtonBox::AcceptRole);
           box.setText(QObject::tr("Fallback display setting failed! %1").arg(var.toString()));
           box.exec();
       }
   }
}

void KiranDisplayConfiguration::selectResolutionComboboxItem(QComboBox *comboBox, const int &w, const int &h)
{
   if(!comboBox) return;

   int count = comboBox->count();
   for(int i=0; i<count; ++i)
   {
       QPair<QSize, QList<int> > pair = comboBox->itemData(i).value<QPair<QSize, QList<int> > >();
       if(pair.first.width() == w && pair.first.height() == h)
       {
           comboBox->setCurrentIndex(i);
           return;
       }
   }
}

void KiranDisplayConfiguration::selectRefreshRateComboboxItem(QComboBox *comboBox, const int &r)
{
   if(!comboBox) return;

   int count = comboBox->count();
   for(int i=0; i<count; ++i)
   {
       int t_r= comboBox->itemData(i).toInt();
       if(r == t_r)
       {
           comboBox->setCurrentIndex(i);
           return;
       }
   }
}

bool KiranDisplayConfiguration::isCopyMode()
{
   QStringList listMonitors = m_listMonitors;
   int count = listMonitors.count();
   if(count == 1) return false;//如果只有一个屏幕，则为扩展模式。

   for(int i=0; i<count; ++i)
   {
       QString monitorPath = listMonitors.at(i);
       if( !MonitorProperty(monitorPath, "enabled").toBool() )
       {
           return false;//如果存在未开启的显示器，则不为复制模式。
       }
   }

   int x = 0;
   int y = 0;
   int w = 0;
   int h = 0;
   if(count > 0)
   {
       x = MonitorProperty(listMonitors.first(), "x").toInt();
       y = MonitorProperty(listMonitors.first(), "y").toInt();

       DisplayModesStu displayModeStu = Monitor<DisplayModesStu>(listMonitors.first(), "GetCurrentMode");
       w = displayModeStu.w;
       h = displayModeStu.h;
   }
   for(int i=1; i<count; ++i)
   {
       QString monitorPath = listMonitors.at(i);
       DisplayModesStu displayModeStu = Monitor<DisplayModesStu>(monitorPath, "GetCurrentMode");

       if(x != MonitorProperty(monitorPath, "x").toInt() || y != MonitorProperty(monitorPath, "y").toInt() || w != displayModeStu.w || h != displayModeStu.h)
       {
           return false;
       }
   }

   return true;
}

bool KiranDisplayConfiguration::extraPrimaryBtnStatus(const bool &onlyEnableScreen, const bool &enable)
{
   if(!onlyEnableScreen && enable) return true;
   return false;
}

void KiranDisplayConfiguration::refreshWidget()
{
   m_listMonitors = Display("ListMonitors").toStringList();
   foreach (QString monitorPath, m_listMonitors) { //如果显示器未开启，且没有最佳分辨率，就认为时虚拟机中的异常情况，就认为此显示器不存在。
       if(!MonitorProperty(monitorPath, "enabled").toBool())
       {
           QList<DisplayModesStu> list = Monitor<QList<DisplayModesStu> >(monitorPath, "ListPreferredModes");
           if(!list.isEmpty())
           {
               if(list.first().w == 0 && list.first().h == 0)
               {
                   m_listMonitors.removeAll(monitorPath);
               }
           }
       }
   }
   //如果只有一个屏幕，应当隐藏“复制模式”和“扩展模式”的选项卡。多屏幕扩展模式（包含单屏幕扩展）。多屏幕复制模式。
   QStringList listMonitors = m_listMonitors;
   ui->tabBtnWidget->setVisible(listMonitors.count() > 1);
   ui->enable_widget->setVisible(listMonitors.count() > 1);
   //    if(listMonitors.count() <= 1)
   //    {
   //        onTabChanged(0, true);
   //    }
   //    else
   //    {

   /*
    * 当一个屏幕时，为扩展显示中的特殊情况，单屏扩展。当多个屏幕时，屏幕位置x,y全部一样，表示为‘复制显示’，否则为‘扩展显示’。
    *‘复制显示’和‘扩展显示’的数据加载函数分别为“onTabChanged(0, true) ”和“onTabChanged(1, true)。
    */
   isCopyMode() ? onTabChanged(0, true) : onTabChanged(1, true);
   //    }
}

QVariantMap KiranDisplayConfiguration::getCopyModeUiData()
{
   QVariantMap map = ui->panel->getData().value(m_curMonitorPath).toMap();
   //添加当前编辑界面的数据
   QPair<QSize, QList<int> > pair = ui->comboBox_resolving->currentData().value<QPair<QSize, QList<int> > >();
   //复制模式，分辨率和刷新率都是一样的。
   map.insert("resolving", pair.first);
   map.insert("refreshRate", ui->comboBox_refreshRate->currentData());

   map.insert("SetWindowScalingFactor", QVariantList() << ui->comboBox_windowScalingFactor->currentIndex());
   return map;
}

QVariantMap KiranDisplayConfiguration::getExtraModeUiData()
{
   QVariantMap ret;
   //添加/更新当前编辑界面的数据
   curExtraData2Cache();

   //往屏幕数据中添加剩余的其它数据。
   QVariantMap map = ui->panel->getData();
   QMapIterator<QString, QVariant> i(map);
   while (i.hasNext()) {
       i.next();
       QVariantMap d = i.value().toMap();
       if(m_extraData.contains(i.key()))//两个map的key值匹配上。
       {
           QVariantMap extraMap =  m_extraData.value(i.key());
           //添加
           d.insert("primary", m_primaryMonitorName==d.value("name").toString());
           d.insert("enabled", extraMap.value("enabled"));
           d.insert("resolving", extraMap.value("resolving"));
           d.insert("refreshRate", extraMap.value("refreshRate"));
       }
       else //如果缓存中还没有暂存数据，则从后台获取。
       {
           QString monitorPath = i.key();
           d.insert("primary", m_primaryMonitorName == MonitorProperty(monitorPath, "name").toString());
           d.insert("enabled", MonitorProperty(monitorPath, "enabled").toBool());

           DisplayModesStu stu;
           if(d.value("enabled").toBool())
           {
               stu = Monitor<DisplayModesStu>(monitorPath, "GetCurrentMode");
           }

           d.insert("resolving", QSize(stu.w, stu.h));
           d.insert("refreshRate", (int)stu.refreshRate);
       }

       ret.insert(i.key(), d);
   }
   //缩放率所有屏幕都是通用的。
   ret.insert("SetWindowScalingFactor", QVariantList() << ui->comboBox_extra_windowScalingFactor->currentIndex());
   return ret;
}

DisplayModesStu KiranDisplayConfiguration::curIntersectionMonitorMode()
{
   DisplayModesStu ret;
   QStringList listMonitors = m_listMonitors;
   if(listMonitors.count() > 0) ret = Monitor<DisplayModesStu>(listMonitors.first(), "GetCurrentMode");

   return ret;
}

void KiranDisplayConfiguration::setMonitorProperty(const QString &monitorPath, const QVariantMap &map)
{
   //qDebug() << "send data:" << monitorPath << map;
   // property enabled must be set before others
   if(map.contains("enabled")) Monitor<QVariant>(monitorPath, "Enable", QVariantList() << map.value("enabled").toBool());

   if(map.contains("x") && map.contains("y")) Monitor<QVariant>(monitorPath, "SetPosition", QVariantList() << (int32_t)map.value("x").toInt() << (int32_t)map.value("y").toInt());
   if(map.contains("x") && map.contains("y")){
       qInfo() << "set position:" << (int32_t)map.value("x").toInt() << (int32_t)map.value("y").toInt();
   }
   if(map.contains("rotation")){
       QVariant var;
       var.setValue(QDBusArgument() << (ushort)map.value("rotation").toUInt());
       Monitor<QVariant>(monitorPath, "SetRotation", QVariantList() << var);
   }
   if(map.contains("reflect")){
       QVariant var;
       var.setValue(QDBusArgument() << (ushort)map.value("reflect").toUInt());
       Monitor<QVariant>(monitorPath, "SetReflect", QVariantList() << var);
   }

   if(map.contains("primary") && map.value("primary").toBool()) Display("SetPrimary", QVariantList() << map.value("name"));

   if(map.contains("resolving") && map.contains("refreshRate"))
       Monitor<QVariant>(monitorPath, "SetMode", QVariantList() << (uint32_t)map.value("resolving").toSize().width() << (uint32_t)map.value("resolving").toSize().height() << map.value("refreshRate").toDouble());
}

void KiranDisplayConfiguration::initCopeMode()
{
   QString text;
   int rotation = 0;
   int reflect = 0;
   QStringList listMonitors = m_listMonitors;
   foreach (QString monitorPath, listMonitors) {
       text += (text.isEmpty() ? "" : "|" ) + MonitorProperty(monitorPath, "name").toString();
   }
   if(listMonitors.count() > 0)
   {
       rotation = MonitorProperty(listMonitors.first(), "rotation").toInt();
       reflect = MonitorProperty(listMonitors.first(), "reflect").toInt();
   }

   QVariantList list;
   QVariantMap map;
   map.insert("text", text);
   map.insert("x", 0);
   map.insert("y", 0);
   map.insert("w", 1920);
   map.insert("h", 1080);
   map.insert("rotation", rotation);
   map.insert("reflect", reflect);
   map.insert("enabled", true);
   list << map;
   ui->panel->setData(list);

   //qDebug() << "receive data:" << list;

   foreach (QString name, m_dbusConnectList) {
       QDBusConnection::sessionBus().disconnect(KIRAN_DBUS_SERVICE_NAME, name, KIRAN_DBUS_INTREFACE_PROPERTIES, KIRAN_DBUS_PROPERTIES_FUN,
                                                this,
                                                SLOT(onDbusPropertiesChanged(QDBusMessage)));
   }
   m_dbusConnectList.clear();
   if(listMonitors.count() > 0)
   {
       QDBusConnection::sessionBus().connect(KIRAN_DBUS_SERVICE_NAME, listMonitors.first(), KIRAN_DBUS_INTREFACE_PROPERTIES, KIRAN_DBUS_PROPERTIES_FUN,
                                             this,
                                             SLOT(onDbusPropertiesChanged(QDBusMessage)));
       m_dbusConnectList << listMonitors.first();
   }
}

void KiranDisplayConfiguration::initExtraMode(const bool &clearChecked)
{
   foreach (QString name, m_dbusConnectList) {
       QDBusConnection::sessionBus().disconnect(KIRAN_DBUS_SERVICE_NAME, name, KIRAN_DBUS_INTREFACE_PROPERTIES, KIRAN_DBUS_PROPERTIES_FUN,
                                                this,
                                                SLOT(onDbusPropertiesChanged(QDBusMessage)));
   }
   m_dbusConnectList.clear();

   QVariantList list;
   QStringList listMonitors = m_listMonitors;
   int offset = 0;
   bool isCopy = isCopyMode();
   foreach (QString monitorPath, listMonitors) {
       QVariantMap map;
       map.insert("text", MonitorProperty(monitorPath, "name"));
       map.insert("x", MonitorProperty(monitorPath, "x").toInt()+offset);
       map.insert("y", MonitorProperty(monitorPath, "y"));
       map.insert("rotation", MonitorProperty(monitorPath, "rotation"));
       map.insert("reflect", MonitorProperty(monitorPath, "reflect"));
       map.insert("enabled", MonitorProperty(monitorPath, "enabled").toBool());

       DisplayModesStu displayModeStu;
       if(map.value("enabled").toBool())
       {
           displayModeStu = Monitor<DisplayModesStu>(monitorPath, "GetCurrentMode");
       }
       //当显示器关闭时，大小将为0。
       if(displayModeStu.w <= 0 || displayModeStu.h <= 0)
       {
           map.insert("x", 99999);//让x足够大，保证从右侧合并过来。
           displayModeStu.w = 1920;
           displayModeStu.h = 1080;
       }
       map.insert("w", displayModeStu.w);
       map.insert("h", displayModeStu.h);
       map.insert("monitorPath", monitorPath);

       if(isCopy) offset += map.value("w").toInt();//如果点击扩展页面时，当前正处于复制模式，那所有x的值都往右边位移。

       list << map;

       QDBusConnection::sessionBus().connect(KIRAN_DBUS_SERVICE_NAME, monitorPath, KIRAN_DBUS_INTREFACE_PROPERTIES, KIRAN_DBUS_PROPERTIES_FUN,
                                             this,
                                             SLOT(onDbusPropertiesChanged(QDBusMessage)));
       m_dbusConnectList << monitorPath;
   }

   ui->panel->setData(list, clearChecked);
   //如果只有一个屏幕,不会进入这个函数，而是进入tab的index为0的函数。
   //    //如果只有一个屏幕，禁用‘设置主屏’‘关闭’按钮。
   //    QString toolTipStr;
   //    if(list.count() <= 1) toolTipStr = tr("禁用");
   //    ui->pushButton_extra_primary->setDisabled(list.count()<=1);
   //    ui->pushButton_enabled->setDisabled(list.count()<=1);
   //    ui->pushButton_extra_primary->setToolTip(toolTipStr);
   //    ui->pushButton_enabled->setToolTip(toolTipStr);

   //qDebug() << "receive data:" << list;
}

void KiranDisplayConfiguration::initComboBoxResolution(QComboBox *comboBox, const QMap<int, QPair<QSize, QList<int> > > &map)
{
   comboBox->clear();

   QMapIterator<int, QPair<QSize, QList<int> > > i(map);
   i.toBack();
   while (i.hasPrevious()) {
       i.previous();
       QPair<QSize, QList<int> > pair = i.value();
       QString text = QString("%1x%2").arg(pair.first.width()).arg(pair.first.height());
       QVariant var;
       var.setValue(pair);
       comboBox->addItem(text, var);
   }
}

void KiranDisplayConfiguration::initComboBoxRefreshRate(QComboBox *comboBox, const QList<int> &refreshRateList)
{
   comboBox->clear();

   QList<int> t_refreshRateList = refreshRateList;
   std::sort(t_refreshRateList.begin(), t_refreshRateList.end(), std::greater<int>());
   foreach (int r, t_refreshRateList) {
       QString text = QString("%1HZ").arg(r);
       comboBox->addItem(text, r);
   }
}

void KiranDisplayConfiguration::on_comboBox_extra_resolving_currentTextChanged(const QString &)
{
   QPair<QSize, QList<int> > pair = ui->comboBox_extra_resolving->currentData().value<QPair<QSize, QList<int> > >();
   initExtraComboBoxRefreshRate(ui->comboBox_extra_refreshRate, pair.second);
}

void KiranDisplayConfiguration::onDbusPropertiesChanged(QDBusMessage)
{
   if(m_dbusPropertiesChangedBlock) return;
   m_extraData.clear();
   refreshWidget();
   //onTabChanged(ui->stackedWidget->currentIndex(), true);
}

void KiranDisplayConfiguration::on_comboBox_resolving_currentTextChanged(const QString &)
{
   QPair<QSize, QList<int> > pair = ui->comboBox_resolving->currentData().value<QPair<QSize, QList<int> > >();
   initComboBoxRefreshRate(ui->comboBox_refreshRate, pair.second);
}

void KiranDisplayConfiguration::on_pushButton_enabled_toggled(bool checked)
{
   ui->pushButton_extra_primary->setEnabled(extraPrimaryBtnStatus(!ui->pushButton_enabled->isEnabled(), checked));
   ui->comboBox_extra_resolving->setEnabled((checked));
   ui->comboBox_extra_refreshRate->setEnabled(checked);
   ui->comboBox_extra_windowScalingFactor->setEnabled(checked);
   ui->panel->changeItemDisabled(checked);

   //    QString toolTipStr;
   //    if(!checked <= 1) toolTipStr = tr("禁用");
   //    ui->pushButton_extra_primary->setToolTip(toolTipStr);
   //    ui->comboBox_extra_resolving->setToolTip(toolTipStr);
   //    ui->comboBox_extra_refreshRate->setToolTip(toolTipStr);
   //    ui->comboBox_extra_windowScalingFactor->setToolTip(toolTipStr);
}

void KiranDisplayConfiguration::on_pushButton_extra_cancel_clicked()
{
   on_pushButton_cancel_clicked();
}

QSize KiranDisplayConfiguration::sizeHint() const
{
   return QSize(679,765);
}