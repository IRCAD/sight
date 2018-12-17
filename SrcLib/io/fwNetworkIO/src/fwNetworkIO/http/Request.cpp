/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
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

#include "fwNetworkIO/http/Request.hpp"

namespace fwNetworkIO
{

namespace http
{

Request::Request()
{
}

Request::Request(const std::string& url) :
    m_url(url)
{
}

//------------------------------------------------------------------------------

Request::sptr Request::New(const std::string& url)
{
    return std::make_shared<Request>(url);
}

//------------------------------------------------------------------------------

void Request::addHeader(const std::string& key, const std::string& value)
{
    m_headers[key] = value;
}

//------------------------------------------------------------------------------

void Request::setHeaders(const HeadersType& headers)
{
    m_headers = headers;
}

//------------------------------------------------------------------------------

const Request::HeadersType& Request::getHeaders() const
{
    return m_headers;
}

//------------------------------------------------------------------------------

const std::string& Request::getUrl() const
{
    return m_url;
}

//------------------------------------------------------------------------------

void Request::setUrl(const std::string& url)
{
    m_url = url;
}

} // namespace http
} // namespace fwNetworkIO
