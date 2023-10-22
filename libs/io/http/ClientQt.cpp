/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ClientQt.hpp"

#include <core/os/temp_path.hpp>

#include <ui/__/dialog/message.hpp>

#include <QList>
#include <QtNetwork>

#include <filesystem>

namespace sight::io::http
{

ClientQt::ClientQt()
= default;

//-----------------------------------------------------------------------------

ClientQt::~ClientQt()
= default;

//-----------------------------------------------------------------------------

QByteArray ClientQt::get(Request::sptr _request)
{
    QNetworkAccessManager network_manager;
    const QUrl qt_url(QString::fromStdString(_request->getUrl()));
    QNetworkRequest qt_request(qt_url);

    sight::io::http::ClientQt::computeHeaders(qt_request, _request->getHeaders());

    QNetworkReply* reply = network_manager.get(qt_request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(
        reply,
        QOverload<QNetworkReply::NetworkError>::of(
            &QNetworkReply::errorOccurred
        ),
        this,
        &ClientQt::processError
    );

    loop.exec();
    QByteArray data = reply->readAll();
    reply->deleteLater();
    return data;
}

//-----------------------------------------------------------------------------

std::string ClientQt::get_file(Request::sptr _request)
{
    QNetworkAccessManager network_manager;
    const QUrl qt_url(QString::fromStdString(_request->getUrl()));
    QNetworkRequest qt_request(qt_url);

    sight::io::http::ClientQt::computeHeaders(qt_request, _request->getHeaders());

    QNetworkReply* reply = network_manager.get(qt_request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(
        reply,
        QOverload<QNetworkReply::NetworkError>::of(
            &QNetworkReply::errorOccurred
        ),
        this,
        &ClientQt::processError
    );

    std::filesystem::path file_path = core::os::temp_file::unique_path();
    QFile file(file_path.string().c_str());

    if(!file.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error("Could not create a temporary file");
    }

    QObject::connect(reply, &QNetworkReply::readyRead, [&]{file.write(reply->readAll());});

    loop.exec();
    file.write(reply->readAll());
    reply->deleteLater();

    return file_path.string();
}

//-----------------------------------------------------------------------------

void ClientQt::processError(QNetworkReply::NetworkError _error_code)
{
    QMetaObject meta_object = QNetworkReply::staticMetaObject;
    QMetaEnum meta_enum     = meta_object.enumerator(meta_object.indexOfEnumerator("NetworkError"));
    const char* desc        = meta_enum.valueToKey(_error_code);

    switch(_error_code)
    {
        case QNetworkReply::ConnectionRefusedError:
            throw io::http::exceptions::ConnectionRefused(desc);
            break;

        case QNetworkReply::HostNotFoundError:
            throw io::http::exceptions::HostNotFound(desc);
            break;

        case QNetworkReply::ContentNotFoundError:
            throw io::http::exceptions::ContentNotFound(desc);
            break;

        default:
            throw io::http::exceptions::Base(desc);
            break;
    }
}

//-----------------------------------------------------------------------------

QByteArray ClientQt::post(Request::sptr _request, const QByteArray& _body)
{
    QNetworkAccessManager network_manager;
    const QUrl qt_url(QString::fromStdString(_request->getUrl()));
    QNetworkRequest qt_request(qt_url);

    qt_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    sight::io::http::ClientQt::computeHeaders(qt_request, _request->getHeaders());

    QNetworkReply* reply = network_manager.post(qt_request, _body);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(
        reply,
        QOverload<QNetworkReply::NetworkError>::of(
            &QNetworkReply::errorOccurred
        ),
        this,
        &ClientQt::processError
    );
    loop.exec();
    QByteArray data = reply->readAll();
    reply->deleteLater();
    return data;
}

//-----------------------------------------------------------------------------

void ClientQt::computeHeaders(QNetworkRequest& _request, const Request::headers_t& _headers)
{
    auto c_it = _headers.begin();
    for( ; c_it != _headers.end() ; ++c_it)
    {
        _request.setRawHeader(c_it->first.c_str(), c_it->second.c_str());
    }
}

//-----------------------------------------------------------------------------

Request::headers_t ClientQt::head(Request::sptr _request)
{
    QNetworkAccessManager network_manager;
    Request::headers_t headers;

    const QUrl qt_url(QString::fromStdString(_request->getUrl()));
    QNetworkRequest qt_request(qt_url);

    this->computeHeaders(qt_request, _request->getHeaders());

    QNetworkReply* reply = network_manager.head(qt_request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    const QList<QNetworkReply::RawHeaderPair>& raw_headers = reply->rawHeaderPairs();

    QList<QNetworkReply::RawHeaderPair>::const_iterator c_it = raw_headers.begin();

    for( ; c_it != raw_headers.end() ; ++c_it)
    {
        headers[c_it->first.data()] = c_it->second.data();
    }

    reply->deleteLater();
    return headers;
}

//-----------------------------------------------------------------------------

} // namespace sight::io::http
