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
#include "service.h"
#include "service_p.h"
#include "device.h"

#include <QUrl>

using namespace UpnpQt;

Service::Service(ServicePrivate *priv, QObject *parent) : QObject(parent)
  , d_ptr(priv)
{

}

Service::~Service()
{

}

QString Service::id() const
{
    Q_D(const Service);
    return d->id;
}

QString Service::type() const
{
    Q_D(const Service);
    return d->type;
}

QUrl Service::controlUrl() const
{
    Q_D(const Service);
    return d->controlurl;
}

QUrl Service::eventsubUrl() const
{
    Q_D(const Service);
    return d->eventsuburl;
}

QUrl Service::scpdUrl() const
{
    Q_D(const Service);
    return d->scpdurl;
}

#include "moc_service.cpp"
