/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "matrix_to_point.hpp"

#include <core/com/signal.hxx>

namespace sight::module::geometry
{

//-----------------------------------------------------------------------------

matrix_to_point::matrix_to_point() :
    filter(m_signals)
{
}

//-----------------------------------------------------------------------------

void matrix_to_point::configuring()
{
}

//-----------------------------------------------------------------------------

void matrix_to_point::starting()
{
}

//-----------------------------------------------------------------------------

void matrix_to_point::stopping()
{
}

//-----------------------------------------------------------------------------

void matrix_to_point::updating()
{
    const auto mat = m_matrix.lock();
    auto point     = m_point.lock();

    (*point)[0] = (*mat)[3];
    (*point)[1] = (*mat)[7];
    (*point)[2] = (*mat)[11];

    if(not (*m_label).empty())
    {
        point->set_label(*m_label);
    }

    point->async_emit(data::point::MODIFIED_SIG);
    this->async_emit(signals::COMPUTED);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::geometry
