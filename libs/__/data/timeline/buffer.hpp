/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/data/config.hpp>

#include "data/timeline/object.hpp"

namespace sight::data::timeline
{

/**
 * @brief   This class defines a TimeLine object buffer.
 */
class SIGHT_DATA_CLASS_API buffer : public object
{
public:

    using buffer_data_t = uint8_t*;
    using deleter_t     = std::function<void (void*)>;

    /// Constructor
    SIGHT_DATA_API buffer(
        core::clock::type _timestamp = 0,
        buffer_data_t _buffer        = nullptr,
        std::size_t _size            = 0,
        deleter_t _d                 = nullptr
    );

    /// Destructor
    SIGHT_DATA_API ~buffer() override;

    /// Makes a copy of this buffer
    SIGHT_DATA_API void deep_copy(const object& _other) override;

    /// Returns size
    [[nodiscard]] std::size_t size() const
    {
        return m_size;
    }

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const buffer& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const buffer& _other) const noexcept;
    /// @}

protected:

    ///buffer size
    std::size_t m_size;

    ///buffer
    buffer_data_t m_buffer;

    ///Deleter
    deleter_t m_deleter;
};

} // namespace sight::data::timeline
