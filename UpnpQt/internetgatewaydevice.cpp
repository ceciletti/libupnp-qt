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
#include "internetgatewaydevice.h"
#include "service.h"
#include "discover.h"
#include "soapenvelope.h"
#include "reply.h"
#include "wanconnectionservice.h"

#include <QHostAddress>

#include <QNetworkReply>
#include <QNetworkAccessManager>

#include <QDomDocument>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(UPNPQT_IDG, "upnpqt.idg")

using namespace UpnpQt;

InternetGatewayDevice::InternetGatewayDevice(DevicePrivate *priv, Discover *parent) : Device(priv, parent)
{

}

WanConnectionService *InternetGatewayDevice::wanIpOrPppConnectionService() const
{
    auto srv = qobject_cast<WanConnectionService *>(findService(QStringLiteral("WANIPConnection")));
    if (!srv) {
        srv = qobject_cast<WanConnectionService *>(findService(QStringLiteral("WANPPPConnection")));
    }
    return srv;
}

#include "moc_internetgatewaydevice.cpp"
