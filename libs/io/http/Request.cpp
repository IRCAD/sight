/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "Request.hpp"

#include <utility>

namespace sight::io::http
{

Request::Request()
= default;

Request::Request(std::string _url) :
    m_url(std::move(_url))
{
}

//------------------------------------------------------------------------------

Request::sptr Request::New(const std::string& _url)
{
    return std::make_shared<Request>(_url);
}

//------------------------------------------------------------------------------

void Request::addHeader(const std::string& _key, const std::string& _value)
{
    m_headers[_key] = _value;
}

//------------------------------------------------------------------------------

void Request::setHeaders(const headers_t& _headers)
{
    m_headers = _headers;
}

//------------------------------------------------------------------------------

const Request::headers_t& Request::getHeaders() const
{
    return m_headers;
}

//------------------------------------------------------------------------------

const std::string& Request::getUrl() const
{
    return m_url;
}

//------------------------------------------------------------------------------

void Request::setUrl(const std::string& _url)
{
    m_url = _url;
}

} // namespace sight::io::http
