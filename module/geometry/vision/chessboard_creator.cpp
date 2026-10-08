/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include "chessboard_creator.hpp"

namespace sight::module::geometry::vision
{

//-----------------------------------------------------------------------------

void chessboard_creator::configuring()
{
}

//-----------------------------------------------------------------------------

void chessboard_creator::starting()
{
    this->updating();
}

//-----------------------------------------------------------------------------

void chessboard_creator::updating()
{
    auto chessboard_data = m_chessboard_model.lock();
    SIGHT_ASSERT("Missing 'chessboard_model'.", chessboard_data);

    // Ignore if values are negative
    if(*m_width < 0 || *m_height < 0 || *m_square_size < 0)
    {
        chessboard_data->clear();
        SIGHT_ERROR("Chessboard properties are negative.");
        return;
    }

    const auto width  = static_cast<std::uint64_t>(*m_width);
    const auto height = static_cast<std::uint64_t>(*m_height);
    const double square_size(*m_square_size);

    chessboard_data->clear();
    for(std::uint64_t i = 0 ; i < height - 1 ; ++i)
    {
        const double x = static_cast<double>(i) * square_size;

        for(std::uint64_t j = 0 ; j < width - 1 ; ++j)
        {
            const double y = static_cast<double>(j) * square_size;
            chessboard_data->push_back(std::make_shared<data::point>(x, y, 0.));
        }
    }

    chessboard_data->async_emit(data::signals::MODIFIED);
}

//-----------------------------------------------------------------------------

void chessboard_creator::stopping()
{
}

//-----------------------------------------------------------------------------

service::connections_t chessboard_creator::auto_connections() const
{
    return {
        {m_width, data::signals::MODIFIED, slots::UPDATE},
        {m_height, data::signals::MODIFIED, slots::UPDATE},
        {m_square_size, data::signals::MODIFIED, slots::UPDATE}
    };
}

//-----------------------------------------------------------------------------

} //namespace sight::module::geometry::vision
