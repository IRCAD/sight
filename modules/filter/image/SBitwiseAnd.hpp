/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#pragma once

#include "modules/filter/image/config.hpp"

#include <data/Image.hpp>

#include <service/filter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Implements the AND bitwise operator pixel-wise between two images.
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::filter::image::SBitwiseAnd">
           <in key="image" uid="..."/>
           <in key="mask" uid="..."/>
           <out key="outputImage" uid="..." />
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [sight::data::Image]: input image
 * - \b mask [sight::data::Image]: mask
 *
 * @subsection Output Output
 * - \b outputImage [sight::data::Image]: result of operation 'AND' between input image and mask
 */
class MODULE_FILTER_IMAGE_CLASS_API SBitwiseAnd final : public sight::service::filter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SBitwiseAnd, sight::service::filter);

    /// Does nothing.
    MODULE_FILTER_IMAGE_API SBitwiseAnd();

    /// Destroys the service.
    MODULE_FILTER_IMAGE_API ~SBitwiseAnd() override;

protected:

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void starting() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void stopping() override;

    /// Performs bitwise operation between mask and image.
    MODULE_FILTER_IMAGE_API void updating() override;

    static constexpr std::string_view s_OUTPUTIMAGE_OUT = "outputImage";

    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_image {this, "image"};
    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_mask {this, "mask"};
    sight::data::ptr<sight::data::Image, sight::data::Access::out> m_outputImage {this, s_OUTPUTIMAGE_OUT};
};

} // namespace sight::module::filter::image.
