//
// Created by lxh on 2021/12/21.
//

#include "upower-interface.h"
#include <upower.h>
#include <cstdio>

bool UPowerInterface::haveBattery()
{
    GError *error = nullptr;
    auto upClient = up_client_new();
    bool hasBattery = false;

#if !UP_CHECK_VERSION(0, 99, 0)
    gboolean ret;
    ret = up_client_enumerate_devices_sync(upClient, NULL, &error);
    if (!ret)
    {
        fprintf(stderr, "failed to get device list: %s", error->message);
        g_error_free(error);
    }
#endif

    auto devices = up_client_get_devices(upClient);
    void *device = nullptr;
    UpDeviceKind kind;
    for (int i = 0; i < devices->len; i++)
    {
        device = g_ptr_array_index(devices, i);
        g_object_get(device,
                     "kind", &kind,
                     NULL);
        if (kind == UP_DEVICE_KIND_BATTERY)
            hasBattery = true;
    }
    g_ptr_array_unref (devices);
    g_object_unref(upClient);
    return hasBattery;
}
