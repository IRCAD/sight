/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "io/igtl/config.hpp"
#include "io/igtl/network.hpp"
// Patched header.
#include "io/igtl/patch/igtlSocket.h"

#include <core/exception.hpp>

#include <data/object.hpp>

#include <igtlClientSocket.h>
#include <igtlMessageHeader.h>

#include <string>

namespace sight::io::igtl
{

/**
 *
 * @brief A Network client using igtl::ClientSocket can send fwData primitive
 */
class IO_IGTL_CLASS_API client : public io::igtl::network
{
public:

    using sptr = std::shared_ptr<client>;

    /**
     * @brief default constructor
     */
    IO_IGTL_API client();

    /**
     * @brief Constructor take a igtl::ClientSocket used only by server class.
     *        You should not use this constructor, use default constructor instead
     */
    IO_IGTL_API client(::igtl::ClientSocket::Pointer _socket);

    /**
     * @brief Destructor if a connection is opened the destructor close it
     */
    IO_IGTL_API ~client() override;

    /**
     * @brief method to connect client to a server by using hostname and port
     *
     * @param[in] addr ip or hostname
     * @param[in] port port of server
     */
    IO_IGTL_API void connect(const std::string& _addr, std::uint16_t _port);

    /**
     * @brief method to disconnect client
     */
    IO_IGTL_API void disconnect();

    /**
     * @brief method to check if the client is connected
     *
     * @return a boolean to say if the client is connected or not
     */
    [[nodiscard]] IO_IGTL_API bool is_connected() const;

private:

    /**
     * @brief utility method to avoid code duplication
     * @param[in] msg exception message
     * @param[in] result throw the exception if result=true
     */
    void throw_exception_if_failed(const std::string& _msg, bool _result);
};

} // namespace sight::io::igtl
