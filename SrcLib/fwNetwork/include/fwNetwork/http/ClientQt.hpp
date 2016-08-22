/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWNETWORK_HTTP_CLIENTQT_HPP__
#define __FWNETWORK_HTTP_CLIENTQT_HPP__

#include <QtNetwork>
#include <QNetworkReply>

#include "fwNetwork/http/Client.hpp"
#include "fwNetwork/config.hpp"

namespace fwNetwork
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
class FWNETWORK_CLASS_API ClientQt : public QObject,
                                     public Client
{
Q_OBJECT

public:
    /**
     * Constructor/Destructor
     * @{
     */
    FWNETWORK_API ClientQt();
    FWNETWORK_API virtual ~ClientQt();
    /**  @} */

    /**
     * @brief Sends data to the server using post
     * @param request the request to post
     * @param content has the data to send
     */
    FWNETWORK_API virtual std::string post(Request::sptr request, const std::string& content);

    /**
     * @brief Retrieves data over network
     * @param request the request
     */
    FWNETWORK_API virtual std::string get(Request::sptr request);

    /**
     * @brief Retrieves data over network
     * @param buffer the buffer where data is written
     * @param size buffer size
     * @return true if the reply has been read successfully
     */
    FWNETWORK_API virtual bool get(Request::sptr request, char* buffer, size_t size);

    /**
     * @brief Puts HTTP request
     * @param url destination of data
     * @param content the data to put
     * @return answer data
     */
    FWNETWORK_API virtual std::string put(Request::sptr url, const std::string& content);

    /**
     * @brief Puts HTTP request asynchronously
     * @param url destination of data
     * @param content the data to put
     */
    FWNETWORK_API virtual void putAsync(Request::sptr url, const std::string& content);

    /**
     * @brief Puts binary file
     * @param request the request
     * @param buffer reference to the buffer
     * @param size buffer size
     */
    FWNETWORK_API virtual std::string put(Request::sptr request, char* buffer, size_t size);

    /**
     * @brief Puts binary file asynchronously
     * @param request the request
     * @param buffer reference to the buffer;
     * @param size buffer size
     */
    FWNETWORK_API virtual void putAsync(Request::sptr request, char* buffer, size_t size);

    /**
     * @brief Performs head request
     * @param request the request
     * @return headers resulting of the request
     */
    FWNETWORK_API virtual Request::HeadersType head(Request::sptr request);


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
} // namespace fwNetwork

#endif /* __FWNETWORK_HTTP_CLIENTQT_HPP__ */

