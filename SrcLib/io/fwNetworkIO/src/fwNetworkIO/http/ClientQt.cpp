/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwNetworkIO/http/ClientQt.hpp"

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

std::string ClientQt::post(Request::sptr request, const std::string& content)
{
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QString byteArray     = QString::fromStdString(content);
    QNetworkReply* replay = m_networkManager->post(qtRequest, byteArray.toUtf8());
    QEventLoop loop;
    QObject::connect(replay, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(replay, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(processError(QNetworkReply::NetworkError)));
    loop.exec();
    const QByteArray& answer = replay->readAll();

    return answer.data();
}

//-----------------------------------------------------------------------------

QByteArray ClientQt::get(Request::sptr request)
{
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QNetworkReply* reply = m_networkManager->get(qtRequest);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(processError(QNetworkReply::NetworkError)));

    loop.exec();
    return reply->readAll();
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

bool ClientQt::get(Request::sptr request, char* buffer, size_t size)
{
    HTTPResponse resp;
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QNetworkReply* reply = m_networkManager->get(qtRequest);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    qint64 readValue = reply->read(buffer, size);

    return (readValue >= 0);
}

//-----------------------------------------------------------------------------

std::string ClientQt::put(Request::sptr request, const std::string& content)
{
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QString byteArray     = QString::fromStdString(content);
    QNetworkReply* replay = m_networkManager->put(qtRequest, byteArray.toUtf8());
    QEventLoop loop;
    QObject::connect(replay, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    const QByteArray& answer = replay->readAll();
    return answer.data();
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

void ClientQt::putAsync(Request::sptr request, const std::string& content)
{
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QString byteArray     = QString::fromStdString(content);
    QNetworkReply* replay = m_networkManager->put(qtRequest, byteArray.toUtf8());

    QObject::connect(replay, SIGNAL(finished()), this, SLOT(onPutAsyncFinished()));
}

//-----------------------------------------------------------------------------

std::string ClientQt::put(Request::sptr request, char* buffer, size_t size)
{
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QByteArray byteArray(buffer, size);
    QNetworkReply* replay = m_networkManager->put(qtRequest, byteArray);

    QEventLoop loop;
    QObject::connect(replay, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QByteArray answer = replay->readAll();
    return answer.data();
}

//-----------------------------------------------------------------------------

void ClientQt::putAsync(Request::sptr request, char* buffer, size_t size)
{
    const QUrl qtUrl(QString::fromStdString(request->getUrl()));
    QNetworkRequest qtRequest(qtUrl);

    this->computeHeaders(qtRequest, request->getHeaders());

    QByteArray byteArray(buffer, size);
    QNetworkReply* replay = m_networkManager->put(qtRequest, byteArray);
    QObject::connect(replay, SIGNAL(finished()), this, SLOT(onPutAsyncFinished()));
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

void ClientQt::onPutAsyncFinished()
{
    SLM_TRACE("Put async. finished.");
}

//-----------------------------------------------------------------------------

} // namespace http

} // namespace fwNetworkIO
