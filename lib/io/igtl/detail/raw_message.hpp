/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <sight/io/igtl/config.hpp>

#include <igtlMessageBase.h>

#include <string>
#include <vector>

namespace sight::io::igtl::detail
{

/**
 *
 * @brief a OpenIGTLink message which you can store raw data
 */
class SIGHT_IO_IGTL_CLASS_API raw_message : public ::igtl::MessageBase
{
public:

    using self_t        = raw_message;
    using superclass    = ::igtl::MessageBase;
    using Pointer       = ::igtl::SmartPointer<self_t>; // NOLINT(readability-identifier-naming)
    using const_pointer = ::igtl::SmartPointer<const self_t>;
    using raw_data_t    = std::vector<char>;

    igtlTypeMacro(RawMessage, ::igtl::MessageBase);

    /// Constructor
    SIGHT_IO_IGTL_API raw_message(std::string const& _body_type);

    /// Destructor
    SIGHT_IO_IGTL_API ~raw_message() override;

    /**
     * @brief append a string to raw data
     */
    SIGHT_IO_IGTL_API void append(const raw_data_t& _content);

    /**
     * @brief append bytes to raw data
     */
    SIGHT_IO_IGTL_API void append(const char* _data, std::size_t _size);

    /**
     * @brief get immutable message
     *
     * @return a immutable byte vector
     */
    SIGHT_IO_IGTL_API raw_data_t const& get_message() const;

    /**
     * @brief get mutable message
     *
     * @return a mutable byte vector
     */
    SIGHT_IO_IGTL_API raw_data_t& get_message();

    /**
     * @brief create a new igtl smart pointer to a raw message
     *
     * @return igtl smart pointer to a raw message
     */
    SIGHT_IO_IGTL_API static raw_message::Pointer New(std::string const& _body_type);

protected:

#ifdef OpenIGTLink_HEADER_VERSION
    /// OpenIGTLink 3 separates message content from the extended header and metadata.
    int CalculateContentBufferSize() override;
    int PackContent() override;
    int UnpackContent() override;
#else
    /// Serialization hooks used by older OpenIGTLink versions.
    int GetBodyPackSize() override;
    int PackBody() override;
    int UnpackBody() override;
#endif

private:

    /// raw data
    raw_data_t m_msg;
};

} //namespace sight::io::igtl::detail
