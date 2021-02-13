/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include "modules/geometry/base/config.hpp"

#include <glm/vec3.hpp>

#include <service/IService.hpp>

namespace sight::modules::geometry::base
{

/**
 * @brief  Return a pointList containing the position of a landmark on a CrossHair
 *
 * @section Slots Slots
 * - \b updateSelectedPoint( std::string, size_t ): update selected landmark point position on the crosshair
 * - \b updatePoint( std::string ): update last landmark point position on the crosshair
 * - \b removePoint(): remove the landmark's position on the crosshair
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::sight::modules::geometry::base::STargeting">
            <in key="landmark" uid="..." />
            <in key="matrix" uid="..." />
            <inout key="pointList" uid="..." />
            <label></label>
            <width></width>
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b landmark [sight::data::Landmarks]: landmarks representing the needle tip and target
 * - \b matrix [sight::data::TransformationMatrix3D]: Targeting matrix.
 *
 * @subsection InOut InOut:
 * - \b pointList [sight::data::PointList]: computed pointlist
 *
 * @subsection Configuration Configuration:
 * - \b label (optional): (std::string) name of the landmark group
 * - \b width (optional): (int) width of the background image in pixels. The image should have the same width and height
 */
class MODULE_GEOMETRY_BASE_CLASS_API STargeting final : public service::IService
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(STargeting, service::IService)

    /// Initializes member variables and slots.
    MODULE_GEOMETRY_BASE_API STargeting() noexcept;

    /// Destroys the service.
    MODULE_GEOMETRY_BASE_API ~STargeting() noexcept override;

protected:

    /// Does nothing.
    MODULE_GEOMETRY_BASE_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_BASE_API void stopping() override;

    /// Updates the position of the landmark on the crosshair.
    MODULE_GEOMETRY_BASE_API void updating() override;

    /// Configures the service parameters.
    MODULE_GEOMETRY_BASE_API void configuring() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Object::s_MODIFIED_SIG of s_MATRIX_INPUT to s_UPDATE_SLOT
     */
    MODULE_GEOMETRY_BASE_API KeyConnectionsMap getAutoConnections() const override;

private:
    /// SLOT: updates selected landmark point position on the crosshair.
    void updateSelectedPoint( std::string name, size_t index );

    /// SLOT: updates last landmark point position on the crosshair.
    void updatePoint( std::string name );

    /// SLOT: removes the landmark's position on the crosshair.
    void removePoint();

    /// Label containing the landmark group name.
    std::string m_label{""};

    /// Index of the point to modify in the landmark group.
    size_t m_index{0};

    /// Bool showing if a landmark is selected.
    bool m_landmarkSelected{false};

    /// Target landmark.
    ::glm::dvec3 m_targetLandmark{0. };

    /// Width of the background image (pixels).
    int m_width{400};

};
} // namespace sight::modules::geometry::base
