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
#include "wanconnectionservice.h"
#include "soapenvelope.h"
#include "device.h"
#include "reply.h"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QDomDocument>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(UPNPQT_WANSRV, "upnpqt.wansrv", QtInfoMsg)

using namespace UpnpQt;

WanConnectionService::WanConnectionService(UpnpQt::ServicePrivate *priv, QObject *parent)
    : Service(priv, parent)
{

}

Reply *WanConnectionService::addPortMapping(quint16 externalPort, const QString &internalAddress, quint16 internalPort, QAbstractSocket::SocketType sockType, const QString &description, bool enabled, int leaseDuration, const QString &remoteHost)
{
    auto ret = new Reply(this);
    qCDebug(UPNPQT_WANSRV) << "addPortMapping" << externalPort << internalAddress << sockType << remoteHost;

    SoapEnvelope envelope(QStringLiteral("AddPortMapping"), type());

    envelope.writeTextElement(QStringLiteral("NewRemoteHost"), remoteHost);
    envelope.writeTextElement(QStringLiteral("NewExternalPort"), QString::number(externalPort));
    envelope.writeTextElement(QStringLiteral("NewProtocol"),
                            sockType == QAbstractSocket::TcpSocket ? QStringLiteral("TCP") : QStringLiteral("UDP"));
    envelope.writeTextElement(QStringLiteral("NewInternalPort"), QString::number(internalPort));
    envelope.writeTextElement(QStringLiteral("NewInternalClient"), internalAddress);
    envelope.writeTextElement(QStringLiteral("NewEnabled"), enabled ? QStringLiteral("1") : QStringLiteral("0"));
    envelope.writeTextElement(QStringLiteral("NewPortMappingDescription"), description);
    envelope.writeTextElement(QStringLiteral("NewLeaseDuration"), QString::number(leaseDuration));

    auto device = qobject_cast<Device*>(parent());
    QUrl url(device->urlBase());
    url.setPath(controlUrl().path());
    url.setQuery(controlUrl().query());

    QNetworkReply *reply = device->nam()->post(envelope.request(url), envelope.render());
    connect(reply, &QNetworkReply::finished, this, [=] {
        reply->deleteLater();

        const QByteArray data = reply->readAll();
        qCDebug(UPNPQT_WANSRV) << "addPortMapping downloaded XML" << reply->error() << data.constData();
        if (reply->error()) {
            auto error = SoapEnvelope::responseError(data);
            ret->finishWithError(error.second, error.first);
        } else {
            ret->finish();
        }
    });

    qCDebug(UPNPQT_WANSRV) << device->urlBase() << url << envelope.render().constData();

    return ret;
}

Reply *WanConnectionService::addPortMapping(quint16 externalPort, const QHostAddress &internalAddress, quint16 internalPort, QAbstractSocket::SocketType sockType, const QString &description, bool enabled, int leaseDuration, const QHostAddress &remoteHost)
{
    return addPortMapping(externalPort, internalAddress.toString(), internalPort, sockType, description, enabled, leaseDuration, remoteHost.toString());
}

Reply *WanConnectionService::deletePortMapping(quint16 externalPort, QAbstractSocket::SocketType sockType, const QString &remoteHost)
{
    auto ret = new Reply(this);
    qCDebug(UPNPQT_WANSRV) << "deletePortMapping port " << externalPort << sockType << remoteHost;

    SoapEnvelope envelope(QStringLiteral("DeletePortMapping"), type());

    envelope.writeTextElement(QStringLiteral("NewRemoteHost"), remoteHost);
    envelope.writeTextElement(QStringLiteral("NewExternalPort"), QString::number(externalPort));
    envelope.writeTextElement(QStringLiteral("NewProtocol"),
                              sockType == QAbstractSocket::TcpSocket ? QStringLiteral("TCP") : QStringLiteral("UDP"));

    auto device = qobject_cast<Device*>(parent());
    QUrl url(device->urlBase());
    url.setPath(controlUrl().path());
    url.setQuery(controlUrl().query());

    QNetworkReply *reply = device->nam()->post(envelope.request(url), envelope.render());
    connect(reply, &QNetworkReply::finished, this, [=] {
        reply->deleteLater();

        const QByteArray data = reply->readAll();
        qCDebug(UPNPQT_WANSRV) << "downloaded XML DeletePortMapping" << reply->error() << data.constData();
        if (reply->error()) {
            auto error = SoapEnvelope::responseError(data);
            ret->finishWithError(error.second, error.first);
        } else {
            ret->finish();
        }
    });

    qCDebug(UPNPQT_WANSRV) << device->urlBase() << url << envelope.render().constData();
    return ret;
}

