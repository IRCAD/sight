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
     * @brief Sends data to the server using post
     * @param request the request to post
     * @param content has the data to send
     */
    FWNETWORKIO_API virtual std::string post(Request::sptr request, const std::string& content);

    /**
     * @brief Retrieves data over network
     * @param request the request
     */
    FWNETWORKIO_API virtual QByteArray get(Request::sptr request);

    /**
     * @brief Retrieves data over network
     * @param buffer the buffer where data is written
     * @param size buffer size
     * @return true if the reply has been read successfully
     */
    FWNETWORKIO_API virtual bool get(Request::sptr request, char* buffer, size_t size);

    /**
     * @brief Puts HTTP request
     * @param url destination of data
     * @param content the data to put
     * @return answer data
     */
    FWNETWORKIO_API virtual std::string put(Request::sptr url, const std::string& content);

    /**
     * @brief Puts HTTP request asynchronously
     * @param url destination of data
     * @param content the data to put
     */
    FWNETWORKIO_API virtual void putAsync(Request::sptr url, const std::string& content);

    /**
     * @brief Puts binary file
     * @param request the request
     * @param buffer reference to the buffer
     * @param size buffer size
     */
    FWNETWORKIO_API virtual std::string put(Request::sptr request, char* buffer, size_t size);

    /**
     * @brief Puts binary file asynchronously
     * @param request the request
     * @param buffer reference to the buffer;
     * @param size buffer size
     */
    FWNETWORKIO_API virtual void putAsync(Request::sptr request, char* buffer, size_t size);

    /**
     * @brief Performs head request
     * @param request the request
     * @return headers resulting of the request
     */
    FWNETWORKIO_API virtual Request::HeadersType head(Request::sptr request);

    /**
     * @brief Performs POST request
     * @param request the request
     * @return body The body content
     */
    FWNETWORKIO_API QByteArray post(Request::sptr request, const QByteArray& body);

public Q_SLOTS:
    /// Slot triggered when an error occurs.
    void processError(QNetworkReply::NetworkError errorCode);

    /// Slot triggered when an asynchronous requested has completed.
    void onPutAsyncFinished();

private:
    /// Set request headers with given values.
    void computeHeaders(QNetworkRequest& request,  const Request::HeadersType& headers);

    // Performs requests.
    QNetworkAccessManager* m_networkManager;

};

} // namespace http
} // namespace fwNetworkIO
