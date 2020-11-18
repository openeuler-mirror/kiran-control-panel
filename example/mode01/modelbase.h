/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef MODELBASE_H
#define MODELBASE_H

#include <QObject>

class modelbase{
public:
    explicit modelbase();
    bool has_unsaved_options();

signals:

public slots:

protected:
    bool m_hasUnsavedOptions;
};

#endif // MODELBASE_H
