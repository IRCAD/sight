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

#include "io/igtl/config.hpp"

#include <igtlMessageBase.h>

#include <string>
#include <vector>

// FIXME
#ifndef WIN32
    #define OVERRIDE_ON_LINUX override
#else
    #define OVERRIDE_ON_LINUX
#endif

namespace sight::io::igtl::detail
{

/**
 *
 * @brief a OpenIGTLink message which you can store raw data
 */
class IO_IGTL_CLASS_API RawMessage : public ::igtl::MessageBase
{
public:

    typedef RawMessage Self;
    typedef ::igtl::MessageBase Superclass;
    typedef ::igtl::SmartPointer<Self> Pointer;
    typedef ::igtl::SmartPointer<const Self> ConstPointer;
    typedef std::vector<char> raw_data_t;

    igtlTypeMacro(RawMessage, ::igtl::MessageBase);

    /// Constructor
    IO_IGTL_API RawMessage(std::string const& _body_type);

    /// Destructor
    IO_IGTL_API ~RawMessage() override;

    /**
     * @brief append a string to raw data
     */
    IO_IGTL_API void append(const raw_data_t& _content);

    /**
     * @brief append bytes to raw data
     */
    IO_IGTL_API void append(const char* _data, std::size_t _size);

    /**
     * @brief get immutable message
     *
     * @return a immutable byte vector
     */
    IO_IGTL_API raw_data_t const& getMessage() const;

    /**
     * @brief get mutable message
     *
     * @return a mutable byte vector
     */
    IO_IGTL_API raw_data_t& getMessage();

    /**
     * @brief create a new igtl smart pointer to a raw message
     *
     * @return igtl smart pointer to a raw message
     */
    IO_IGTL_API static RawMessage::Pointer New(std::string const& _body_type);

private:

    /// Override
    int GetBodyPackSize() OVERRIDE_ON_LINUX; // FIXME

    /// Override
    int PackBody() OVERRIDE_ON_LINUX; // FIXME

    /// Override
    int UnpackBody() OVERRIDE_ON_LINUX; // FIXME

private:

    /// raw data
    raw_data_t m_msg;
};

} //namespace sight::io::igtl::detail
