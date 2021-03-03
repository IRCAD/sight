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

#include <data/Matrix4.hpp>

#include <service/IService.hpp>

namespace sight::module::geometry::base
{

/**
 * @brief Apply a matrix to a landmark.
 *
 * @section Signals Signals
 * - \b landmarkUpdated(string): Emitted when the landmark is modified
 *
 * @section Slots Slots
 * - \b updateSelectedPoint(std::string , size_t ): update the point on which we apply the transform when we select an
 * other landmark
 * - \b updatePoint(std::string ): update the point on which we apply the transform when a point is added
 * - \b removePoint( ): update the point on which we apply the transform when a point is removed
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::geometry::base::STransformLandmark">
            <in key="matrix" uid="..." />
            <inout key="landmark" uid="..." />
            <label></label>
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b matrix [sight::data::Matrix4]: transform applied to the landmark
 *
 * @subsection InOut InOut:
 * - \b landmark [sight::data::Landmarks]: landmark to modify
 *
 * @subsection Configuration Configuration:
 * - \b label (optional): (std::string) name of the landmark group
 */
class MODULE_GEOMETRY_BASE_CLASS_API STransformLandmark final : public service::IService
{

public:

    /// Generates default methods like New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE( STransformLandmark, service::IService)

    /// Initializes signals.
    MODULE_GEOMETRY_BASE_API STransformLandmark() noexcept;

    /// Destroys the service.
    MODULE_GEOMETRY_BASE_API ~STransformLandmark() noexcept override;

protected:

    /// Gets the input matrix.
    MODULE_GEOMETRY_BASE_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_BASE_API void stopping() override;

    /// Applies the transform to the landmark.
    MODULE_GEOMETRY_BASE_API void updating() override;

    /// This method is used to configure the service parameters.
    MODULE_GEOMETRY_BASE_API void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     */
    MODULE_GEOMETRY_BASE_API KeyConnectionsMap getAutoConnections() const override;

private:

    typedef core::com::Signal< void (std::string) > LandmarkUpdatedSignalType;

    /// SLOT: updates selected point.
    void updateSelectedPoint( std::string name, size_t index );

    /// SLOT: updates added point.
    void updatePoint( std::string name );

    /// SLOT: updates removed point.
    void removePoint();

    /// Transform to apply to Landmark.
    data::Matrix4::csptr m_transform;

    /// Label containing the landmark group name.
    std::string m_label{""};

    /// Index of the point to modify in the landmark group.
    size_t m_index{0};

    /// Bool showing if a landmark is selected.
    bool m_landmarkSelected{false};

};
} // namespace sight::module::geometry::base
