/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwNetworkIO/config.hpp"
#include "fwNetworkIO/exceptions/ConnectionRefused.hpp"
#include "fwNetworkIO/exceptions/ContentNotFound.hpp"
#include "fwNetworkIO/exceptions/HostNotFound.hpp"
#include "fwNetworkIO/http/Request.hpp"

#include <QNetworkReply>
#include <QtNetwork>

namespace fwNetworkIO
{
namespace http
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
class FWNETWORKIO_CLASS_API ClientQt : public QObject
{
Q_OBJECT

public:
    /**
     * Constructor/Destructor
     * @{
     */
    FWNETWORKIO_API ClientQt();
    FWNETWORKIO_API virtual ~ClientQt();
    /**  @} */

    /**
     * @brief Retrieves data over network
     * @param request the request
     */
    FWNETWORKIO_API QByteArray get(Request::sptr request);

    /**
     * @brief Retrieves data over network
     * @param request the request
     */
    FWNETWORKIO_API std::string getFile(Request::sptr request);

    /**
     * @brief Performs head request
     * @param request the request
     * @return headers resulting of the request
     */
    FWNETWORKIO_API Request::HeadersType head(Request::sptr request);

    /**
     * @brief Performs POST request
     * @param request the request
     * @return body The body content
     */
    FWNETWORKIO_API QByteArray post(Request::sptr request, const QByteArray& body);

public Q_SLOTS:
    /// Slot triggered when an error occurs.
    void processError(QNetworkReply::NetworkError errorCode);

private:
    /// Set request headers with given values.
    void computeHeaders(QNetworkRequest& request,  const Request::HeadersType& headers);

    // Performs requests.
    QNetworkAccessManager* m_networkManager;

};

} // namespace http
} // namespace fwNetworkIO
