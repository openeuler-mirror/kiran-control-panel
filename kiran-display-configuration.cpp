#include "kiran-display-configuration.h"
#include "ui_kiran-display-configuration.h"
#include "kiranwidgets-qt5/kiran-message-box.h"
#include <QTimer>
#include <QButtonGroup>
#include <QDebug>

KiranDisplayConfiguration::KiranDisplayConfiguration(QWidget *parent) :
    QWidget(parent), m_btnGroup(nullptr),
    ui(new Ui::KiranDisplayConfiguration)
{
    ui->setupUi(this);
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
    QVariantMap map = ui->panel->getData().value(m_curMonitorPath).toMap();
    //添加当前编辑界面的数据
    QPair<QSize, QList<int> > pair = ui->comboBox_resolving->currentData().value<QPair<QSize, QList<int> > >();
    //复制模式，分辨率和刷新率都是一样的。
    map.insert("resolving", pair.first);
    map.insert("refreshRate", ui->comboBox_refreshRate->currentData());

    QStringList listMonitors = Display("ListMonitors").toStringList();
    foreach (QString monitorPath, listMonitors) {
        setMonitorProperty(monitorPath, map);
    }

    Display("SetWindowScalingFactor", QVariantList() << ui->comboBox_windowScalingFactor->currentIndex());
    Display("ApplyChanges");
    //可能设置失败，界面根据设置实际情况再刷新。
    refreshWidget();

    showMessageBox();
}

void KiranDisplayConfiguration::on_pushButton_extra_ok_clicked()
{
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

        setMonitorProperty(i.key(), d);
    }
    //缩放率所有屏幕都是通用的。
    Display("SetWindowScalingFactor", QVariantList() << ui->comboBox_extra_windowScalingFactor->currentIndex());
    Display("ApplyChanges");
    //可能设置失败，界面根据设置实际情况再刷新。
    refreshWidget();

    showMessageBox();
}

void KiranDisplayConfiguration::on_pushButton_cancel_clicked()
{
    if(Q_UNLIKELY( nullptr == parent() ))
    {
        this->close();
    }
    else
    {
        sigClose();
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
        DisplayModesStu stu = curIntersectionMonitorMode();
        //复制模式没有推荐，直接set text。
        ui->comboBox_resolving->setCurrentText(QString("%1x%2").arg(stu.w).arg(stu.h));
        ui->comboBox_refreshRate->setCurrentText(QString("%1HZ").arg((int)stu.refreshRate));

        ui->comboBox_windowScalingFactor->setCurrentIndex(windowScalingFactor);
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
            DisplayModesStu stu = Monitor<DisplayModesStu>(monitorPath, "GetCurrentMode");
            selectResolutionComboboxItem(ui->comboBox_extra_resolving, stu.w, stu.h);
            selectRefreshRateComboboxItem(ui->comboBox_extra_refreshRate, (int)stu.refreshRate);

            ui->comboBox_extra_windowScalingFactor->setCurrentIndex(windowScalingFactor);
        }
    }
}

