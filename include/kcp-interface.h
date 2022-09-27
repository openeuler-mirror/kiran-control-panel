#pragma once

#include <QString>

class KcpInterface
{
public:
    virtual void handlePluginSubItemInfoChanged(const QString& subItemID) = 0;
    virtual void handlePluginSubItemChanged() = 0;
};