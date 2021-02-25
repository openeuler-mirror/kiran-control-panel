#include "kiran-system-widget.h"
#include "kiran-system-information.h"
#include "license/user-license-agreement.h"
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QAction>
#include <QEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>
#include <QTextDocument>
#include <QTimer>

#define EULAFILE "/usr/share/kylin-release/EULA"

KiranSystemWidget::KiranSystemWidget():
    KiranTitlebarWindow(),
    userlicenseAgreement(nullptr)
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
    if(userlicenseAgreement != nullptr )
    {
        delete userlicenseAgreement;
    }
}

QSize KiranSystemWidget::sizeHint() const
{
    /*根据系统分辨率设置窗口大小*/
    QDesktopWidget *desktop = QApplication::desktop();
    qInfo() << desktop->width() << desktop->height();
    QSize windowSize;
    if(desktop->height() >= 730 && desktop->width() >= 918 ) //能显示全
    {
        windowSize = QSize(918,730);
    }
    else
    {
        windowSize = QSize(desktop->width() , desktop->height());
    }

    return windowSize;
}

void KiranSystemWidget::resizeEvent(QResizeEvent *event)
{

     qInfo() << "width: " << getWindowContentWidget()->width() << " height: " << getWindowContentWidget()->height() << endl;

}

QMenu* KiranSystemWidget::createMenu()
{
    actionEULA = new QAction(this);
    actionExport = new QAction(this);

    actionEULA->setText(tr("EULA"));
    actionEULA->setToolTip(tr("Look over EULA"));
    actionExport->setText(tr("Export"));
    actionExport->setToolTip(tr("Export EULA"));

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

    menu = createMenu();
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

    connect(actionEULA , SIGNAL(triggered()), this,SLOT(actionEulaClicked()));
    connect(actionExport, SIGNAL(triggered()), this, SLOT(actionExportClicked()));
}

void KiranSystemWidget::actionEulaClicked()
{
    if(userlicenseAgreement == nullptr)
    {
        userlicenseAgreement = new UserlicenseAgreement();
    }
    userlicenseAgreement->show();
}

void KiranSystemWidget::actionExportClicked()
{
    //获取界面显示的EULA文字
    if(userlicenseAgreement == nullptr)
    {
        userlicenseAgreement = new UserlicenseAgreement();
    }
    QString eulaText = userlicenseAgreement->getEulaText();

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("保存"),
                                                    "/root/EULA.pdf",
                                                    tr("PDF(*.pdf)"));
    if(fileName.isNull())
    {
        return;
    }
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        KiranMessageBox::KiranStandardButton button = KiranMessageBox::message(nullptr,QString(tr("Export EULA")),QString(tr("Export EULA failed!")),KiranMessageBox::Ok);
        if(button == KiranMessageBox::Ok)
        {
            return;
        }
    }
    else
    {
        //将EULA文字转化为PDF
        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPaperSize(QPrinter::A4);
        printer.setOutputFileName(fileName);

        QTextDocument doc;
        doc.setPlainText(eulaText); /* 可替换为文档内容 */
        doc.setPageSize(printer.pageRect().size());
        doc.print(&printer);
    }
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
