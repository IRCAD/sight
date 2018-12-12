/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Color.hpp>
#include <fwData/Point.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vector>

class vtkDistanceRepresentation2D;
class vtkActor;
class vtkLineSource;

namespace visuVTKAdaptor
{

/**
 * @brief Display the distance between the two points of the point list
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::namespace::classname">
       <in key="pointList" uid="..." />
       <config renderer="default" />
   </service>
   @endcode
 * @subsection Input Input
 * - \b pointList [::fwData::PointList]: point list containing two points.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the distance.
 */
class VISUVTKADAPTOR_CLASS_API SDistance : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SDistance)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SDistance() noexcept;

    VISUVTKADAPTOR_API virtual ~SDistance() noexcept;

    static const ::fwServices::IService::KeyType s_POINTLIST_INPUT;

    /// set Distance Axis color AND alpha
    VISUVTKADAPTOR_API void setAxisColor( ::fwData::Color::sptr newColor) noexcept;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect PointList::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    vtkDistanceRepresentation2D* m_distanceRepresentation;

    vtkActor* m_lineActor;
    vtkLineSource* m_lineSource;

    ::fwData::Point::cwptr m_point1;
    ::fwData::Point::cwptr m_point2;

    /// Connection between point 1 modified and this service reveive
    ::fwCom::Connection m_point1Connection;

    /// Connection between point 2 modified and this service reveive
    ::fwCom::Connection m_point2Connection;
};

} //namespace visuVTKAdaptor
