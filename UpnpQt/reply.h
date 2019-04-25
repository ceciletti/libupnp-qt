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
#ifndef UPNPREPLY_H
#define UPNPREPLY_H

#include <QObject>

#include <QVariant>

#include <UpnpQt/global.h>

namespace UpnpQt {

class UPNPQT_LIBRARY Reply : public QObject
{
    Q_OBJECT
public:
    explicit Reply(QObject *parent = nullptr);
    virtual ~Reply();

    bool error() const;
    QString errorCode() const;
    QString errorString() const;
    QVariant value() const;

    void finish();
    void finishWithData(const QVariant &data);
    void finishWithError(const QString &msg, const QString &code = QString());
    void finishWithErrorLater(const QString &msg, const QString &code = QString());

Q_SIGNALS:
    void finished(Reply *reply);

private:
    QVariant m_value;
    QString m_errorCode;
    QString m_errorString;
    bool m_error = false;
};

}

#endif // UPNPREPLY_H
