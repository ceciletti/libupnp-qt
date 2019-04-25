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
#include "discover.h"
#include "device.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef Q_WS_WIN
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#endif
#include <arpa/inet.h>

#include <QUdpSocket>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(UPNPQT_DISCOVER, "upnpqt.discover", QtInfoMsg)

namespace UpnpQt {

class DiscoverPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(Discover)
public:
    DiscoverPrivate(Discover *q) : q_ptr(q) {}

    void joinUPnPMGroup(int fd);
    void leaveUPnPMGroup(int fd);
    void parse(const QByteArray &data, Discover *parent);

    Discover *q_ptr;
    QNetworkAccessManager *nam;
    QUdpSocket udpSocket;
};

}

using namespace UpnpQt;

Discover::Discover(QObject *parent) : QObject(parent)
  , d_ptr(new DiscoverPrivate(this))
{
    Q_D(Discover);
    d->nam = new QNetworkAccessManager(this);
    connect(&d->udpSocket, &QUdpSocket::readyRead, this, [=] {
        const qint64 pendingDatagramSize = d->udpSocket.pendingDatagramSize();
        if (pendingDatagramSize == 0) {
            qCDebug(UPNPQT_DISCOVER) << "0 byte UDP packet ";
            // TODO validate this
            // KDatagramSocket wrongly handles UDP packets with no payload
            // so we need to deal with it oursleves
            int fd = int(d->udpSocket.socketDescriptor());
            char tmp;
            ::read(fd, &tmp, 1);
            return;
        }

        QByteArray data(int(pendingDatagramSize), 0);
        if (d->udpSocket.readDatagram(data.data(), pendingDatagramSize) == -1) {
            return;
        }

        qCDebug(UPNPQT_DISCOVER) << "Got data" << data;

        d->parse(data, this);
    });
    connect(&d->udpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            [=](QAbstractSocket::SocketError socketError){
        qCWarning(UPNPQT_DISCOVER) << "Socket Error " << socketError << d->udpSocket.errorString();
    });

    for (quint16 i = 1900; i < 1910; ++i) {
        if (!d->udpSocket.bind(i, QUdpSocket::ShareAddress)) {
            qCWarning(UPNPQT_DISCOVER) << "Cannot bind to UDP port" << i << d->udpSocket.errorString();
        } else {
            qCInfo(UPNPQT_DISCOVER) << "Bound to UDP port" << i;
            break;
        }
    }

    d->joinUPnPMGroup(int(d->udpSocket.socketDescriptor()));
}

Discover::~Discover()
{
    d_ptr->leaveUPnPMGroup(int(d_ptr->udpSocket.socketDescriptor()));
    delete d_ptr;
}

QNetworkAccessManager *Discover::nam() const
{
    Q_D(const Discover);
    return  d->nam;
}

void Discover::discoverInternetGatewayDevice()
{
    Q_D(Discover);

    qCInfo(UPNPQT_DISCOVER) << "Trying to find UPnP devices on the local network";

    // send a HTTP M-SEARCH message to 239.255.255.250:1900
    const char* upnp_data = "M-SEARCH * HTTP/1.1\r\n"
                            "HOST: 239.255.255.250:1900\r\n"
                            "ST:urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n"
                            "MAN:\"ssdp:discover\"\r\n"
                            "MX:3\r\n"
                            "\r\n\0";

    const char* tr64_data = "M-SEARCH * HTTP/1.1\r\n"
                            "HOST: 239.255.255.250:1900\r\n"
                            "ST:urn:dslforum-org:device:InternetGatewayDevice:1\r\n"
                            "MAN:\"ssdp:discover\"\r\n"
                            "MX:3\r\n"
                            "\r\n\0";

    qCDebug(UPNPQT_DISCOVER) << "Sending" << upnp_data;
    qCDebug(UPNPQT_DISCOVER) << "Sending" << tr64_data;

    d->udpSocket.writeDatagram(upnp_data, qstrlen(upnp_data), QHostAddress(QStringLiteral("239.255.255.250")), 1900);
    d->udpSocket.writeDatagram(tr64_data, qstrlen(tr64_data), QHostAddress(QStringLiteral("239.255.255.250")), 1900);
}

