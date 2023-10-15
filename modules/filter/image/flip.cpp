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
const core::com::slots::key_t flip::FLIP_AXIS_Y_SLOT = "flipAxisY";
const core::com::slots::key_t flip::FLIP_AXIS_Z_SLOT = "flipAxisZ";

//------------------------------------------------------------------------------

flip::flip()
{
    // Initialize the slots
    new_slot(FLIP_AXIS_X_SLOT, &flip::flipAxisX, this);
    new_slot(FLIP_AXIS_Y_SLOT, &flip::flipAxisY, this);
    new_slot(FLIP_AXIS_Z_SLOT, &flip::flipAxisZ, this);
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
    const auto inImg = m_source.lock();

    SIGHT_ASSERT("No 'imageIn' found !", inImg);
    if(inImg)
    {
        data::image::sptr outImg = std::make_shared<data::image>();

        sight::filter::image::flipper::flip(inImg.get_shared(), outImg, m_flipAxes);

        m_target = outImg;

        m_sigComputed->async_emit();
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

void flip::flipAxisX()
{
    m_flipAxes[0] = !(m_flipAxes[0]);
    this->updating();
}

//------------------------------------------------------------------------------

void flip::flipAxisY()
{
    m_flipAxes[1] = !(m_flipAxes[1]);
    this->updating();
}

//------------------------------------------------------------------------------

void flip::flipAxisZ()
{
    m_flipAxes[2] = !(m_flipAxes[2]);
    this->updating();
}

//------------------------------------------------------------------------------

service::connections_t flip::auto_connections() const
{
    return {
        {s_IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE}
    };
}

} // namespace sight::module::filter::image
