/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuVTKARAdaptor/config.hpp"

#include <fwCore/base.hpp>

#include <fwData/Color.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkActor.h>
#include <vtkSmartPointer.h>

namespace visuVTKARAdaptor
{

/**
 * @brief   Display a 2D point list.
 *
 *  The coordinate system of the point list is [ (0,0);(width,-height) ] whereas the coordinate system of the scene
 *  is [ (-width/2;-height/2,width/2:height/2], so we need to transform the points.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool) : show/hide the pointlist
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKARAdaptor::SPointList" autoConnect="yes">
       <in key="pointlist" uid="..." />
       <config renderer="default" color="#cb1f72" radius="3.0" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b pointlist [::fwData::PointList]: displayed point list.
 *
 * @subsection Configuration Configuration
 * - \b renderer : ID of renderer the adaptor must use.
 * - \b color(optional) (default: white): point color.
 * - \b radius (optional) (default: 3.): point radius.
 */
class VISUVTKARADAPTOR_CLASS_API SPointList : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceMacro(SPointList, ::fwRenderVTK::IAdaptor);

    /// Constructor.
    VISUVTKARADAPTOR_API SPointList() noexcept;

    /// Destructor.
    VISUVTKARADAPTOR_API virtual ~SPointList() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ::fwData::PointList::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect ::fwData::PointList::s_POINT_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect ::fwData::PointList::s_POINT_REMOVED_SIG to this::s_UPDATE_SLOT
     *
     */
    VISUVTKARADAPTOR_API KeyConnectionsMap getAutoConnections() const override;

protected:

    VISUVTKARADAPTOR_API void configuring() override;

    /// Initialize the service and update it.
    VISUVTKARADAPTOR_API void starting() override;

    /// Create/update vtk props.
    VISUVTKARADAPTOR_API void updating() override;

    /// Cleanup vtk props.
    VISUVTKARADAPTOR_API void stopping() override;

    /// Restart the service (stop-start).
    VISUVTKARADAPTOR_API void swapping() override;

private:

    /// Slot: update point list visibility (true = visible)
    void updateVisibility ( bool isVisible );

    /// Point list actor.
    vtkSmartPointer<vtkActor> m_actor;

    /// Point color.
    ::fwData::Color::sptr m_pointColor;

    /// Point size.
    double m_radius;

};

} //namespace visuVTKARAdaptor
