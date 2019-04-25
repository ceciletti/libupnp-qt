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
#ifndef UPNPSERVICE_H
#define UPNPSERVICE_H

#include <QObject>

#include <UpnpQt/global.h>

class QUrl;
namespace UpnpQt {

class ServicePrivate;
class UPNPQT_LIBRARY Service : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Service)
public:
    explicit Service(ServicePrivate *priv, QObject *parent = nullptr);
    ~Service();

    QString id() const;
    QString type() const;
    QUrl controlUrl() const;
    QUrl eventsubUrl() const;
    QUrl scpdUrl() const;

protected:
    ServicePrivate *d_ptr;
};

}

#endif // UPNPSERVICE_H
