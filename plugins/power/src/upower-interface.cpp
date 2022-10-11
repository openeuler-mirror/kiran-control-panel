/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

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

    auto devices = up_client_get_devices2(upClient);
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
