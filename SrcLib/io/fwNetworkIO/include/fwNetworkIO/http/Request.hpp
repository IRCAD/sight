/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#pragma once

#include "fwNetworkIO/config.hpp"

#include <fwCore/base.hpp>

#include <map>

namespace fwNetworkIO
{

namespace http
{

/**
 * @brief HTTP request.
 */
class FWNETWORKIO_CLASS_API Request : public ::fwCore::BaseObject
{

public:
    fwCoreAllowSharedFromThis();
    fwCoreClassMacro(Request, ::fwCore::BaseObject, new Request);

    /// Maps header name to its value.
    typedef std::map<std::string, std::string> HeadersType;

    /**
     * @name Constructor/Destructor
     * @{ */
    FWNETWORKIO_API Request();

    /**
     * @brief Construct a new Request with given url.
     */
    FWNETWORKIO_API Request(const std::string& url);

    virtual ~Request()
    {
    }
    /**  @} */

    /**
     * @brief Creates a new Request with given url.
     *
     * @param url URL belonging to the request.
     *
     * @return  the newly created Request object
     */
    FWNETWORKIO_API static Request::sptr New(const std::string& url);

    /**
     * @brief add element in the request header
     */
    FWNETWORKIO_API void addHeader(const std::string& key, const std::string& value);

    /**
     *@brief headers setter.
     */
    FWNETWORKIO_API void setHeaders(const HeadersType& headers);

    /**
     *@brief headers getter.
     */
    FWNETWORKIO_API const HeadersType& getHeaders() const;

    /**
     * @brief return current url
     */
    FWNETWORKIO_API const std::string& getUrl() const;

    /**
     * @brief set current url
     */
    FWNETWORKIO_API void setUrl(const std::string& url);

private:

    HeadersType m_headers;

    std::string m_url;
};

}  // namespace http

}

// namespace fwNetworkIO
