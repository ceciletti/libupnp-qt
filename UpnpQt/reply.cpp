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
#include "reply.h"

#include <QTimer>

using namespace UpnpQt;

Reply::Reply(QObject *parent) : QObject (parent)
{

}

Reply::~Reply()
{

}

bool Reply::error() const
{
    return m_error;
}

QString Reply::errorCode() const
{
    return m_errorCode;
}

QString Reply::errorString() const
{
    return m_errorString;
}

QVariant Reply::value() const
{
    return m_value;
}

void Reply::finish()
{
    Q_EMIT finished(this);
}

void Reply::finishWithData(const QVariant &data)
{
    m_value = data;
    Q_EMIT finished(this);
}

void Reply::finishWithError(const QString &msg, const QString &code)
{
    m_error = true;
    m_errorString = msg;
    m_errorCode = code;
    Q_EMIT finished(this);
}

void Reply::finishWithErrorLater(const QString &msg, const QString &code)
{
    m_error = true;
    m_errorString = msg;
    m_errorCode = code;
    QTimer::singleShot(0, this, [this] {
       Q_EMIT finished(this);
    });
}

#include "moc_reply.cpp"
