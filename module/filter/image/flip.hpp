/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include <service/filter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Service flipping an input image along x or y or z axis.
 *
 * @section Slots Slots
 * - flip_axis_x(): flip along X axis
 * - flip_axis_y(): flip along Y axis
 * - flip_axis_z(): flip along Z axis

 * @section XML XML Configuration
 *
 * @code{.xml}
       <service type="sight::module::filter::image::flip">
           <input image="${...}" />
           <output image="${...}" />
       </service>
   @endcode
 * @subsection Input Input
 * - \b input.image [sight::data::image]: image to flip.
 * @subsection Output Output:
 * - \b output.image [sight::data::image]: New flipped image.
 */
class flip : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(flip, sight::service::filter);

    struct slots
    {
        static inline const slot_key_t FLIP_AXIS_X = "flip_axis_x";
        static inline const slot_key_t FLIP_AXIS_Y = "flip_axis_y";
        static inline const slot_key_t FLIP_AXIS_Z = "flip_axis_z";
    };

    /// Constructor, does nothing.
    flip();

    /// Destructor, does nothing.
    ~flip() override = default;

protected:

    /// Configures the service.
    void configuring() override;

    /// Does nothing.
    void starting() override;

    /// Does nothing.
    void stopping() override;

    /// Apply the flip operator.
    void updating() override;

    /**
     * @brief Signal-slot auto-connection proposals
     *
     * Connect data::signals::MODIFIED to this::service::slots::UPDATE
     * Connect image::signals::BUFFER_MODIFIED to this::service::slots::UPDATE
     */
    connections_t auto_connections() const override;

private:

    /// Slot: flip the first axis
    void flip_axis_x();

    /// Slot: flip the second axis
    void flip_axis_y();

    /// Slot: flip the third axis
    void flip_axis_z();

    // Store whether to flip or not one of the 3 axis
    std::array<bool, 3> m_flip_axes {{false, false, false}};

    static constexpr std::string_view IMAGE_IN  = "input.image";
    static constexpr std::string_view IMAGE_OUT = "output.image";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_source {this, IMAGE_IN};
    sight::data::ptr<sight::data::image, sight::data::access::out> m_target {this, IMAGE_OUT};
};

} // namespace sight::module::filter::image
