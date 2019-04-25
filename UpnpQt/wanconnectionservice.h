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
#ifndef WANCONNECTIONSERVICE_H
#define WANCONNECTIONSERVICE_H

#include <QObject>
#include <QHostAddress>
#include <QAbstractSocket>

#include <UpnpQt/global.h>
#include <UpnpQt/service.h>

namespace UpnpQt {

class Reply;
class UPNPQT_LIBRARY WanConnectionService : public Service
{
    Q_OBJECT
public:
    explicit WanConnectionService(ServicePrivate *priv, QObject *parent = nullptr);

    struct PortMap {
        quint16 externalPort;
        QString internalAddress;
        quint16 internalPort;
        QAbstractSocket::SocketType sockType;
        QString description;
        bool enabled = true;
        int leaseDuration = 0;
        QString remoteHost;
    };

    /**
     * @brief addPortMapping
     *
     * Adds a port mapping to an internal host, parameters that have a default
     * value are not required.
     * @param externalPort The
     * @param internalAddress
     * @param internalPort
     * @param sockType
     * @param description
     * @param enabled
     * @param leaseDuration 0 is the maximum value the router will allow, otherwise the unit is seconds
     * @param remoteHost
     * @return when Reply emits finished error must be false
     */
    Reply *addPortMapping(quint16 externalPort,
                          const QString &internalAddress,
                          quint16 internalPort,
                          QAbstractSocket::SocketType sockType,
                          const QString &description,
                          bool enabled = true,
                          int leaseDuration = 0,
                          const QString &remoteHost = QString());
    Reply *addPortMapping(quint16 externalPort,
                          const QHostAddress &internalAddress,
                          quint16 internalPort,
                          QAbstractSocket::SocketType sockType,
                          const QString &description,
                          bool enabled = true,
                          int leaseDuration = 0,
                          const QHostAddress &remoteHost = QHostAddress());

    Reply *deletePortMapping(quint16 externalPort, QAbstractSocket::SocketType sockType, const QString &remoteHost = QString());
    Reply *deletePortMapping(quint16 externalPort, QAbstractSocket::SocketType sockType, const QHostAddress &remoteHost);

    Reply *getSpecificPortMappingEntry(quint16 externalPort,
                                       QAbstractSocket::SocketType sockType,
                                       const QString &remoteHost = QString());
    Reply *getSpecificPortMappingEntry(quint16 externalPort,
                                       QAbstractSocket::SocketType sockType,
                                       const QHostAddress &remoteHost);

    Reply *getGenericPortMapping();

    Reply *getStatusInfo();

    Reply *getExternalIp();

private:
    void getGenericPortMapping(Reply *ret, int index = 0, const std::vector<PortMap> &portMaps = std::vector<PortMap>());
};

}

Q_DECLARE_METATYPE(UpnpQt::WanConnectionService::PortMap)

#endif // WANCONNECTIONSERVICE_H
