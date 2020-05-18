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

#include "igtlProtocol/config.hpp"

#include <igtlMessageBase.h>

#include <string>
#include <vector>

namespace igtlProtocol
{
/**
 *
 * @brief a OpenIGTLink message which you can store raw data
 */
class IGTLPROTOCOL_CLASS_API RawMessage :
    public igtl::MessageBase
{
public:

    typedef RawMessage Self;
    typedef MessageBase Superclass;
    typedef igtl::SmartPointer<Self> Pointer;
    typedef igtl::SmartPointer<const Self> ConstPointer;
    typedef std::vector< char > RawDataType;

    igtlTypeMacro(RawMessage, igtl::MessageBase);

    /// Constructor
    IGTLPROTOCOL_API RawMessage(std::string const& bodyType);

    /// Destructor
    IGTLPROTOCOL_API ~RawMessage();

    /**
     * @brief append a string to raw data
     */
    IGTLPROTOCOL_API void append(const RawDataType& content);

    /**
     * @brief append bytes to raw data
     */
    IGTLPROTOCOL_API void append(const char* data, std::size_t size);

    /**
     * @brief get immutable message
     *
     * @return a immutable byte vector
     */
    IGTLPROTOCOL_API RawDataType const& getMessage() const;

    /**
     * @brief get mutable message
     *
     * @return a mutable byte vector
     */
    IGTLPROTOCOL_API RawDataType& getMessage();

    /**
     * @brief create a new igtl smart pointer to a raw message
     *
     * @return igtl smart pointer to a raw message
     */
    IGTLPROTOCOL_API static RawMessage::Pointer New(std::string const& bodyType);

private:
    /// Override
    int GetBodyPackSize();

    /// Override
    int PackBody();

    /// Override
    int UnpackBody();

private:

    /// raw data
    RawDataType m_msg;
};
}//namespace igtlProtocol
