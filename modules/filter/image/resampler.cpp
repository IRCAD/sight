/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "resampler.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

#include <filter/image/resampler.hpp>

#include <service/macros.hpp>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

resampler::resampler()
= default;

//------------------------------------------------------------------------------

resampler::~resampler()
= default;

//------------------------------------------------------------------------------

void resampler::configuring()
{
}

//------------------------------------------------------------------------------

void resampler::starting()
{
}

//------------------------------------------------------------------------------

void resampler::updating()
{
    const auto inImg     = m_imageIn.lock();
    auto outImg          = m_imageOut.lock();
    const auto target    = m_targetIn.lock();
    const auto transform = m_transformIn.lock();

    SIGHT_ASSERT("No '" << s_IMAGE_IN << "' found !", inImg);
    SIGHT_ASSERT("No '" << s_IMAGE_IN << "' found !", outImg);
    SIGHT_ASSERT("No '" << s_TRANSFORM_IN << "' found !", transform);

    sight::filter::image::resampler::resample(
        inImg.get_shared(),
        outImg.get_shared(),
        transform.get_shared(),
        std::make_tuple(target->size(), target->getOrigin(), target->getSpacing())
    );

    m_sigComputed->async_emit();

    auto imgBufModifiedSig = outImg->signal<data::image::BufferModifiedSignalType>
                                 (data::image::BUFFER_MODIFIED_SIG);

    imgBufModifiedSig->async_emit();

    auto imgModifiedSig = outImg->signal<data::image::ModifiedSignalType>
                              (data::image::MODIFIED_SIG);

    imgModifiedSig->async_emit();
}

//------------------------------------------------------------------------------

void resampler::stopping()
{
}

//------------------------------------------------------------------------------

service::connections_t resampler::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_TRANSFORM_IN, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_TARGET_IN, data::image::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

} // namespace sight::module::filter::image
