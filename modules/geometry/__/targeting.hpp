/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include <data/landmarks.hpp>
#include <data/matrix4.hpp>
#include <data/point_list.hpp>

#include <service/base.hpp>

#include <glm/vec3.hpp>

namespace sight::module::geometry
{

/**
 * @brief  Returns a pointList containing the position of a landmark on a CrossHair
 *
 * @section Slots Slots
 * - \b updateSelectedPoint( std::string, std::size_t ): update selected landmark point position on the crosshair
 * - \b updatePoint( std::string ): update last landmark point position on the crosshair
 * - \b removePoint(): remove the landmark's position on the crosshair
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::geometry::targeting">
            <in key="landmark" uid="..." />
            <in key="matrix" uid="..." />
            <inout key="pointList" uid="..." />
            <label></label>
            <width></width>
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b landmark [sight::data::landmarks]: landmarks representing the needle tip and target
 * - \b matrix [sight::data::matrix4]: Targeting matrix.
 *
 * @subsection InOut InOut:
 * - \b pointList [sight::data::point_list]: computed pointlist
 *
 * @subsection Configuration Configuration:
 * - \b label (optional): (std::string) name of the landmark group
 * - \b width (optional): (int) width of the background image in pixels. The image should have the same width and height
 */
class targeting final : public service::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(targeting, service::base);

    /// Initializes member variables and slots.
    targeting() noexcept;

    /// Destroys the service.
    ~targeting() noexcept override;

protected:

    /// Does nothing.
    void starting() override;

    /// Does nothing.
    void stopping() override;

    /// Updates the position of the landmark on the crosshair.
    void updating() override;

    /// Configures the service parameters.
    void configuring() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::object::MODIFIED_SIG of s_MATRIX_INPUT to service::slots::UPDATE
     */
    connections_t auto_connections() const override;

private:

    /// SLOT: updates selected landmark point position on the crosshair.
    void update_selected_point(std::string _name, std::size_t _index);

    /// SLOT: updates last landmark point position on the crosshair.
    void update_point(std::string _name);

    /// SLOT: removes the landmark's position on the crosshair.
    void remove_point();

    /// Label containing the landmark group name.
    std::string m_label;

    /// Index of the point to modify in the landmark group.
    std::size_t m_index {0};

    /// Bool showing if a landmark is selected.
    bool m_landmark_selected {false};

    /// Target landmark.
    glm::dvec3 m_target_landmark {0.};

    /// Width of the background image (pixels).
    int m_width {400};

    static constexpr std::string_view MATRIX_INPUT = "matrix";
    data::ptr<data::matrix4, sight::data::access::in> m_matrix {this, MATRIX_INPUT, true};
    data::ptr<data::landmarks, sight::data::access::in> m_landmark {this, "landmark", false};
    data::ptr<data::point_list, sight::data::access::inout> m_point_list {this, "pointList", false};
};

} // namespace sight::module::geometry
