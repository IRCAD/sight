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

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwData/Plane.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkPlaneSource;
class vtkPlaneCollection;
class vtkPlane;
class vtkObject;
class vtkActor;

namespace fwData
{
class Plane;
}

namespace visuVTKAdaptor
{

/**
 * @brief Represents a Plane that can be interacted with 3 points
 *
 * @section Signals Signals
 * - \b interactionStarted(::fwData::Plane::sptr) : emitted when the user begin to interac with the plane (contains the
 *   current plane)
 *
 * @section Slots Slots
 * - \b updatePoints() : Update the plane from the points and notify planes is modified
 * - \b startInteraction() : Re-notify "startInteraction" for this service
 * - \b selectPlane(bool) : Changes points color (red is selected, else white)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SPlane" autoConnect="yes">
       <inout key="plane" uid="..." />
       <config renderer="default" picker="" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b plane [::fwData::Plane]: plane to display.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : renderer where the resection is displayed
 *    - \b picker(optional) : picker used to pick on the resection
 *    - \b planecollection(optional) : identifier of the planes collection
 */
class VISUVTKADAPTOR_CLASS_API SPlane : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceMacro(SPlane, ::fwRenderVTK::IAdaptor);

    VISUVTKADAPTOR_API SPlane() noexcept;

    VISUVTKADAPTOR_API virtual ~SPlane() noexcept;

    static const ::fwServices::IService::KeyType s_PLANE_INOUT;

    VISUVTKADAPTOR_API void setVtkPlaneCollection( vtkObject* col );

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when plane interaction is started (store current plane)
    typedef ::fwCom::Signal< void (::fwData::Plane::sptr) > InteractionStartedSignalType;
    VISUVTKADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_INTERACTION_STARTED_SIG;
    /**
     * @}
     */

    /**
     * @name Slots
     * @{
     */
    /// Changes points color (red is selected, else white)
    void selectPlane(bool select);
    /**
     * @}
     */

protected:
    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Plane::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Plane::s_SELECTED_SIG to this::s_SELECT_PLANE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    /**
     * @name Slots
     * @{
     */
    /// Update the plane from the points and notify planes is modified
    void updatePoints();

    /// Re-notify "startInteraction" for this service
    void startInteraction();
    /**
     * @}
     */

private:

    vtkPlaneSource* m_vtkPlane;
    vtkActor* m_actorPlan;

    vtkPlane* m_vtkImplicitPlane;
    vtkPlaneCollection* m_vtkPlaneCollection;

    ::fwRenderVTK::SRender::VtkObjectIdType m_planeCollectionId;

    /// register connections between plane's points and this service
    ::fwCom::helper::SigSlotConnection m_connections;
};

} //namespace visuVTKAdaptor
