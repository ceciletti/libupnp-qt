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
#ifndef UPNPSOAPENVELOPE_H
#define UPNPSOAPENVELOPE_H

#include <QString>

class QUrl;
class QNetworkRequest;
class QXmlStreamWriter;

namespace UpnpQt {

class SoapEnvelope
{
public:
    SoapEnvelope(const QString &action, const QString &actionNamespace);
    ~SoapEnvelope();

    void writeTextElement(const QString &qualifiedName, const QString &text);

    QByteArray render();
    QNetworkRequest request(const QUrl &url) const;

    /**
     * returns errorCode and errorDescription
     */
    static std::pair<QString, QString> responseError(const QByteArray &data);

private:
    QXmlStreamWriter *m_stream;
    QString m_action;
    QString m_actionNamespace;
    QByteArray m_data;
    bool m_open = true;
};

}

#endif // UPNPSOAPENVELOPE_H
