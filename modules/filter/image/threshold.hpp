/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <data/object.hpp>

#include <service/filter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief   Apply a threshold on an image.
 *
 * This Service needs two image: the source and the target image.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::filter::image::threshold">
            <in key="source" uid="..." />
            <out key="target" uid="..." />
            <config>
                <threshold>50</threshold>
            </config>
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b source [sight::data::object]: Source image, can be a data::image_series or a data::image.
 * @subsection Output Output:
 * - \b target [sight::data::object]: Target image (ie the filtered image),
 * if the type of the source is a data::image_series then target will be a data::image_series,
 * otherwise it will be an simple data::image.
 * @subsection Configuration Configuration
 * - \b threshold : Specify the desired threshold used in filter
 * (Pixels with value lower than this value will be set to 0,
 * otherwise it will be set to the maximum value of the pixel type of the image.).
 */
class MODULE_FILTER_IMAGE_CLASS_API threshold : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(threshold, sight::service::filter);

    MODULE_FILTER_IMAGE_API threshold() noexcept;

    MODULE_FILTER_IMAGE_API ~threshold() noexcept override;

protected:

    MODULE_FILTER_IMAGE_API void starting() override;

    MODULE_FILTER_IMAGE_API void stopping() override;

    /// Configure the service.
    MODULE_FILTER_IMAGE_API void configuring() override;

    /// Apply the threshold.
    MODULE_FILTER_IMAGE_API void updating() override;

private:

    /// Threshold value used in filter
    double m_threshold {50.0};

    static constexpr std::string_view s_IMAGE_IN  = "source";
    static constexpr std::string_view s_IMAGE_OUT = "target";

    sight::data::ptr<sight::data::object, sight::data::Access::in> m_source {this, s_IMAGE_IN, true};
    sight::data::ptr<sight::data::object, sight::data::Access::out> m_target {this, s_IMAGE_OUT};
};

} // namespace sight::module::filter::image
