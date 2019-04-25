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
#include "device.h"
#include "discover.h"
#include "internetgatewaydevice.h"
#include "wanconnectiondevice.h"
#include "wanconnectionservice.h"
#include "service.h"
#include "service_p.h"

#include <QXmlStreamReader>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(UPNPQT_XML, "upnpqt.xml", QtInfoMsg)

namespace UpnpQt {

class DevicePrivate {
public:
    QString type;
    QString friendlyName;
    QString manufacturer;
    QUrl manufacturerURL;
    QString modelDescription;
    QString modelName;
    QString modelNumber;
    QUrl modelUrl;
    QString udn;
    QString urlBase;
    Discover *q_ptr;
    std::vector<Device *> devices;
    std::vector<Service *> services;
};

}

using namespace UpnpQt;

Device::Device(DevicePrivate *priv, Discover *parent)
    : QObject(parent)
    , d_ptr(priv)
{
    Q_D(Device);
    d->q_ptr = parent;
    for (Device *dev : d->devices) {
        dev->setParent(this);
    }
    for (Service *srv : d->services) {
        srv->setParent(this);
    }
}

QString Device::type() const
{
    Q_D(const Device);
    return d->type;
}

QString Device::friendlyName() const
{
    Q_D(const Device);
    return d->friendlyName;
}

QString Device::manufacturer() const
{
    Q_D(const Device);
    return d->manufacturer;
}

QUrl Device::manufacturerUrl() const
{
    Q_D(const Device);
    return d->manufacturerURL;
}

QString Device::modelDescription() const
{
    Q_D(const Device);
    return d->modelDescription;
}

QString Device::modelName() const
{
    Q_D(const Device);
    return d->modelName;
}

QString Device::modelNumber() const
{
    Q_D(const Device);
    return d->modelNumber;
}

QUrl Device::modelUrl() const
{
    Q_D(const Device);
    return d->modelUrl;
}

QString Device::udn() const
{
    Q_D(const Device);
    return d->udn;
}

QString Device::urlBase() const
{
    Q_D(const Device);
    return d->urlBase;
}

QNetworkAccessManager *Device::nam() const
{
    Q_D(const Device);
    return d->q_ptr->nam();
}

std::vector<Device *> Device::devices() const
{
    Q_D(const Device);
    return d->devices;
}

std::vector<Service *> Device::services() const
{
    Q_D(const Device);
    return d->services;
}

Service *Device::findService(const QString &service) const
{
    for (Service *srv : services()) {
        const QString type = srv->type();
        if (type.contains(service)) {
            return srv;
        }
    }

    for (Device *device : devices()) {
        Service *srv = device->findService(service);
        if (srv) {
            return srv;
        }
    }
    return nullptr;
}

Service *parseService(QXmlStreamReader &xml, QObject *parent)
{
    qCDebug(UPNPQT_XML) << "parse service";
    ServicePrivate *priv = new ServicePrivate;
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType type = xml.readNext();
        qCDebug(UPNPQT_XML) << "next service" << type << xml.dtdName() << xml.text() << xml.name();
        if (type == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("serviceType")) {
                priv->type = xml.readElementText();
            } else if (xml.name() == QLatin1String("serviceId")) {
                priv->id = xml.readElementText();
            } else if (xml.name() == QLatin1String("controlURL")) {
                priv->controlurl = QUrl(xml.readElementText());
            } else if (xml.name() == QLatin1String("eventSubURL")) {
                priv->eventsuburl = QUrl(xml.readElementText());
            } else if (xml.name() == QLatin1String("SCPDURL")) {
                priv->scpdurl = QUrl(xml.readElementText());
            } else {
                xml.skipCurrentElement();
            }
        } else if (type == QXmlStreamReader::EndElement) {
            break;
        }
    }

    qCDebug(UPNPQT_XML) << "SERIVCE TYPE" << priv->type;
    if (priv->type == QLatin1String("urn:schemas-upnp-org:service:WANPPPConnection:1")) {
        return new WanConnectionService(priv, parent);
    } else if (priv->type == QLatin1String("urn:schemas-upnp-org:service:WANIPConnection:1")) {
        return new WanConnectionService(priv, parent);
    }
    return new Service(priv, parent);
}

