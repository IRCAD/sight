/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/http/config.hpp"

#include <core/base.hpp>

#include <map>

namespace sight::io::http
{

/**
 * @brief HTTP request.
 */
class IO_HTTP_CLASS_API request : public core::base_object
{
public:

    SIGHT_ALLOW_SHARED_FROM_THIS();
    SIGHT_DECLARE_CLASS(request, core::base_object);

    /// Maps header name to its value.
    using headers_t = std::map<std::string, std::string>;

    /**
     * @name Constructor/Destructor
     * @{ */
    IO_HTTP_API request();

    /**
     * @brief Construct a new Request with given url.
     */
    IO_HTTP_API request(std::string _url);

    ~request() override
    = default;

    /**  @} */

    /**
     * @brief Creates a new Request with given url.
     *
     * @param url URL belonging to the request.
     *
     * @return  the newly created Request object
     */
    IO_HTTP_API static request::sptr New(const std::string& _url);

    /**
     * @brief add element in the request header
     */
    IO_HTTP_API void add_header(const std::string& _key, const std::string& _value);

    /**
     *@brief headers setter.
     */
    IO_HTTP_API void set_headers(const headers_t& _headers);

    /**
     *@brief headers getter.
     */
    IO_HTTP_API const headers_t& get_headers() const;

    /**
     * @brief return current url
     */
    IO_HTTP_API const std::string& get_url() const;

    /**
     * @brief set current url
     */
    IO_HTTP_API void set_url(const std::string& _url);

private:

    headers_t m_headers;

    std::string m_url;
};

} // namespace sight::io::http