Reply *WanConnectionService::deletePortMapping(quint16 externalPort, QAbstractSocket::SocketType sockType, const QHostAddress &remoteHost)
{
    return deletePortMapping(externalPort, sockType, remoteHost.toString());
}

Reply *WanConnectionService::getSpecificPortMappingEntry(quint16 externalPort, QAbstractSocket::SocketType sockType, const QString &remoteHost)
{
    qCDebug(UPNPQT_WANSRV) << "Forwarding port " << externalPort << sockType << remoteHost;
    auto ret = new Reply(this);

    SoapEnvelope envelope(QStringLiteral("GetSpecificPortMappingEntry"), type());

    envelope.writeTextElement(QStringLiteral("NewRemoteHost"), remoteHost);
    envelope.writeTextElement(QStringLiteral("NewExternalPort"), QString::number(externalPort));
    envelope.writeTextElement(QStringLiteral("NewProtocol"),
                            sockType == QAbstractSocket::TcpSocket ? QStringLiteral("TCP") : QStringLiteral("UDP"));

    auto device = qobject_cast<Device*>(parent());
    QUrl url(device->urlBase());
    url.setPath(controlUrl().path());
    url.setQuery(controlUrl().query());

    QNetworkReply *reply = device->nam()->post(envelope.request(url), envelope.render());
    connect(reply, &QNetworkReply::finished, this, [=] {
        reply->deleteLater();

        const QByteArray data = reply->readAll();
        qCDebug(UPNPQT_WANSRV) << "downloaded XML GetSpecificPortMappingEntry" << reply->error() << data.constData();
        if (reply->error()) {
            auto error = SoapEnvelope::responseError(data);
            ret->finishWithError(error.second, error.first);
        } else {
            QDomDocument doc;
            doc.setContent(data, true);
            const QDomElement res = doc
                    .documentElement()
                    .firstChildElement(QStringLiteral("Body"))
                    .firstChildElement(QStringLiteral("GetSpecificPortMappingEntryResponse"));
            qCDebug(UPNPQT_WANSRV) << res.firstChildElement(QStringLiteral("NewLeaseDuration")).text();
            PortMap map;
            map.internalPort = quint16(res.firstChildElement(QStringLiteral("NewInternalPort")).text().toUInt());
            map.internalAddress = res.firstChildElement(QStringLiteral("NewInternalClient")).text();
            map.externalPort = externalPort;
            map.sockType = sockType;
            map.enabled = res.firstChildElement(QStringLiteral("NewEnabled")).text() == QLatin1String("1");
            map.description = res.firstChildElement(QStringLiteral("NewPortMappingDescription")).text();
            map.leaseDuration = res.firstChildElement(QStringLiteral("NewLeaseDuration")).text().toInt();
            map.remoteHost = remoteHost;
            ret->finishWithData(QVariant::fromValue(map));
        }
    });

    qCDebug(UPNPQT_WANSRV) << device->urlBase() << url << envelope.render().constData();
    return ret;
}

Reply *WanConnectionService::getSpecificPortMappingEntry(quint16 externalPort, QAbstractSocket::SocketType sockType, const QHostAddress &remoteHost)
{
    return getSpecificPortMappingEntry(externalPort, sockType, remoteHost.toString());
}

Reply *WanConnectionService::getGenericPortMapping()
{
    auto ret = new Reply(this);
    getGenericPortMapping(ret);
    return ret;
}

Reply *WanConnectionService::getStatusInfo()
{
    qCDebug(UPNPQT_WANSRV) << "GetStatusInfo";
    auto ret = new Reply(this);

    SoapEnvelope envelope(QStringLiteral("GetStatusInfo"), type());

    auto device = qobject_cast<Device*>(parent());
    QUrl url(device->urlBase());
    url.setPath(controlUrl().path());
    url.setQuery(controlUrl().query());

    QNetworkReply *reply = device->nam()->post(envelope.request(url), envelope.render());
    connect(reply, &QNetworkReply::finished, this, [=] {
        reply->deleteLater();

        const QByteArray data = reply->readAll();
        qCDebug(UPNPQT_WANSRV) << "GetStatusInfo downloaded XML" << reply->error() << data.constData();
        if (reply->error()) {
            auto error = SoapEnvelope::responseError(data);
            ret->finishWithError(error.second, error.first);
        } else {
            ret->finish();
        }
    });

    qCDebug(UPNPQT_WANSRV) << device->urlBase() << url << envelope.render().constData();
    return ret;
}