void DiscoverPrivate::joinUPnPMGroup(int fd)
{
    struct ip_mreq mreq;
    memset(&mreq,0,sizeof(struct ip_mreq));

    inet_aton("239.255.255.250",&mreq.imr_multiaddr);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

#ifndef Q_WS_WIN
    if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(struct ip_mreq)) < 0)
#else
    if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char *)&mreq,sizeof(struct ip_mreq)) < 0)
#endif
    {
        qCWarning(UPNPQT_DISCOVER) << "Failed to join multicast group 239.255.255.250";
    }
}

void DiscoverPrivate::leaveUPnPMGroup(int fd)
{
    struct ip_mreq mreq;
    memset(&mreq,0,sizeof(struct ip_mreq));

    inet_aton("239.255.255.250",&mreq.imr_multiaddr);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

#ifndef Q_WS_WIN
    if (setsockopt(fd,IPPROTO_IP,IP_DROP_MEMBERSHIP,&mreq,sizeof(struct ip_mreq)) < 0)
#else
    if (setsockopt(fd,IPPROTO_IP,IP_DROP_MEMBERSHIP,(char *)&mreq,sizeof(struct ip_mreq)) < 0)
#endif
    {
        qCWarning(UPNPQT_DISCOVER) << "Failed to leave multicast group 239.255.255.250";
    }
}

void DiscoverPrivate::parse(const QByteArray &data, Discover *parent)
{
    QStringList lines = QString::fromLatin1(data).split(QStringLiteral("\r\n"));
    QString server;
    QUrl location;

    // first read first line and see if contains a HTTP 200 OK message
    QString line = lines.first();
    if (!line.contains(QLatin1String("HTTP"), Qt::CaseInsensitive)) {
        // it is either a 200 OK or a NOTIFY
        if (!line.contains(QLatin1String("NOTIFY"), Qt::CaseInsensitive) && !line.contains(QLatin1String("200"))) {
            return;
        }
    } else if (line.contains(QLatin1String("M-SEARCH"), Qt::CaseInsensitive)) { // ignore M-SEARCH
        return;
    }

    // quick check that the response being parsed is valid
    bool validDevice = false;
    for (int idx = 0; idx < lines.count() && !validDevice; ++idx) {
        line = lines[idx];
        if ((line.contains(QLatin1String("ST:"), Qt::CaseInsensitive) ||
             line.contains(QLatin1String("NT:"), Qt::CaseInsensitive)) &&
                line.contains(QLatin1String("InternetGatewayDevice"), Qt::CaseInsensitive)) {
            validDevice = true;
        }
    }

    if (!validDevice) {
        qCDebug(UPNPQT_DISCOVER) << "Not a valid Internet Gateway Device" << server << location;
        return;
    }

    // read all lines and try to find the server and location fields
    for (int i = 1;i < lines.count();i++) {
        line = lines[i];
        if (line.startsWith(QLatin1String("Location"), Qt::CaseInsensitive)) {
            location = QUrl(line.mid(line.indexOf(QLatin1Char(':')) + 1).trimmed());
            if (!location.isValid()) {
                return;
            }
        } else if (line.startsWith(QLatin1String("Server"), Qt::CaseInsensitive)) {
            server = line.mid(line.indexOf(QLatin1Char(':')) + 1).trimmed();
            if (server.length() == 0) {
                return;
            }
        }
    }

    qDebug(UPNPQT_DISCOVER) << "Detected IGD " << server << location << ", downloading it's XML file.";
    QNetworkRequest request(location);
    QNetworkReply *reply = nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [=] {
        reply->deleteLater();

        const QByteArray data = reply->readAll();
        qDebug(UPNPQT_DISCOVER) << "downloaded XML" << server << location << data.constData();
        if (!reply->error()) {
            Device *dev = Device::fromXml(data, parent);
            if (dev) {
                if (dev->urlBase().isEmpty()) {
                    dev->setUrlBase(location.toString());
                }
                Q_EMIT q_ptr->discovered(dev);
            }
        }
    });
}

#include "moc_discover.cpp"
#include "discover.moc"
