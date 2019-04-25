/*
 * Copyright (C) 2019 Daniel Nicoletti <dantti12@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef UPNPINTERNETGATEWAYDEVICE_H
#define UPNPINTERNETGATEWAYDEVICE_H

#include <QObject>

#include <UpnpQt/global.h>
#include <UpnpQt/device.h>

#include <QAbstractSocket>

namespace UpnpQt {

class Reply;
class Discover;
class WanConnectionService;
class InternetGatewayDevicePrivate;
class UPNPQT_LIBRARY InternetGatewayDevice : public Device
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(InternetGatewayDevice)
public:
    explicit InternetGatewayDevice(DevicePrivate *priv, Discover *parent);

    /**
     * @brief wanIpOrPppConnectionService
     * Search for WanIpConnectionService or WanPppConnection service respectively
     * @return service class or nullptr if not found.
     */
    WanConnectionService *wanIpOrPppConnectionService() const;

private:
    InternetGatewayDevicePrivate *d_ptr;
};

}

#endif // UPNPINTERNETGATEWAYDEVICE_H
