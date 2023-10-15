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

#include "data/config.hpp"
#include "data/timeline/object.hpp"

namespace sight::data::timeline
{

/**
 * @brief   This class defines a TimeLine object buffer.
 */
class DATA_CLASS_API buffer : public object
{
public:

    typedef uint8_t* BufferDataType;
    typedef std::function<void (void*)> DeleterType;

    /// Constructor
    DATA_API buffer(
        core::hires_clock::type timestamp = 0,
        BufferDataType buffer             = nullptr,
        std::size_t size                  = 0,
        DeleterType d                     = nullptr
    );

    /// Destructor
    DATA_API ~buffer() override;

    /// Makes a copy of this buffer
    DATA_API void deep_copy(const object& other) override;

    /// Returns size
    [[nodiscard]] std::size_t size() const
    {
        return m_size;
    }

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const buffer& other) const noexcept;
    DATA_API bool operator!=(const buffer& other) const noexcept;
    /// @}

protected:

    ///buffer size
    std::size_t m_size;

    ///buffer
    BufferDataType m_buffer;

    ///Deleter
    DeleterType m_deleter;
};

} // namespace sight::data::timeline
