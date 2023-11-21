/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "marker_to_point.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/matrix4.hpp>
#include <data/point.hpp>

#include <service/base.hpp>

namespace sight::module::geometry::vision
{

const core::com::slots::key_t marker_to_point::ADD_POINT_SLOT = "add_point";
const core::com::slots::key_t marker_to_point::CLEAR_SLOT     = "clear";

// ----------------------------------------------------------------------------

marker_to_point::marker_to_point() noexcept
{
    new_slot(ADD_POINT_SLOT, &marker_to_point::add_point, this);
    new_slot(CLEAR_SLOT, &marker_to_point::clear, this);
}

// ----------------------------------------------------------------------------

marker_to_point::~marker_to_point() noexcept =
    default;

// ----------------------------------------------------------------------------

void marker_to_point::configuring()
{
}

// ----------------------------------------------------------------------------

void marker_to_point::starting()
{
}

// ----------------------------------------------------------------------------

void marker_to_point::updating()
{
}

// ----------------------------------------------------------------------------

void marker_to_point::stopping()
{
}

// ----------------------------------------------------------------------------

void marker_to_point::add_point()
{
    const auto matrix_tl = m_matrix_tl.lock();

    data::matrix4::sptr matrix_3d = std::make_shared<data::matrix4>();

    core::clock::type current_timestamp = core::clock::get_time_in_milli_sec();
    CSPTR(data::matrix_tl::buffer_t) buffer = matrix_tl->get_closest_buffer(current_timestamp);
    SIGHT_ASSERT("Buffer not found with timestamp " << current_timestamp, buffer);

    const std::array<float, 16> values = buffer->get_element(0);

    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        for(unsigned int j = 0 ; j < 4 ; ++j)
        {
            (*matrix_3d)(i, j) = values[i * std::size_t(4) + j];
        }
    }

    SIGHT_DEBUG(
        "Marker Center Position : " << (*matrix_3d)(0, 3) << " , "
        << (*matrix_3d)(1, 3) << " , "
        << (*matrix_3d)(2, 3)
    );

    //Save the position and drop the orientation
    data::point::sptr p = std::make_shared<data::point>(
        (*matrix_3d)(0, 3),
        (*matrix_3d)(1, 3),
        (*matrix_3d)(2, 3)
    );

    const auto pl = m_point_list.lock();
    pl->push_back(p);
    auto sig = pl->signal<data::point_list::point_added_signal_t>(data::point_list::POINT_ADDED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit(p);
    }
}

// ----------------------------------------------------------------------------

void marker_to_point::clear()
{
    const auto pl = m_point_list.lock();

    if(pl && !pl->get_points().empty())
    {
        pl->clear();

        auto sig = pl->signal<data::point_list::modified_signal_t>(data::point_list::MODIFIED_SIG);
        {
            core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
            sig->async_emit();
        }
    }
}

} //namespace sight::module::geometry::vision
