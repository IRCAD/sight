/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "SResampler.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <filter/image/Resampler.hpp>

#include <service/macros.hpp>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

SResampler::SResampler()
{
}

//------------------------------------------------------------------------------

SResampler::~SResampler()
{
}

//------------------------------------------------------------------------------

void SResampler::configuring()
{
}

//------------------------------------------------------------------------------

void SResampler::starting()
{
}

//------------------------------------------------------------------------------

void SResampler::updating()
{
    const auto inImg     = m_imageIn.lock();
    auto outImg          = m_imageOut.lock();
    const auto target    = m_targetIn.lock();
    const auto transform = m_transformIn.lock();

    SIGHT_ASSERT("No '" << s_IMAGE_IN << "' found !", inImg);
    SIGHT_ASSERT("No '" << s_IMAGE_IN << "' found !", outImg);
    SIGHT_ASSERT("No '" << s_TRANSFORM_IN << "' found !", transform);

    sight::filter::image::Resampler::resample(
        inImg.get_shared(),
        outImg.get_shared(),
        transform.get_shared(),
        std::make_tuple(target->getSize(), target->getOrigin(), target->getSpacing())
    );

    m_sigComputed->asyncEmit();

    auto imgBufModifiedSig = outImg->signal<data::Image::BufferModifiedSignalType>
                                 (data::Image::s_BUFFER_MODIFIED_SIG);

    imgBufModifiedSig->asyncEmit();

    auto imgModifiedSig = outImg->signal<data::Image::ModifiedSignalType>
                              (data::Image::s_MODIFIED_SIG);

    imgModifiedSig->asyncEmit();
}

//------------------------------------------------------------------------------

void SResampler::stopping()
{
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SResampler::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_IN, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_IN, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TRANSFORM_IN, data::Matrix4::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TARGET_IN, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

} // namespace sight::module::filter::image
