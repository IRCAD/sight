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
#include "data/timeline/Buffer.hpp"

namespace sight::data::timeline
{

/**
 * @brief   This class defines a TimeLine object buffer.
 */
class DATA_CLASS_API RawBuffer : public data::timeline::Buffer
{
public:

    /// Constructor
    DATA_API RawBuffer(
        core::hires_clock::type timestamp = 0,
        BufferDataType buffer             = nullptr,
        std::size_t size                  = 0,
        DeleterType d                     = nullptr
    );

    /// Destructor
    DATA_API ~RawBuffer() override;

    /// Set buffer values
    DATA_API void setBufferValues(BufferDataType values);

    /// Return buffer
    [[nodiscard]] BufferDataType buffer() const
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
