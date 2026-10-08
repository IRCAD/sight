/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "module/filter/image/flip.hpp"

#include <filter/image/flipper.hpp>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

flip::flip() :
    filter(has_signals::signals())
{
    // Initialize the slots
    new_slot(slots::FLIP_AXIS_X, &flip::flip_axis_x, this);
    new_slot(slots::FLIP_AXIS_Y, &flip::flip_axis_y, this);
    new_slot(slots::FLIP_AXIS_Z, &flip::flip_axis_z, this);
}

//------------------------------------------------------------------------------

void flip::configuring()
{
}

//------------------------------------------------------------------------------

void flip::starting()
{
}

//------------------------------------------------------------------------------

void flip::updating()
{
    auto input  = m_source.lock();
    auto output = m_target.lock();

    SIGHT_THROW_IF("Invalid input image", !input);
    SIGHT_THROW_IF("Invalid output image", !output);

    if(input->num_elements() == 0)
    {
        output->shallow_copy(data::factory::make(output->get_classname()));
        output->async_emit(data::signals::MODIFIED);
        this->async_emit(signals::SUCCEEDED);
        return;
    }

    if(output->size() != input->size())
    {
        output->resize(input->size(), input->type(), input->pixel_format());
    }

    sight::filter::image::flipper::flip(input.get_shared(), output.get_shared(), m_flip_axes);
    output->async_emit(data::signals::MODIFIED);
    this->async_emit(signals::SUCCEEDED);
}

//------------------------------------------------------------------------------

void flip::stopping()
{
}

//------------------------------------------------------------------------------

void flip::flip_axis_x()
{
    m_flip_axes[0] = !(m_flip_axes[0]);
    this->updating();
}

//------------------------------------------------------------------------------

void flip::flip_axis_y()
{
    m_flip_axes[1] = !(m_flip_axes[1]);
    this->updating();
}

//------------------------------------------------------------------------------

void flip::flip_axis_z()
{
    m_flip_axes[2] = !(m_flip_axes[2]);
    this->updating();
}

//------------------------------------------------------------------------------

service::connections_t flip::auto_connections() const
{
    return {
        {"input.image", data::signals::MODIFIED, service::slots::UPDATE},
        {"input.image", data::image::signals::BUFFER_MODIFIED, service::slots::UPDATE}
    };
}

} // namespace sight::module::filter::image
