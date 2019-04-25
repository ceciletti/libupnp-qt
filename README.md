# libupnp-qt
Simple UPnP library based on Qt

## Current Features
* Discovering IGD (Internet Gateway Device)
  * Creating a Port Map
  * Deleting a Port Map
  * Querying a Port Map
  * Querying all Port Maps
  * Querying current external IP
  
## Usage

``` cpp
auto s = new Discover;
s->discoverInternetGatewayDevice();
connect(s, &Discover::discovered, this, [=] (Device *device) {
    auto idg = qobject_cast<InternetGatewayDevice*>(device);
    if (idg) {
        WanConnectionService *srv = idg->wanIpOrPppConnectionService();
        if (srv) {
            Reply *reply;
            reply = srv->getExternalIp();
            connect(reply, &Reply::finished, this, [=] {
                qDebug() << "Got" << reply->error() << reply->errorString() << reply->value();
            });

            reply = srv->addPortMapping(3004, QHostAddress("192.168.15.50"), 3004, QAbstractSocket::TcpSocket, "Cutelyst-3004", true, 0);
            connect(reply, &Reply::finished, this, [=] {
                qDebug() << "Got" << reply->error() << reply->errorString() << reply->value();
            });

            reply = srv->getSpecificPortMappingEntry(3004, QAbstractSocket::TcpSocket);
            connect(reply, &Reply::finished, this, [=] {
                auto map = reply->value().value<WanConnectionService::PortMap>();
                qDebug() << "Got" << reply->error() << reply->errorString() << reply->value() << map.leaseDuration << map.description;
            });

            reply = srv->getGenericPortMapping();
            connect(reply, &Reply::finished, this, [=] {
                qDebug() << "Got" << reply->error() << reply->errorString() << reply->value();
                auto maps = reply->value().value<std::vector<WanConnectionService::PortMap>>();
                for (const WanConnectionService::PortMap &map : maps) {
                    qDebug() << map.externalPort << map.internalAddress << map.internalPort << map.sockType << map.remoteHost << map.leaseDuration << map.description;
                }
            });

            reply = srv->deletePortMapping(3004, QAbstractSocket::TcpSocket);
            connect(reply, &Reply::finished, this, [=] {
                qDebug() << "Got" << reply->error() << reply->errorString() << reply->value();
            });
        }
    }
});
```
