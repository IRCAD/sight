/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#pragma once

#include <sight/io/http/config.hpp>

#include "io/http/exceptions/connection_refused.hpp"
#include "io/http/exceptions/content_not_found.hpp"
#include "io/http/exceptions/host_not_found.hpp"
#include "io/http/request.hpp"

#include <QNetworkReply>
#include <QtNetwork>

namespace sight::io::http
{

/**
 * @brief This structure represent an HTTP response.
 */
struct http_response
{
    unsigned int status_code;
    std::string message;
};

/**
 * @brief HTTP client using Qt Network.
 */
class SIGHT_IO_HTTP_CLASS_API client_qt : public QObject
{
Q_OBJECT

public:

    /**
     * Constructor/Destructor
     * @{
     */
    SIGHT_IO_HTTP_API client_qt();
    SIGHT_IO_HTTP_API ~client_qt() override;
    /**  @} */

    /**
     * @brief Retrieves data over network
     * @param _request the request
     */
    SIGHT_IO_HTTP_API QByteArray get(request::sptr _request);

    /**
     * @brief Retrieves data over network
     * @param _request the request
     */
    SIGHT_IO_HTTP_API std::string get_file(request::sptr _request);

    /**
     * @brief Performs head request
     * @param _request the request
     * @return headers resulting of the request
     */
    SIGHT_IO_HTTP_API static request::headers_t head(request::sptr _request);

    /**
     * @brief Performs POST request
     * @param _request the request
     * @param _body The body content
     */
    SIGHT_IO_HTTP_API QByteArray post(request::sptr _request, const QByteArray& _body);

public Q_SLOTS:

    /// Slot triggered when an error occurs.
    static void process_error(QNetworkReply::NetworkError _error_code);

private:

    /// Set request headers with given values.
    static void compute_headers(QNetworkRequest& _request, const request::headers_t& _headers);
};

} // namespace sight::io::http
