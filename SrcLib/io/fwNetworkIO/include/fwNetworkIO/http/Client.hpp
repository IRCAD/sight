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

namespace fwNetworkIO
{

namespace http
{

/**
 * @brief Base class for HTTP clients providing basic network operations (GET, PUT, POST).
 */
class FWNETWORKIO_CLASS_API Client
{
public:
    virtual ~Client()
    {
    }

    /**
     * @brief Get http request
     */
    FWNETWORKIO_API virtual std::string get(Request::sptr request) = 0;

    /**
     * @brief Get http request set the buffer with the content
     */
    FWNETWORKIO_API virtual bool get(Request::sptr request, char* buffer, size_t size) = 0;

    /**
     * @brief Post http request
     */
    FWNETWORKIO_API virtual std::string post(Request::sptr request, const std::string& content) = 0;

    /**
     * @brief Put http request
     */
    FWNETWORKIO_API virtual std::string put(Request::sptr url, const std::string& content) = 0;

    /**
     * @brief Put http request asynchronously
     */
    FWNETWORKIO_API virtual void putAsync(Request::sptr url, const std::string& content) = 0;

    /**
     * @brief Put http request
     */
    FWNETWORKIO_API virtual std::string put(Request::sptr request, char* buffer, size_t size) = 0;

    /**
     * @brief Put http request asynchronously
     */
    FWNETWORKIO_API virtual void putAsync(Request::sptr request, char* buffer, size_t size) = 0;

    /**
     * @brief Head http request
     */
    FWNETWORKIO_API virtual Request::HeadersType head(Request::sptr request) = 0;
};

}  // namespace http

}  // namespace fwNetworkIO
