/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <service/filter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Service computing the center of a 3D image, it outputs the translation going from origin to the center.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent after computing center.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::filter::image::image_center">
       <in key="image" uid="..." />
       <inout key="transform" uid="..." />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [sight::data::image]: Input image on which the center will be computed.
 *
 * @subsection In-Out In-Out
 * - \b transform [sight::data::matrix4]: Will be first reset to identity and then will contain
 * translation
 * from world origin(0,0,0) to the image center (in mm).
 *
 */
class image_center : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(image_center, service::filter);

    /// Constructor, does nothing.
    image_center();

    /// Destructor, does nothing.
    ~image_center() override = default;

protected:

    /// Configure registration parameters.
    void configuring() override;

    /// Does nothing.
    void starting() override;

    /// Do the registration.
    void updating() override;

    /// Does nothing.
    void stopping() override;

    /**
     * @brief Auto connections
     *
     * - Update service when one of the two images is modified.
     * - Update service when the transform matrix is modified.
     */
    connections_t auto_connections() const override;

private:

    static constexpr std::string_view IMAGE_IN        = "image";
    static constexpr std::string_view TRANSFORM_INOUT = "transform";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_image {this, IMAGE_IN};
    sight::data::ptr<sight::data::matrix4, sight::data::access::inout> m_transform {this, TRANSFORM_INOUT};
};

} // namespace sight::module::filter::image
