#include "kiran-display-configuration.h"
#include "ui_kiran-display-configuration.h"
#include "kiranwidgets-qt5/kiran-message-box.h"
#include <QTimer>
#include <QButtonGroup>
#include <QDebug>

KiranDisplayConfiguration::KiranDisplayConfiguration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KiranDisplayConfiguration)
{
    ui->setupUi(this);
    QButtonGroup *btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->pushButton_copy_display, 0);
    btnGroup->addButton(ui->pushButton_extend_display, 1);
    connect(btnGroup, SIGNAL(buttonToggled(int,bool)), this, SLOT(onTabChanged(int, bool)));

    connect(ui->panel, &KiranDisplayConfigurationPanel::buttonChecked, this, &KiranDisplayConfiguration::onScreenItemChecked);

    onTabChanged(0, true);

    QDBusConnection::sessionBus().connect(KIRAN_DBUS_SERVICE_NAME, KIRAN_DBUS_DISPLAY, KIRAN_DBUS_INTREFACE_PROPERTIES, KIRAN_DBUS_PROPERTIES_FUN,
                                          this,
                                          SLOT(onDbusPropertiesChanged(QDBusMessage)));
}

KiranDisplayConfiguration::~KiranDisplayConfiguration()
{
    delete ui;
}

void KiranDisplayConfiguration::on_pushButton_ok_clicked()
{
    QVariantMap map = ui->panel->getData().value(m_curMonitorPath).toMap();
    //添加当前编辑界面的数据
    QPair<QSize, QList<int> > pair = ui->comboBox_resolving->currentData().value<QPair<QSize, QList<int> > >();
    map.insert("resolving", pair.first);
    map.insert("refreshRate", ui->comboBox_refreshRate->currentData());

    QStringList listMonitors = Display("ListMonitors").toStringList();
    foreach (QString monitorPath, listMonitors) {
        setMonitorProperty(monitorPath, map);
    }

    Display("SetWindowScalingFactor", QVariantList() << ui->comboBox_windowScalingFactor->currentIndex());
    Display("ApplyChanges");

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

    if(index == 0)
    {
        initCopeMode();
    }
    else
    {
        initExtraMode(ui->stackedWidget->currentIndex()!=index);
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
            ui->comboBox_extra_resolving->setCurrentText(QString("%1x%2").arg(size.width()).arg(size.height()));
            ui->comboBox_extra_refreshRate->setCurrentText(QString("%1HZ").arg(map.value("refreshRate").toInt()));

            ui->comboBox_extra_windowScalingFactor->setCurrentIndex(map.value("windowScalingFactor").toInt());
        }
        else
        {
            QString primaryName = DisplayProperty("primary").toString();
            ui->pushButton_extra_primary->setChecked( primaryName == clickedName );
            ui->pushButton_enabled->setChecked( MonitorProperty(monitorPath, "enabled").toBool() );
            //
            DisplayModesStu stu = Monitor<DisplayModesStu>(monitorPath, "GetCurrentMode");
            ui->comboBox_extra_resolving->setCurrentText(QString("%1x%2").arg(stu.w).arg(stu.h));
            ui->comboBox_extra_refreshRate->setCurrentText(QString("%1HZ").arg((int)stu.refreshRate));

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
    KiranMessageBox box;
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
        Display("Save");
    }
    else
    {
        Display("RestoreChanges");
    }
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
    foreach (QString monitorPath, listMonitors) {
        QVariantMap map;
        map.insert("text", MonitorProperty(monitorPath, "name"));
        map.insert("x", MonitorProperty(monitorPath, "x"));
        map.insert("y", MonitorProperty(monitorPath, "y"));
        map.insert("rotation", MonitorProperty(monitorPath, "rotation"));

        DisplayModesStu displayModeStu = Monitor<DisplayModesStu>(monitorPath, "GetCurrentMode");
        map.insert("w", displayModeStu.w);
        map.insert("h", displayModeStu.h);
        map.insert("monitorPath", monitorPath);

        list << map;

        QDBusConnection::sessionBus().connect(KIRAN_DBUS_SERVICE_NAME, monitorPath, KIRAN_DBUS_INTREFACE_PROPERTIES, KIRAN_DBUS_PROPERTIES_FUN,
                                              this,
                                              SLOT(onDbusPropertiesChanged(QDBusMessage)));
        m_dbusConnectList << monitorPath;
    }

    ui->panel->setData(list, clearChecked);
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
    onTabChanged(ui->stackedWidget->currentIndex(), true);
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
    ui->pushButton_extra_ok->setEnabled(checked);
}

void KiranDisplayConfiguration::on_pushButton_extra_ok_clicked()
{
    //添加当前编辑界面的数据
    curExtraData2Cache();

    QVariantMap map = ui->panel->getData();
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        QVariantMap d = i.value().toMap();
        if(m_extraData.contains(i.key()))
        {
            QVariantMap extraMap =  m_extraData.value(i.key());
            d.insert("primary", m_primaryMonitorName==d.value("name").toString());
            d.insert("enabled", extraMap.value("enabled"));
            d.insert("resolving", extraMap.value("resolving"));
            d.insert("refreshRate", extraMap.value("refreshRate"));
        }

        setMonitorProperty(i.key(), d);
    }

    Display("SetWindowScalingFactor", QVariantList() << ui->comboBox_extra_windowScalingFactor->currentIndex());
    Display("ApplyChanges");

    showMessageBox();
}

void KiranDisplayConfiguration::on_pushButton_extra_cancel_clicked()
{
    on_pushButton_cancel_clicked();
}
