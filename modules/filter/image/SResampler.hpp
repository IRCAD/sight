/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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
#include <data/Matrix4.hpp>

#include <service/IFilter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Service applying a transform to an image and outputing the resampled image.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent after resampling.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="opITK::SPropagator">
       <in key="imageIn" uid="..." autoConnect="true" />
       <in key="transform" uid="..." />
       <in key="target" uid="..." />
       <inout key="imageOut" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b imageIn [sight::data::Image]: Image to resample.
 * - \b transform [sight::data::Matrix4]: Transform to apply.
 * - \b target [sight::data::Image] (optional): target image defining the size, spacing and origin of the output.
 * @subsection In-Out In-Out
 * - \b imageOut [sight::data::Image]: New resampled image.
 */
class MODULE_FILTER_IMAGE_CLASS_API SResampler : public service::IFilter
{
public:

    SIGHT_DECLARE_SERVICE(SResampler, service::IFilter);

    /// Constructor, does nothing.
    MODULE_FILTER_IMAGE_API SResampler();

    /// Destructor, does nothing.
    MODULE_FILTER_IMAGE_API ~SResampler() override;

protected:

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void starting() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void stopping() override;

    /// Apply the transform and resample.
    MODULE_FILTER_IMAGE_API void updating() override;

    /**
     * @brief Auto connections
     *
     * - Update service when the image or its buffer is modified.
     * - Update service when the transform matrix is modified.
     * - Update service when the target image is modified.
     */
    MODULE_FILTER_IMAGE_API KeyConnectionsMap getAutoConnections() const override;

private:

    static const constexpr std::string_view s_IMAGE_IN     = "imageIn";
    static const constexpr std::string_view s_IMAGE_INOUT  = "imageOut";
    static const constexpr std::string_view s_TARGET_IN    = "target";
    static const constexpr std::string_view s_TRANSFORM_IN = "transform";

    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_imageIn {this, s_IMAGE_IN};
    sight::data::ptr<sight::data::Image, sight::data::Access::inout> m_imageOut {this, s_IMAGE_INOUT};
    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_targetIn {this, s_TARGET_IN, false, true};
    sight::data::ptr<sight::data::Matrix4, sight::data::Access::in> m_transformIn {this, s_TRANSFORM_IN};
};

} // namespace sight::module::filter::image
