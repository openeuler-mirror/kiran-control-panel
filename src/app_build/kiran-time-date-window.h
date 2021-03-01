/**
 *@file  kiran-time-date-window.h
 *@brief 
 *@auth  liuxinhao <liuxinhao@kylinos.com.cn>
 *@copyright (c) 2021 KylinSec. All rights reserved.
 */
#ifndef KIRAN_TIMEDATE_MANAGER_KIRAN_TIME_DATE_WINDOW_H
#define KIRAN_TIMEDATE_MANAGER_KIRAN_TIME_DATE_WINDOW_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>

class KiranTimeDateWindow : public KiranTitlebarWindow{
    Q_OBJECT
public:
    explicit KiranTimeDateWindow(QWidget* parent= nullptr);
    ~KiranTimeDateWindow();
};


#endif //KIRAN_TIMEDATE_MANAGER_KIRAN_TIME_DATE_WINDOW_H
