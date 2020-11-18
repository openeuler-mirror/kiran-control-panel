/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "modelbase.h"
#include <QDebug>

modelbase::modelbase()
{
    m_hasUnsavedOptions = false;
}

bool modelbase::has_unsaved_options()
{
    return m_hasUnsavedOptions;
}
