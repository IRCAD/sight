/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwNetworkIO/http/ClientQt.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwTools/System.hpp>
#include <fwTools/UUID.hpp>

#include <boost/filesystem/operations.hpp>

#include <QList>
#include <QtNetwork>

namespace fwNetworkIO
{
namespace http
{

ClientQt::ClientQt()
{
    m_networkManager = new QNetworkAccessManager();
}

//-----------------------------------------------------------------------------

ClientQt::~ClientQt()
{
}

//-----------------------------------------------------------------------------

QByteArray ClientQt::get(Request::sptr request)
{
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QNetworkReply* reply = m_networkManager->get(qtRequest);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(reply, QOverload<QNetworkReply::NetworkError>::of(
                         &QNetworkReply::error), this, &ClientQt::processError);

    loop.exec();
    return reply->readAll();
}

//-----------------------------------------------------------------------------

std::string ClientQt::getFile(Request::sptr request)
{
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QNetworkReply* reply = m_networkManager->get(qtRequest);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(reply, QOverload<QNetworkReply::NetworkError>::of(
                         &QNetworkReply::error), this, &ClientQt::processError);

    ::boost::filesystem::path folderPath = ::fwTools::System::getTemporaryFolder();
    ::boost::filesystem::path filePath   = folderPath / ::fwTools::UUID::generateUUID();

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

    QObject::connect(reply, &QNetworkReply::readyRead, this, [&]  { file.write(reply->readAll()); } );

    loop.exec();
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
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    qtRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    this->computeHeaders(qtRequest, request->getHeaders());

    QNetworkReply* replay = m_networkManager->post(qtRequest, body);
    QEventLoop loop;
    QObject::connect(replay, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    const QByteArray& answer = replay->readAll();
    return answer.data();
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
    Request::HeadersType headers;

    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QNetworkReply* replay = m_networkManager->head(qtRequest);
    QEventLoop loop;
    QObject::connect(replay, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    const QList< QNetworkReply::RawHeaderPair>& rawHeaders = replay->rawHeaderPairs();

    QList< QNetworkReply::RawHeaderPair>::const_iterator cIt = rawHeaders.begin();

    for(; cIt != rawHeaders.end(); ++cIt)
    {
        headers[cIt->first.data()] = cIt->second.data();
    }

    return headers;
}

//-----------------------------------------------------------------------------

} // namespace http

} // namespace fwNetworkIO
