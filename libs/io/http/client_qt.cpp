/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "client_qt.hpp"

#include <core/os/temp_path.hpp>

#include <ui/__/dialog/message.hpp>

#include <QList>
#include <QtNetwork>

#include <filesystem>

namespace sight::io::http
{

client_qt::client_qt()
= default;

//-----------------------------------------------------------------------------

client_qt::~client_qt()
= default;

//-----------------------------------------------------------------------------

QByteArray client_qt::get(request::sptr _request)
{
    QNetworkAccessManager network_manager;
    const QUrl qt_url(QString::fromStdString(_request->get_url()));
    QNetworkRequest qt_request(qt_url);

    sight::io::http::client_qt::compute_headers(qt_request, _request->get_headers());

    QNetworkReply* reply = network_manager.get(qt_request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(
        reply,
        QOverload<QNetworkReply::NetworkError>::of(
            &QNetworkReply::errorOccurred
        ),
        this,
        &client_qt::process_error
    );

    loop.exec();
    QByteArray data = reply->readAll();
    reply->deleteLater();
    return data;
}

//-----------------------------------------------------------------------------

std::string client_qt::get_file(request::sptr _request)
{
    QNetworkAccessManager network_manager;
    const QUrl qt_url(QString::fromStdString(_request->get_url()));
    QNetworkRequest qt_request(qt_url);

    sight::io::http::client_qt::compute_headers(qt_request, _request->get_headers());

    QNetworkReply* reply = network_manager.get(qt_request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(
        reply,
        QOverload<QNetworkReply::NetworkError>::of(
            &QNetworkReply::errorOccurred
        ),
        this,
        &client_qt::process_error
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

void client_qt::process_error(QNetworkReply::NetworkError _error_code)
{
    QMetaObject meta_object = QNetworkReply::staticMetaObject;
    QMetaEnum meta_enum     = meta_object.enumerator(meta_object.indexOfEnumerator("NetworkError"));
    const char* desc        = meta_enum.valueToKey(_error_code);

    switch(_error_code)
    {
        case QNetworkReply::ConnectionRefusedError:
            throw io::http::exceptions::connection_refused(desc);
            break;

        case QNetworkReply::HostNotFoundError:
            throw io::http::exceptions::host_not_found(desc);
            break;

        case QNetworkReply::ContentNotFoundError:
            throw io::http::exceptions::content_not_found(desc);
            break;

        default:
            throw io::http::exceptions::base(desc);
            break;
    }
}

//-----------------------------------------------------------------------------

QByteArray client_qt::post(request::sptr _request, const QByteArray& _body)
{
    QNetworkAccessManager network_manager;
    const QUrl qt_url(QString::fromStdString(_request->get_url()));
    QNetworkRequest qt_request(qt_url);

    qt_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    sight::io::http::client_qt::compute_headers(qt_request, _request->get_headers());

    QNetworkReply* reply = network_manager.post(qt_request, _body);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(
        reply,
        &QNetworkReply::errorOccurred,
        this,
        &client_qt::process_error
    );
    loop.exec();
    QByteArray data = reply->readAll();
    delete reply;
    return data;
}

//-----------------------------------------------------------------------------

void client_qt::compute_headers(QNetworkRequest& _request, const request::headers_t& _headers)
{
    auto c_it = _headers.begin();
    for( ; c_it != _headers.end() ; ++c_it)
    {
        _request.setRawHeader(c_it->first.c_str(), c_it->second.c_str());
    }
}

//-----------------------------------------------------------------------------

request::headers_t client_qt::head(request::sptr _request)
{
    QNetworkAccessManager network_manager;
    request::headers_t headers;

    const QUrl qt_url(QString::fromStdString(_request->get_url()));
    QNetworkRequest qt_request(qt_url);

    client_qt::compute_headers(qt_request, _request->get_headers());

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
