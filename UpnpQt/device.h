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
#ifndef UPNPQT_DEVICE_H
#define UPNPQT_DEVICE_H

#include <QObject>
#include <QString>
#include <QUrl>

#include <UpnpQt/global.h>

class QNetworkAccessManager;
namespace UpnpQt {

class Discover;
class Service;
class DevicePrivate;
class UPNPQT_LIBRARY Device : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Device)
public:
    Device();
    Device(DevicePrivate *priv, Discover *parent);

    QString type() const;
    QString friendlyName() const;
    QString manufacturer() const;
    QUrl manufacturerUrl() const;
    QString modelDescription() const;
    QString modelName() const;
    QString modelNumber() const;
    QUrl modelUrl() const;
    QString udn() const;
    QString urlBase() const;

    QNetworkAccessManager *nam() const;

    std::vector<Device *> devices() const;
    std::vector<Service *> services() const;

    Service *findService(const QString &service) const;

    static Device *fromXml(const QByteArray &data, Discover *parent);

protected:
    friend class DiscoverPrivate;
    void setUrlBase(const QString &urlBase);

    DevicePrivate *d_ptr;
};

}

#endif // UPNPQT_DEVICE_H
