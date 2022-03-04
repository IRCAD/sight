/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "data/config.hpp"
#include "data/timeline/Object.hpp"

namespace sight::data
{

namespace timeline
{

/**
 * @brief   This class defines a TimeLine object buffer.
 */
class DATA_CLASS_API Buffer : public Object
{
public:

    typedef uint8_t* BufferDataType;
    typedef std::function<void (void*)> DeleterType;

    /// Constructor
    DATA_API Buffer(
        core::HiResClock::HiResClockType timestamp = 0,
        BufferDataType buffer                      = 0,
        std::size_t size                           = 0,
        DeleterType d                              = 0
    );

    /// Destructor
    DATA_API virtual ~Buffer();

    /// Makes a copy of this buffer
    DATA_API virtual void deepCopy(const Object& other);

    /// Returns size
    std::size_t getSize() const
    {
        return m_size;
    }

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Buffer& other) const noexcept;
    DATA_API bool operator!=(const Buffer& other) const noexcept;
    /// @}

protected:

    ///Buffer size
    std::size_t m_size;

    ///Buffer
    BufferDataType m_buffer;

    ///Deleter
    DeleterType m_deleter;
};

} // namespace timeline

} // namespace sight::data
