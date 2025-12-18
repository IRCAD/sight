/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
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

#include <data/image.hpp>
#include <data/matrix4.hpp>
#include <data/string.hpp>

#include <service/filter.hpp>

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
   <service uid="..." type="sight::module::filter::image::resampler">
       <in key="imageIn" uid="..." auto_connect="true" />
       <in key="transform" uid="..." />
       <in key="target" uid="..." />
       <inout key="imageOut" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b imageIn [sight::data::image]: image to resample.
 * - \b transform [sight::data::matrix4]: Transform to apply.
 * - \b target [sight::data::image] (optional): target image defining the size, spacing and origin of the output.
 * @subsection In-Out In-Out
 * - \b imageOut [sight::data::image]: New resampled image.
 */
class resampler final : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(resampler, service::filter);

    /// Constructor, does nothing.
    resampler();

    /// Destructor, does nothing.
    ~resampler() final = default;

protected:

    /// Does nothing.
    void configuring() final;

    /// Does nothing.
    void starting() final;

    /// Does nothing.
    void stopping() final;

    /// Apply the transform and resample.
    void updating() final;

    /**
     * @brief Auto connections
     *
     * - Update service when the image or its buffer is modified.
     * - Update service when the transform matrix is modified.
     * - Update service when the target image is modified.
     */
    connections_t auto_connections() const final;

private:

    static constexpr std::string_view IMAGE_IN     = "imageIn";
    static constexpr std::string_view IMAGE_INOUT  = "imageOut";
    static constexpr std::string_view TARGET_IN    = "target";
    static constexpr std::string_view TRANSFORM_IN = "transform";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_image_in {this, IMAGE_IN};
    sight::data::ptr<sight::data::image, sight::data::access::inout> m_image_out {this, IMAGE_INOUT};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_target_in {this, TARGET_IN, true};
    sight::data::ptr<sight::data::matrix4, sight::data::access::in> m_transform_in {this, TRANSFORM_IN};

    sight::data::property<sight::data::string> m_interpolation {this, "interpolation", std::string("LINEAR")};
};

} // namespace sight::module::filter::image
