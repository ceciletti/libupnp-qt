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
#include "soapenvelope.h"
#include "config.h"

#include <QXmlStreamWriter>
#include <QNetworkRequest>
#include <QCoreApplication>
#include <QDomDocument>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(UPNPQT_SOAP, "upnpqt.soap", QtInfoMsg)

using namespace UpnpQt;

SoapEnvelope::SoapEnvelope(const QString &action, const QString &actionNamespace)
    : m_stream(new QXmlStreamWriter(&m_data))
    , m_action(action)
    , m_actionNamespace(actionNamespace)
{
    m_stream->setAutoFormatting(true);

    m_stream->writeStartDocument();

    const QString envelopeNS = QStringLiteral("http://schemas.xmlsoap.org/soap/envelope/");
    m_stream->writeNamespace(envelopeNS, QStringLiteral("s"));
    m_stream->writeStartElement(envelopeNS, QStringLiteral("Envelope"));
    m_stream->writeAttribute(envelopeNS, QStringLiteral("encodingStyle"), QStringLiteral("http://schemas.xmlsoap.org/soap/encoding/"));

    m_stream->writeStartElement(envelopeNS, QStringLiteral("Body"));

    m_stream->writeStartElement(QLatin1String("u:") + action);
    m_stream->writeNamespace(actionNamespace, QStringLiteral("u"));
}

SoapEnvelope::~SoapEnvelope()
{
    delete m_stream;
}

void SoapEnvelope::writeTextElement(const QString &qualifiedName, const QString &text)
{
    if (m_open) {
        m_stream->writeTextElement(qualifiedName, text);
    }
}

QByteArray SoapEnvelope::render()
{
    if (m_open) {
        m_stream->writeEndElement(); // ACTION

        m_stream->writeEndElement(); // Body

        m_stream->writeEndElement(); // Envelope

        m_stream->writeEndDocument(); // XML

        m_open = false;
    }

    return m_data;
}

QNetworkRequest SoapEnvelope::request(const QUrl &url) const
{
    QNetworkRequest req(url);
    static const QByteArray ua =
            QCoreApplication::applicationName().toLatin1() + '/' + QCoreApplication::applicationVersion().toLatin1() + ", libupnp-qt/" + UPNPQT_VERSION;
    req.setHeader(QNetworkRequest::ContentTypeHeader, QByteArrayLiteral("text/xml"));
    req.setHeader(QNetworkRequest::UserAgentHeader, ua);
    req.setRawHeader(QByteArrayLiteral("SOAPAction"), m_actionNamespace.toLatin1() + '#' + m_action.toLatin1());
    return req;
}

std::pair<QString, QString> parseUPnPError(QXmlStreamReader &xml)
{
    std::pair<QString, QString> ret;
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType type = xml.readNext();
//        qCDebug(UPNPQT_SOAP) << "next" << type << xml.dtdName() << xml.text() << xml.name();
        if (type == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("errorCode")) {
                ret.first = xml.readElementText();
            } else if (xml.name() == QLatin1String("errorDescription")) {
                ret.second = xml.readElementText();
            } else {
                xml.skipCurrentElement();
            }
        }
    }
    return ret;
}

std::pair<QString, QString> parseDetail(QXmlStreamReader &xml)
{
    std::pair<QString, QString> ret;
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType type = xml.readNext();
//        qCDebug(UPNPQT_SOAP) << "next" << type << xml.dtdName() << xml.text() << xml.name();
        if (type == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("UPnPError")) {
                ret = parseUPnPError(xml);
            } else {
                xml.skipCurrentElement();
            }
        }
    }
    return ret;
}

std::pair<QString, QString> parseFault(QXmlStreamReader &xml)
{
    std::pair<QString, QString> ret;
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType type = xml.readNext();
//        qCDebug(UPNPQT_SOAP) << "next" << type << xml.dtdName() << xml.text() << xml.name();
        if (type == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("detail")) {
                ret = parseDetail(xml);
            } else {
                xml.skipCurrentElement();
            }
        }
    }
    return ret;
}

std::pair<QString, QString> parseBody(QXmlStreamReader &xml)
{
    std::pair<QString, QString> ret;
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType type = xml.readNext();
//        qCDebug(UPNPQT_SOAP) << "next" << type << xml.dtdName() << xml.text() << xml.name();
        if (type == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("Fault")) {
                ret = parseFault(xml);
            } else {
                xml.skipCurrentElement();
            }
        }
    }
    return ret;
}

std::pair<QString, QString> parseEnvelope(QXmlStreamReader &xml)
{
    std::pair<QString, QString> ret;
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType type = xml.readNext();
//        qCDebug(UPNPQT_SOAP) << "next" << type << xml.dtdName() << xml.text() << xml.name();
        if (type == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("Body")) {
                ret = parseBody(xml);
            } else {
                xml.skipCurrentElement();
            }
        }
    }
    return ret;
}

std::pair<QString, QString> SoapEnvelope::responseError(const QByteArray &data)
{
    std::pair<QString, QString> ret;
    QXmlStreamReader xml(data);
    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType type = xml.readNext();
//        qCDebug(UPNPQT_SOAP) << "next" << type << xml.dtdName() << xml.text() << xml.name();
        if (type == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("Envelope")) {
                ret = parseEnvelope(xml);
            } else {
                xml.skipCurrentElement();
            }
        }
    }
    return ret;
}