QList<DisplayModesStu> KiranDisplayConfiguration::intersectionMonitorModes()
{
    QMap<int, DisplayModesStu> ret;
    QStringList listMonitors = Display("ListMonitors").toStringList();
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
        if(text == recommend) text += tr(" (推荐)");
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
        if(text == recommend) text += tr(" (推荐)");
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
    box.setTitle(tr("显示是否正常?"));

    QPushButton saveBtn;
    saveBtn.setText(tr("保存当前配置(K)"));
    saveBtn.setFixedSize(QSize(200, box.buttonSize().height()));

    QPushButton cancelBtn;
    cancelBtn.setText(tr("恢复之前的配置(R)"));
    cancelBtn.setFixedSize(QSize(200, box.buttonSize().height()));

    box.addButton(&saveBtn, QDialogButtonBox::AcceptRole);
    box.addButton(&cancelBtn, QDialogButtonBox::RejectRole);
    saveBtn.setShortcut(Qt::CTRL + Qt::Key_K);
    cancelBtn.setShortcut(Qt::CTRL + Qt::Key_R);

    QString text = tr("显示将会在 %1 秒后恢复之前的配置");
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
            box.setTitle(QObject::tr("提示"));

            QPushButton btn;
            btn.setText(QObject::tr("确定(K)"));
            btn.setFixedSize(QSize(200, box.buttonSize().height()));
            btn.setShortcut(Qt::CTRL + Qt::Key_K);
            box.addButton(&btn, QDialogButtonBox::AcceptRole);
            box.setText(QObject::tr("应用显示设置失败!"));
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
            box.setTitle(QObject::tr("提示"));

            QPushButton btn;
            btn.setText(QObject::tr("确定(K)"));
            btn.setFixedSize(QSize(200, box.buttonSize().height()));
            btn.setShortcut(Qt::CTRL + Qt::Key_K);
            box.addButton(&btn, QDialogButtonBox::AcceptRole);
            box.setText(QObject::tr("回撤显示设置失败!"));
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
    QStringList listMonitors = Display("ListMonitors").toStringList();
    int count = listMonitors.count();
    if(count == 1) return false;//如果只有一个屏幕，则为扩展模式。

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

void KiranDisplayConfiguration::refreshWidget()
{
    //如果只有一个屏幕，应当隐藏“复制模式”和“扩展模式”的选项卡。
    QStringList listMonitors = Display("ListMonitors").toStringList();
    ui->tabBtnWidget->setVisible(listMonitors.count() > 1);
    ui->enable_widget->setVisible(listMonitors.count() > 1);
//    if(listMonitors.count() <= 1)
//    {
//        onTabChanged(0, true);
//    }
//    else
//    {

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

            DisplayModesStu stu = Monitor<DisplayModesStu>(monitorPath, "GetCurrentMode");
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
    QStringList listMonitors = Display("ListMonitors").toStringList();
    if(listMonitors.count() > 0) ret = Monitor<DisplayModesStu>(listMonitors.first(), "GetCurrentMode");

    return ret;
}

void KiranDisplayConfiguration::setMonitorProperty(const QString &monitorPath, const QVariantMap &map)
{
    qDebug() << "发送数据:" << monitorPath << map;

    if(map.contains("x") && map.contains("y")) Monitor<QVariant>(monitorPath, "SetPosition", QVariantList() << (int32_t)map.value("x").toInt() << (int32_t)map.value("y").toInt());
    if(map.contains("rotation")){
        QVariant var;
        var.setValue(QDBusArgument() << (ushort)map.value("rotation").toUInt());
        Monitor<QVariant>(monitorPath, "SetRotation", QVariantList() << var);
    }

    if(map.contains("primary") && map.value("primary").toBool()) Display("SetPrimary", QVariantList() << map.value("name"));
    if(map.contains("enabled")) Monitor<QVariant>(monitorPath, "Enable", QVariantList() << map.value("enabled").toBool());

    if(map.contains("resolving") && map.contains("refreshRate"))
        Monitor<QVariant>(monitorPath, "SetMode", QVariantList() << (uint32_t)map.value("resolving").toSize().width() << (uint32_t)map.value("resolving").toSize().height() << map.value("refreshRate").toDouble());
}

void KiranDisplayConfiguration::initCopeMode()
{
    QString text;
    int rotation = 0;
    QStringList listMonitors = Display("ListMonitors").toStringList();
    foreach (QString monitorPath, listMonitors) {
        text += (text.isEmpty() ? "" : "|" ) + MonitorProperty(monitorPath, "name").toString();
    }
    if(listMonitors.count() > 0)
        rotation = MonitorProperty(listMonitors.first(), "rotation").toInt();

    QVariantList list;
    QVariantMap map;
    map.insert("text", text);
    map.insert("x", 0);
    map.insert("y", 0);
    map.insert("w", 1920);
    map.insert("h", 1080);
    map.insert("rotation", rotation);
    map.insert("enabled", true);
    list << map;
    ui->panel->setData(list);

    qDebug() << "接收数据:" << list;

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
    QStringList listMonitors = Display("ListMonitors").toStringList();
    int offset = 0;
    bool isCopy = isCopyMode();
    foreach (QString monitorPath, listMonitors) {
        QVariantMap map;
        map.insert("text", MonitorProperty(monitorPath, "name"));
        map.insert("x", MonitorProperty(monitorPath, "x").toInt()+offset);
        map.insert("y", MonitorProperty(monitorPath, "y"));
        map.insert("rotation", MonitorProperty(monitorPath, "rotation"));
        map.insert("enabled", MonitorProperty(monitorPath, "enabled").toBool());

        DisplayModesStu displayModeStu = Monitor<DisplayModesStu>(monitorPath, "GetCurrentMode");
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

    qDebug() << "接收数据:" << list;
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
    ui->pushButton_extra_primary->setEnabled(checked);
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
