/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/filter/image/flip.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <filter/image/flipper.hpp>

namespace sight::module::filter::image
{

const core::com::slots::key_t flip::FLIP_AXIS_X_SLOT = "flipAxisX";
const core::com::slots::key_t flip::FLIP_AXIS_Y_SLOT = "flip_axis_y";
const core::com::slots::key_t flip::FLIP_AXIS_Z_SLOT = "flipAxisZ";

//------------------------------------------------------------------------------

flip::flip()
{
    // Initialize the slots
    new_slot(FLIP_AXIS_X_SLOT, &flip::flip_axis_x, this);
    new_slot(FLIP_AXIS_Y_SLOT, &flip::flip_axis_y, this);
    new_slot(FLIP_AXIS_Z_SLOT, &flip::flip_axis_z, this);
}

//------------------------------------------------------------------------------

flip::~flip()
= default;

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
    const auto in_img = m_source.lock();

    SIGHT_ASSERT("No 'imageIn' found !", in_img);
    if(in_img)
    {
        data::image::sptr out_img = std::make_shared<data::image>();

        sight::filter::image::flipper::flip(in_img.get_shared(), out_img, m_flip_axes);

        m_target = out_img;

        m_sig_computed->async_emit();
    }
    else
    {
        SIGHT_ERROR("An update was triggered with an empty image as input. No output image was created.");
    }
}

//------------------------------------------------------------------------------

void flip::stopping()
{
    m_target.reset();
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
        {IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE}
    };
}

} // namespace sight::module::filter::image
