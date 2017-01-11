/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWNETWORK_HTTP_CLIENT_HPP__
#define __FWNETWORK_HTTP_CLIENT_HPP__

#include "fwNetwork/config.hpp"
#include "fwNetwork/http/Request.hpp"

#include "fwNetwork/exceptions/ConnectionRefused.hpp"
#include "fwNetwork/exceptions/HostNotFound.hpp"
#include "fwNetwork/exceptions/ContentNotFound.hpp"

namespace fwNetwork
{

namespace http
{


/**
 * @brief Base class for HTTP clients providing basic network operations (GET, PUT, POST).
 */
class FWNETWORK_CLASS_API Client
{
public:
    virtual ~Client()
    {
    }

    /**
     * @brief Get http request
     */
    FWNETWORK_API virtual std::string get(Request::sptr request) = 0;

    /**
     * @brief Get http request set the buffer with the content
     */
    FWNETWORK_API virtual bool get(Request::sptr request, char* buffer, size_t size) = 0;

    /**
     * @brief Post http request
     */
    FWNETWORK_API virtual std::string post(Request::sptr request, const std::string& content) = 0;

    /**
     * @brief Put http request
     */
    FWNETWORK_API virtual std::string put(Request::sptr url, const std::string& content) = 0;

    /**
     * @brief Put http request asynchronously
     */
    FWNETWORK_API virtual void putAsync(Request::sptr url, const std::string& content) = 0;

    /**
     * @brief Put http request
     */
    FWNETWORK_API virtual std::string put(Request::sptr request, char* buffer, size_t size) = 0;

    /**
     * @brief Put http request asynchronously
     */
    FWNETWORK_API virtual void putAsync(Request::sptr request, char* buffer, size_t size) = 0;

    /**
     * @brief Head http request
     */
    FWNETWORK_API virtual Request::HeadersType head(Request::sptr request) = 0;
};

}  // namespace http

}  // namespace fwNetwork

#endif //__FWNETWORK_HTTP_CLIENT_HPP__

