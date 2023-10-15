/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "image_center.hpp"

#include <data/helper/MedicalImage.hpp>

#include <geometry/data/matrix4.hpp>

#include <service/macros.hpp>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

void image_center::configuring()
{
}

//------------------------------------------------------------------------------

void image_center::starting()
{
}

//------------------------------------------------------------------------------

void image_center::updating()
{
    const auto image = m_image.lock();

    SIGHT_ASSERT("Missing image '" << s_IMAGE_IN << "'", image);

    const bool imageValidity = data::helper::MedicalImage::checkImageValidity(image.get_shared());

    if(!imageValidity)
    {
        SIGHT_WARN("Can not compute center of a invalid image.");
        return;
    }

    auto matrix = m_transform.lock();

    SIGHT_ASSERT("Missing matrix '" << s_TRANSFORM_INOUT << "'", matrix);

    geometry::data::identity(*matrix);

    //compute the center
    const data::image::Size size       = image->size();
    const data::image::Spacing spacing = image->getSpacing();
    const data::image::Origin origin   = image->getOrigin();

    SIGHT_ASSERT("image should be in 3 Dimensions", size.size() == 3);

    std::vector<double> center(3, 0.);

    center[0] = (static_cast<double>(size[0]) * spacing[0]) / 2.;
    center[1] = (static_cast<double>(size[1]) * spacing[1]) / 2.;
    center[2] = (static_cast<double>(size[2]) * spacing[2]) / 2.;

    //compute origin -center

    center[0] += origin[0];
    center[1] += origin[1];
    center[2] += origin[2];

    (*matrix)(0, 3) = center[0];
    (*matrix)(1, 3) = center[1];
    (*matrix)(2, 3) = center[2];

    // output the translation matrix

    auto sig = matrix->signal<data::matrix4::ModifiedSignalType>
                   (data::matrix4::MODIFIED_SIG);

    sig->async_emit();

    m_sigComputed->async_emit();
}

//------------------------------------------------------------------------------

void image_center::stopping()
{
}

//------------------------------------------------------------------------------

service::connections_t image_center::auto_connections() const
{
    return {{s_IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE}};
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image
