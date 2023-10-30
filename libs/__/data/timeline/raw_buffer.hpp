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
#include "data/timeline/buffer.hpp"

namespace sight::data::timeline
{

/**
 * @brief   This class defines a TimeLine object buffer.
 */
class DATA_CLASS_API raw_buffer : public data::timeline::buffer
{
public:

    /// Constructor
    DATA_API raw_buffer(
        core::hires_clock::type _timestamp = 0,
        buffer_data_t _buffer              = nullptr,
        std::size_t _size                  = 0,
        deleter_t _d                       = nullptr
    );

    /// Destructor
    DATA_API ~raw_buffer() override;

    /// Set buffer values
    DATA_API void set_buffer_values(buffer_data_t _values);

    /// Return buffer
    [[nodiscard]] buffer_data_t buffer() const
    {
        return m_buffer;
    }

    //------------------------------------------------------------------------------

    template<typename TYPE>
    [[nodiscard]] TYPE* buffer() const
    {
        return reinterpret_cast<TYPE*>(m_buffer);
    }
};

} // namespace sight::data::timeline
