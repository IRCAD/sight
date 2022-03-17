/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "modules/filter/image/SFlip.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <filter/image/Flipper.hpp>

namespace sight::module::filter::image
{

const core::com::Slots::SlotKeyType SFlip::s_FLIP_AXIS_X_SLOT = "flipAxisX";
const core::com::Slots::SlotKeyType SFlip::s_FLIP_AXIS_Y_SLOT = "flipAxisY";
const core::com::Slots::SlotKeyType SFlip::s_FLIP_AXIS_Z_SLOT = "flipAxisZ";

//------------------------------------------------------------------------------

SFlip::SFlip()
{
    // Initialize the slots
    newSlot(s_FLIP_AXIS_X_SLOT, &SFlip::flipAxisX, this);
    newSlot(s_FLIP_AXIS_Y_SLOT, &SFlip::flipAxisY, this);
    newSlot(s_FLIP_AXIS_Z_SLOT, &SFlip::flipAxisZ, this);
}

//------------------------------------------------------------------------------

SFlip::~SFlip()
{
}

//------------------------------------------------------------------------------

void SFlip::configuring()
{
}

//------------------------------------------------------------------------------

void SFlip::starting()
{
}

//------------------------------------------------------------------------------

void SFlip::updating()
{
    const auto inImg = m_source.lock();

    SIGHT_ASSERT("No 'imageIn' found !", inImg);
    if(inImg)
    {
        data::Image::sptr outImg = data::Image::New();

        sight::filter::image::Flipper::flip(inImg.get_shared(), outImg, m_flipAxes);

        m_target = outImg;

        m_sigComputed->asyncEmit();
    }
    else
    {
        SIGHT_ERROR("An update was triggered with an empty image as input. No output image was created.");
    }
}

//------------------------------------------------------------------------------

void SFlip::stopping()
{
    m_target.reset();
}

//------------------------------------------------------------------------------

void SFlip::flipAxisX()
{
    m_flipAxes[0] = !(m_flipAxes[0]);
    this->updating();
}

//------------------------------------------------------------------------------

void SFlip::flipAxisY()
{
    m_flipAxes[1] = !(m_flipAxes[1]);
    this->updating();
}

//------------------------------------------------------------------------------

void SFlip::flipAxisZ()
{
    m_flipAxes[2] = !(m_flipAxes[2]);
    this->updating();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SFlip::getAutoConnections() const
{
    return {
        {s_IMAGE_IN, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT},
        {s_IMAGE_IN, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT}
    };
}

} // namespace sight::module::filter::image
