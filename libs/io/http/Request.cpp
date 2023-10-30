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

request::request()
= default;

request::request(std::string _url) :
    m_url(std::move(_url))
{
}

//------------------------------------------------------------------------------

request::sptr request::New(const std::string& _url)
{
    return std::make_shared<request>(_url);
}

//------------------------------------------------------------------------------

void request::add_header(const std::string& _key, const std::string& _value)
{
    m_headers[_key] = _value;
}

//------------------------------------------------------------------------------

void request::set_headers(const headers_t& _headers)
{
    m_headers = _headers;
}

//------------------------------------------------------------------------------

const request::headers_t& request::get_headers() const
{
    return m_headers;
}

//------------------------------------------------------------------------------

const std::string& request::get_url() const
{
    return m_url;
}

//------------------------------------------------------------------------------

void request::set_url(const std::string& _url)
{
    m_url = _url;
}

} // namespace sight::io::http
