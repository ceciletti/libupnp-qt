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
#ifndef UPNPWANCONNECTIONDEVICE_H
#define UPNPWANCONNECTIONDEVICE_H

#include <QObject>

#include <UpnpQt/global.h>
#include <UpnpQt/device.h>

namespace UpnpQt {

class Discover;
class UPNPQT_LIBRARY WANConnectionDevice : public Device
{
    Q_OBJECT
public:
    explicit WANConnectionDevice(DevicePrivate *priv, Discover *parent);

};

}

#endif // UPNPWANCONNECTIONDEVICE_H
