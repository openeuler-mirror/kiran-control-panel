#include "kiran-system-widget.h"
#include "kiran-system-information.h"

#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QAction>
#include <QEvent>
#include <QDebug>
#include <QTimer>
KiranSystemWidget::KiranSystemWidget(): KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    setTitle(tr("kiran-system-imformation"));
    setIcon(QIcon(":/images/kylin-about.png"));
    //setWindowModality(Qt::NonModal);

    centerWgt = new kiranSystemInformation;
    setWindowContentWidget(centerWgt);

    setMenuIntoTitlebar();
    connect(centerWgt, &kiranSystemInformation::destroyed, this, &KiranSystemWidget::close);

//    this->adjustSize();

//    QTimer::singleShot(1000,this,[this](){
//        this->adjustSize();
//    });
}

KiranSystemWidget::~KiranSystemWidget()
{
    if(centerWgt != nullptr)
    {
       delete centerWgt;
    }
}

QSize KiranSystemWidget::sizeHint() const
{
    QSize size = QSize(918,670);
    return size;
}

void KiranSystemWidget::resizeEvent(QResizeEvent *event)
{

     qInfo() << "width: " << this->width() << " height: " << this->height() << endl;

}

QMenu* KiranSystemWidget::createMenu()
{
    QAction* actionEULA = new QAction(this);
    QAction* actionExport = new QAction(this);

    actionEULA->setText(tr("EULA"));
    actionExport->setText(tr("export"));

    QMenu* menu = new QMenu(this);
    menu->addAction(actionEULA);
    menu->addAction(actionExport);

    menu->installEventFilter(this);
    return menu;
}

void KiranSystemWidget::setMenuIntoTitlebar()
{
    setTitlebarCustomLayoutAlignHCenter(false);

    QHBoxLayout* layout = getTitlebarCustomLayout();
    btnMenu = new QPushButton(this);
    btnMenu->setFixedSize(24,24);
    btnMenu->setIcon(QIcon(":/images/menu.svg"));

    btnMenu->setStyleSheet("QPushButton::menu-indicator{image:none;}"
                           "QPushButton{background-color: transparent;}"
                           "QPushButton:pressed{background-color: #393939;}"
                           "QPushButton:hover{background-color: #2d2d2d;}"
                           "QPushButton{border: none;}");

    QMenu* menu = createMenu();
    menu->setStyleSheet("QMenu{background-color: #222222;border-radius: 8px; border:1px solid rgba(255,255,255,0.2);padding-top:12px;"
                              "color:#ffffff; font-family: \"Noto Sans CJK SC regular\"; font-size:10px;}"
                        "QMenu::item{margin-bottom:5px;"
                                    "height :30px;"
                                    "padding-left:24px}"
                        "QMenu::item:selected{background-color:#393939;"
                                             "border-right:2px solid #2eb3ff}");
    menu->setFixedSize(115,96);

    btnMenu->setMenu(menu);
    layout->addWidget(btnMenu,0,Qt::AlignRight);

    QWidget* widget = new QWidget(this);
    widget->setFixedSize(1,24);
    widget->setStyleSheet("QWidget{background-color: #2d2d2d;}");

    layout->addWidget(widget);
    layout->setSpacing(10);
}

bool KiranSystemWidget::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::Show && target == btnMenu->menu())
    {

        int menuXPos = btnMenu->menu()->pos().x();
        int menuYPos = btnMenu->menu()->pos().y();

        QPoint pos = QPoint(menuXPos,menuYPos+4);

        btnMenu->menu()->move(pos);
        return true;
    }

    return false;
}
