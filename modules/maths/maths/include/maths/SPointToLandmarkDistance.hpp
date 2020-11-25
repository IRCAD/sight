/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "maths/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IService.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

namespace maths
{

/**
 * @brief Computes the intersection between a mesh and a line.
 *
 * @section Signals Signals
 * - \b distanceChanged(float): Emitted when the distance between the point and the current landmark
 *  is modified
 *
 * @section Slots Slots
 * - \b updateSelectedPoint(std::string , size_t ): update the transformation matrix of the point
 * to the landmark when a landmark is selected
 * - \b updatePoint(std::string ): update the transformation matrix of the point
 * to the landmark when a landmark is added
 * - \b removePoint( ): update the transformation matrix when a point is removed
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::maths::SPointToLandmarkDistance">
       <in key="pointMatrix" uid="..." />
       <in key="landmark" uid="..." />
       <inout key="pointToLandmarkMatrix" uid="..." />
       <inout key="distanceText" uid="..." />
       <config unit="mm" precision="6"/>
   </service>
   @endcode
 *
 * @subsection In In
 * - \b pointMatrix [::fwData::TransformationMatrix3D]: point transformation matrix.
 * - \b landmark [::fwData::Landmarks]: landmark.
 *
 * @subsection In-Out In-Out
 * - \b pointToLandmarkMatrix [::fwData::TransformationMatrix3D]: point to landmark transformation matrix
 * - \b distanceText [::fwData::String]: string containing the distance
 *
 * @subsection Configuration Configuration
 * - \b config(optional): contains the service configuration
 *    - \b unit: unit of the distance computed by this service
 *    - \b precision: precision of the displayed distance
 */
class MATHS_CLASS_API SPointToLandmarkDistance final : public ::fwServices::IService
{

public:

    /// Generates default methods like New, dynamicCast, ...
    fwCoreServiceMacro(SPointToLandmarkDistance, ::fwServices::IService)

    /// Initializes signals and slots.
    MATHS_API SPointToLandmarkDistance() noexcept;

    /// Destroys the service.
    MATHS_API ~SPointToLandmarkDistance() noexcept override;

protected:

    /// Does nothing.
    MATHS_API void starting() override;

    /// Does nothing.
    MATHS_API void stopping() override;

    /// Generates and updates the mesh.
    MATHS_API void updating() override;

    /// Configures the service's parameters.
    MATHS_API void configuring() override;

private:

    typedef ::fwCom::Signal< void (float) > DistanceChangedSignalType;

    /// SLOT: updates selected point.
    void updateSelectedPoint ( std::string name, size_t index );

    /// SLOT: updates added point.
    void updatePoint ( std::string name );

    /// SLOT: updates removed point.
    void removePoint ();

    /// Selected landmark.
    ::glm::dvec3 m_currentLandmark;

    /// Bool showing if a landmark is selected.
    bool m_landmarkSelected;

    /// Unit to concatenate with the computed result.
    std::string m_unit;

    /// Precision of the displayed distance.
    int m_precision{6};
};

} // namespace maths