Reply *WanConnectionService::getExternalIp()
{
    auto ret = new Reply(this);
    qCDebug(UPNPQT_WANSRV) << "getExternalIp" << parent() << parent()->parent() << qobject_cast<Device*>(parent()->parent())->urlBase();

    SoapEnvelope envelope(QStringLiteral("GetExternalIPAddress"), type());

    auto device = qobject_cast<Device*>(parent());
    QUrl url(device->urlBase());
    url.setPath(controlUrl().path());
    url.setQuery(controlUrl().query());

    QNetworkReply *reply = device->nam()->post(envelope.request(url), envelope.render());
    connect(reply, &QNetworkReply::finished, this, [=] {
        reply->deleteLater();

        QByteArray data = reply->readAll();
        qCDebug(UPNPQT_WANSRV) << "getExternalIp downloaded XML" << reply->error() << data.constData();
        if (!reply->error()) {
            QDomDocument doc;
            doc.setContent(data, true);
            const QDomElement res = doc
                    .documentElement()
                    .firstChildElement(QStringLiteral("Body"))
                    .firstChildElement(QStringLiteral("GetExternalIPAddressResponse"))
                    .firstChildElement(QStringLiteral("NewExternalIPAddress"));
            ret->finishWithData(res.text());
        } else {
            ret->finishWithError(QStringLiteral("error"));
        }
    });

    qCDebug(UPNPQT_WANSRV) << "getExternalIp" << device->urlBase() << url << envelope.render().constData();
    return ret;
}

void WanConnectionService::getGenericPortMapping(Reply *ret, int index, const std::vector<PortMap> &portMaps)
{
    qCDebug(UPNPQT_WANSRV) << "getGenericPortMapping";
    SoapEnvelope envelope(QStringLiteral("GetGenericPortMappingEntry"), type());

    envelope.writeTextElement(QStringLiteral("NewPortMappingIndex"), QString::number(index));

    auto device = qobject_cast<Device*>(parent());
    QUrl url(device->urlBase());
    url.setPath(controlUrl().path());
    url.setQuery(controlUrl().query());

    QNetworkReply *reply = device->nam()->post(envelope.request(url), envelope.render());
    connect(reply, &QNetworkReply::finished, this, [=] {
        reply->deleteLater();

        const QByteArray data = reply->readAll();
        qCDebug(UPNPQT_WANSRV) << "downloaded XML" << reply->error() << data.constData();
        if (!reply->error()) {
            QDomDocument doc;
            doc.setContent(data, true);
            const QDomElement res = doc
                    .documentElement()
                    .firstChildElement(QStringLiteral("Body"))
                    .firstChildElement(QStringLiteral("GetGenericPortMappingEntryResponse"));
            qCDebug(UPNPQT_WANSRV) << res.firstChildElement(QStringLiteral("NewLeaseDuration")).text();
            PortMap map;
            map.internalPort = quint16(res.firstChildElement(QStringLiteral("NewInternalPort")).text().toUInt());
            map.internalAddress = res.firstChildElement(QStringLiteral("NewInternalClient")).text();
            map.externalPort = quint16(res.firstChildElement(QStringLiteral("NewExternalPort")).text().toUInt());
            map.sockType = res.firstChildElement(QStringLiteral("NewProtocol")).text() == QLatin1String("TCP")
                    ? QAbstractSocket::TcpSocket : QAbstractSocket::UdpSocket;
            map.enabled = res.firstChildElement(QStringLiteral("NewEnabled")).text() == QLatin1String("1");
            map.description = res.firstChildElement(QStringLiteral("NewPortMappingDescription")).text();
            map.leaseDuration = res.firstChildElement(QStringLiteral("NewLeaseDuration")).text().toInt();
            map.remoteHost = res.firstChildElement(QStringLiteral("NewRemoteHost")).text();

            std::vector<PortMap> maps = portMaps;
            maps.push_back(map);
            getGenericPortMapping(ret, index + 1, maps);
        } else {
            auto error = SoapEnvelope::responseError(data);
            if (error.first == QLatin1String("713") || error.second == QLatin1String("SpecifiedArrayIndexInvalid")) {
                ret->finishWithData(QVariant::fromValue(portMaps));
            } else {
                ret->finishWithError(error.second, error.first);
            }
        }
    });

    qCDebug(UPNPQT_WANSRV) << device->urlBase() << url << envelope.render().constData();
}

#include "moc_wanconnectionservice.cpp"
