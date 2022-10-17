/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/http/config.hpp"
#include "io/http/exceptions/ConnectionRefused.hpp"
#include "io/http/exceptions/ContentNotFound.hpp"
#include "io/http/exceptions/HostNotFound.hpp"
#include "io/http/Request.hpp"

#include <QNetworkReply>
#include <QtNetwork>

namespace sight::io::http
{

/**
 * @brief This structure represent an HTTP response.
 */
struct HTTPResponse
{
    unsigned int status_code;
    std::string message;
};

/**
 * @brief HTTP client using Qt Network.
 */
class IO_HTTP_CLASS_API ClientQt : public QObject
{
Q_OBJECT

public:

    /**
     * Constructor/Destructor
     * @{
     */
    IO_HTTP_API ClientQt();
    IO_HTTP_API ~ClientQt() override;
    /**  @} */

    /**
     * @brief Retrieves data over network
     * @param request the request
     */
    IO_HTTP_API QByteArray get(Request::sptr request);

    /**
     * @brief Retrieves data over network
     * @param request the request
     */
    IO_HTTP_API std::string getFile(Request::sptr request);

    /**
     * @brief Performs head request
     * @param request the request
     * @return headers resulting of the request
     */
    IO_HTTP_API Request::HeadersType head(Request::sptr request);

    /**
     * @brief Performs POST request
     * @param request the request
     * @return body The body content
     */
    IO_HTTP_API QByteArray post(Request::sptr request, const QByteArray& body);

public Q_SLOTS:

    /// Slot triggered when an error occurs.
    static void processError(QNetworkReply::NetworkError errorCode);

private:

    /// Set request headers with given values.
    static void computeHeaders(QNetworkRequest& request, const Request::HeadersType& headers);
};

} // namespace sight::io::http
