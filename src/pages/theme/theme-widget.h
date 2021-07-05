#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
class ThemeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ThemeWidget(QSize iconSize, QString currentIconTheme,QString iconThemeName,
                         QStringList iconStringList , QWidget *parent = 0);
    explicit ThemeWidget(QString name, QString currentTheme, QWidget*parent = 0);
    explicit ThemeWidget(QSize cursorSize, QString currentCursorTheme, QString cursorThemeName,
                         const QList<QPixmap> &cursorListMap , QWidget *parent = 0);

    //explicit ThemeWidget(QSize iSize, QString name, const QList<QPixmap> &listMap , QWidget *parent = 0);
    ~ThemeWidget();
    void setSelectStatus(bool, int themeType);
    void setTheme(QString);
    QString getTheme();


signals:
    void clicked();

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    QLabel* m_iconSelectLabel;
    QWidget* m_iconImgWidget;
    QWidget* m_currIconTheme;

    QLabel* m_selectLabel;
    QWidget* m_themeImgWidget;

    QLabel* m_cursorSelectLabel;
    QWidget *m_cursorWidget;

    QString m_theme;

};

#endif // THEMEWIDGET_H
