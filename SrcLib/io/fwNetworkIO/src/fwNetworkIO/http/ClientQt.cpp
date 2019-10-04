/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwNetworkIO/http/ClientQt.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwTools/System.hpp>
#include <fwTools/UUID.hpp>

#include <filesystem>
#include <QList>
#include <QtNetwork>

namespace fwNetworkIO
{
namespace http
{

ClientQt::ClientQt()
{
}

//-----------------------------------------------------------------------------

ClientQt::~ClientQt()
{
}

//-----------------------------------------------------------------------------

QByteArray ClientQt::get(Request::sptr request)
{
    QNetworkAccessManager networkManager;
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QNetworkReply* reply = networkManager.get(qtRequest);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(reply, QOverload<QNetworkReply::NetworkError>::of(
                         &QNetworkReply::error), this, &ClientQt::processError);

    loop.exec();
    QByteArray data = reply->readAll();
    reply->deleteLater();
    return data;
}

//-----------------------------------------------------------------------------

std::string ClientQt::getFile(Request::sptr request)
{
    QNetworkAccessManager networkManager;
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QNetworkReply* reply = networkManager.get(qtRequest);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(reply, QOverload<QNetworkReply::NetworkError>::of(
                         &QNetworkReply::error), this, &ClientQt::processError);

    std::filesystem::path folderPath = ::fwTools::System::getTemporaryFolder();
    std::filesystem::path filePath   = folderPath / ::fwTools::UUID::generateUUID();

    QFile file(filePath.string().c_str());

    while(file.exists())
    {
        filePath = folderPath / ::fwTools::UUID::generateUUID();
        file.setFileName(filePath.string().c_str());
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        throw ("Could not create a temporary file");
    }

    QObject::connect(reply, &QNetworkReply::readyRead,  [&]  { file.write(reply->readAll()); } );

    loop.exec();
    file.write(reply->readAll());
    reply->deleteLater();

    return filePath.string();
}

//-----------------------------------------------------------------------------

void ClientQt::processError(QNetworkReply::NetworkError errorCode)
{
    QMetaObject metaObject = QNetworkReply::staticMetaObject;
    QMetaEnum metaEnum     = metaObject.enumerator(metaObject.indexOfEnumerator("NetworkError"));
    const char* desc       = metaEnum.valueToKey(errorCode);

    switch(errorCode)
    {
        case QNetworkReply::ConnectionRefusedError:
            throw ::fwNetworkIO::exceptions::ConnectionRefused(desc);
            break;
        case QNetworkReply::HostNotFoundError:
            throw ::fwNetworkIO::exceptions::HostNotFound(desc);
            break;
        case QNetworkReply::ContentNotFoundError:
            throw ::fwNetworkIO::exceptions::ContentNotFound(desc);
            break;
        default:
            throw ::fwNetworkIO::exceptions::Base(desc);
            break;
    }
}

//-----------------------------------------------------------------------------

QByteArray ClientQt::post(Request::sptr request, const QByteArray& body)
{
    QNetworkAccessManager networkManager;
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    qtRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    this->computeHeaders(qtRequest, request->getHeaders());

    QNetworkReply* reply = networkManager.post(qtRequest, body);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, QOverload<QNetworkReply::NetworkError>::of(
                         &QNetworkReply::error), this, &ClientQt::processError);
    loop.exec();
    QByteArray data = reply->readAll();
    reply->deleteLater();
    return data;
}

//-----------------------------------------------------------------------------

void ClientQt::computeHeaders(QNetworkRequest& request,  const Request::HeadersType& headers)
{
    Request::HeadersType::const_iterator cIt = headers.begin();
    for(; cIt != headers.end(); ++cIt)
    {
        request.setRawHeader(cIt->first.c_str(), cIt->second.c_str());
    }
}

//-----------------------------------------------------------------------------

Request::HeadersType ClientQt::head(Request::sptr request)
{
    QNetworkAccessManager networkManager;
    Request::HeadersType headers;

    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QNetworkReply* reply = networkManager.head(qtRequest);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    const QList< QNetworkReply::RawHeaderPair>& rawHeaders = reply->rawHeaderPairs();

    QList< QNetworkReply::RawHeaderPair>::const_iterator cIt = rawHeaders.begin();

    for(; cIt != rawHeaders.end(); ++cIt)
    {
        headers[cIt->first.data()] = cIt->second.data();
    }
    reply->deleteLater();
    return headers;
}

//-----------------------------------------------------------------------------

} // namespace http

} // namespace fwNetworkIO
