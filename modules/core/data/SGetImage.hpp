/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "modules/data/config.hpp"

#include <core/base.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>

#include <service/IController.hpp>

namespace sight::module::data
{

/**
 * @brief This service extracts an image from an image series.
 *
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::data::SGetImage" >
           <in key="imageSeries" uid="..." />
           <out key ="image" uid="..."/>
        </service>
       @endcode
 * @subsection In In
 * - \b imageSeries [sight::data::ImageSeries]: image series from which the image is extracted.
 *
 * @subsection Output Output
 * - \b image [sight::data::Image]: output image extracted from the input image series.
 */
class MODULE_DATA_CLASS_API SGetImage : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SGetImage, service::IController);

    /// Constructor
    MODULE_DATA_API SGetImage() = default;

    /// Destructor
    MODULE_DATA_API ~SGetImage() = default;

protected:

    /// Does nothing
    MODULE_DATA_API void starting() override;

    /// Does nothing
    MODULE_DATA_API void configuring() override;

    /// Does nothing
    MODULE_DATA_API void stopping() override;

    /// Extract an image
    MODULE_DATA_API void updating() override;

private:

    static constexpr std::string_view s_IMAGESERIES_IN = "imageSeries";
    static constexpr std::string_view s_IMAGE_OUT      = "image";

    sight::data::ptr<sight::data::ImageSeries, sight::data::Access::in> m_imageSeries {this, s_IMAGESERIES_IN};
    sight::data::ptr<sight::data::Image, sight::data::Access::out> m_image {this, s_IMAGE_OUT};
};

} // namespace sight::module::data