Device *parseDevice(QXmlStreamReader &xml, Discover *parent)
{
    qCDebug(UPNPQT_XML) << "parse device";
    DevicePrivate *priv = new DevicePrivate;
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType type = xml.readNext();
        qCDebug(UPNPQT_XML) << "next dev" << type << xml.dtdName() << xml.text() << xml.name();
        if (type == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("deviceType")) {
                priv->type = xml.readElementText();
            } else if (xml.name() == QLatin1String("friendlyName")) {
                priv->friendlyName = xml.readElementText();
            } else if (xml.name() == QLatin1String("manufacturer")) {
                priv->manufacturer = xml.readElementText();
            } else if (xml.name() == QLatin1String("manufacturerURL")) {
                priv->manufacturerURL = QUrl(xml.readElementText());
            } else if (xml.name() == QLatin1String("modelDescription")) {
                priv->modelDescription = xml.readElementText();
            } else if (xml.name() == QLatin1String("modelName")) {
                priv->modelName = xml.readElementText();
            } else if (xml.name() == QLatin1String("modelNumber")) {
                priv->modelNumber = xml.readElementText();
            } else if (xml.name() == QLatin1String("modelURL")) {
                priv->modelUrl = QUrl(xml.readElementText());
            } else if (xml.name() == QLatin1String("UDN")) {
                priv->udn = xml.readElementText();
            } else if (xml.name() == QLatin1String("deviceList")) {
                while (!xml.atEnd()) {
                    QXmlStreamReader::TokenType typeRoot = xml.readNext();
                    qCDebug(UPNPQT_XML) << "next deviceList" << typeRoot << xml.dtdName() << xml.text() << xml.name();
                    if (typeRoot == QXmlStreamReader::StartElement) {
                        if (xml.name() == QLatin1String("device")) {
                            Device *dev = parseDevice(xml, parent);
                            if (dev) {
                                priv->devices.push_back(dev);
                            }
                        } else {
                            xml.skipCurrentElement();
                        }
                    } else if (typeRoot == QXmlStreamReader::EndElement) {
                        break;
                    }
                }
            } else if (xml.name() == QLatin1String("serviceList")) {
                while (!xml.atEnd()) {
                    QXmlStreamReader::TokenType typeRoot = xml.readNext();
                    qCDebug(UPNPQT_XML) << "next serviceList" << typeRoot << xml.dtdName() << xml.text() << xml.name();
                    if (typeRoot == QXmlStreamReader::StartElement) {
                        if (xml.name() == QLatin1String("service")) {
                            Service *service = parseService(xml, parent);
                            if (service) {
                                priv->services.push_back(service);
                            }
                        } else {
                            xml.skipCurrentElement();
                        }
                    } else if (typeRoot == QXmlStreamReader::EndElement) {
                        break;
                    }
                }
            } else {
                xml.skipCurrentElement();
            }
        } else if (type == QXmlStreamReader::EndElement) {
            break;
        }
    }

    qCDebug(UPNPQT_XML) << "DEVICE TYPE" << priv->type;
    if (priv->type == QLatin1String("urn:schemas-upnp-org:device:InternetGatewayDevice:1")) {
        return new InternetGatewayDevice(priv, parent);
    } else if (priv->type == QLatin1String("urn:schemas-upnp-org:device:WANConnectionDevice:1")) {
        return new WANConnectionDevice(priv, parent);
    }
    return new Device(priv, parent);
}

Device *Device::fromXml(const QByteArray &data, Discover *parent)
{
    Device *ret = nullptr;
    QString urlBase;
    QXmlStreamReader xml(data);
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType type = xml.readNext();
        qCDebug(UPNPQT_XML) << "next" << type << xml.dtdName() << xml.text() << xml.name();
        if (type == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("root")) {
                while (!xml.atEnd()) {
                    QXmlStreamReader::TokenType typeRoot = xml.readNext();
                    qCDebug(UPNPQT_XML) << "next Root" << typeRoot << xml.dtdName() << xml.text() << xml.name();
                    if (typeRoot == QXmlStreamReader::StartElement) {
                        if (xml.name() == QLatin1String("device")) {
                            ret = parseDevice(xml, parent);
                        } else if (xml.name() == QLatin1String("URLBase")) {
                            urlBase = xml.readElementText();
                        } else {
                            xml.skipCurrentElement();
                        }
                    } else if (typeRoot == QXmlStreamReader::EndElement) {
                        break;
                    }
                }
            } else {
                xml.skipCurrentElement();
            }
        }
    }

    if (xml.hasError()) {
        delete ret;
        return nullptr;
    }
    qCDebug(UPNPQT_XML) << "URL BASE" << urlBase << ret << ret->urlBase();
    ret->setUrlBase(urlBase);

    return ret;
}

void Device::setUrlBase(const QString &urlBase)
{
    Q_D(Device);
    d->urlBase = urlBase;
    for (Device *dev : d->devices) {
        if (dev->urlBase().isEmpty()) {
            dev->setUrlBase(urlBase);
        }
    }
}

#include "moc_device.cpp"
