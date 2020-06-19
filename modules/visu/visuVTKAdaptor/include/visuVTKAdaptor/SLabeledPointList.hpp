/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{
/**
 * @brief Adaptor to display a labeled point list
 *
 * @section Slots Slots
 * - \b updateVisibility(bool) : show/hide the pointlist
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
      <service uid="..." type="::visuVTKAdaptor::SLabeledPointList" autoConnect="yes">
        <inout key="pointList" uid="..." />
        <config renderer="default" picker="..." color="#FFFFFF" radius="10" interaction="on"/>
      </service>
     @endcode
 * @subsection In-Out In-Out
 * - \b pointList [::fwData::PointList]: labeled point list to display (each point should have a label as field).
 *
 * @subsection Configuration Configuration
 * - \b renderer : defines the renderer to show the labeled point list.
 * - \b picker : defines the picker of the labeled point list.
 * - \b color(#FFFFFF) : color of the points.
 * - \b radius(double) : points radius.
 * - \b interaction (optional, default: on): if "on" interactions are enabled
 */
class VISUVTKADAPTOR_CLASS_API SLabeledPointList : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceMacro(SLabeledPointList, ::fwRenderVTK::IAdaptor);

    VISUVTKADAPTOR_API SLabeledPointList() noexcept;

    VISUVTKADAPTOR_API virtual ~SLabeledPointList() noexcept;

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
     * Connect PointList::s_POINT_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect PointList::s_POINT_REMOVED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Slot: update point list visibility (true = visible)
    VISUVTKADAPTOR_API void updateVisibility( bool isVisible );

    vtkCommand* m_rightButtonCommand;

    /// Points color
    ::fwData::Color::sptr m_ptColor;

    /// Points radius
    double m_radius;

    /// If true, the interactions are enabled
    bool m_interaction;
};

} //namespace visuVTKAdaptor
