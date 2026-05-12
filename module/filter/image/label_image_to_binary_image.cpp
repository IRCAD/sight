/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "module/filter/image/label_image_to_binary_image.hpp"

#include <core/com/signal.hxx>

#include <filter/image/labeling.hpp>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

label_image_to_binary_image::label_image_to_binary_image() :
    filter(has_signals::signals())
{
}

//------------------------------------------------------------------------------

void label_image_to_binary_image::configuring()
{
    const config_t config = this->get_config();

    m_label_set_field_name = config.get<std::string>("config.<xmlattr>.labelsField", "");
}

//------------------------------------------------------------------------------

void label_image_to_binary_image::starting()
{
}

//------------------------------------------------------------------------------

void label_image_to_binary_image::updating()
{
    const auto label_image = m_label_image.lock();
    SIGHT_ASSERT("No " << LABEL_IMAGE_INPUT << " input.", label_image);

    const auto mask_image = m_binary_mask.lock();
    SIGHT_ASSERT("No " << BINARY_MASK_INOUT << " inout.", mask_image);

    SIGHT_ASSERT(
        "The label image must be a greyscale image with uint8 values.",
        label_image->type() == core::type::UINT8 && label_image->num_components() == 1
    );

    sight::filter::image::convert_label_image_to_binary_mask(*label_image, *mask_image, m_label_set_field_name);

    mask_image->async_emit(data::image::MODIFIED_SIG);
    this->async_emit(signals::SUCCEEDED);
}

//------------------------------------------------------------------------------

void label_image_to_binary_image::stopping()
{
}

//------------------------------------------------------------------------------

service::connections_t label_image_to_binary_image::auto_connections() const
{
    return {
        {LABEL_IMAGE_INPUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {LABEL_IMAGE_INPUT, data::image::MODIFIED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image.
