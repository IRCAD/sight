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

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Plane.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{

/**
 * @brief Represents a list of Plane that can be interacted with 3 points
 *
 * @section Signals Signals
 * - \b interactionStarted(::fwData::Plane::sptr) : emitted when the user begin to interac with the plane (contains the
 *   current plane)
 *
 * @section Slots Slots
 * - \b updateSelection(::fwData::Plane::sptr) : Update plane selection
 * - \b updatePlanes() : Update planes
 * - \b showPlanes(bool) : Show/hide planes
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SPlaneList">
       <inout key="planes" uid="..." />
       config renderer="default" picker="" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b plane [::fwData::Plane]: plane to display.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : renderer where the resection is displayed
 *    - \b picker(optional) : picker used to pick on the resection
 *    - \b autoresetcamera(optional, default: yes) : reset the camera point of view when the data is modified ("yes" or
 *         "no", default: "yes").
 *    - \b planecollection(optional) : identifier of the planes collection
 */
class VISUVTKADAPTOR_CLASS_API SPlaneList : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceMacro(SPlaneList, ::fwRenderVTK::IAdaptor);

    VISUVTKADAPTOR_API SPlaneList() noexcept;

    VISUVTKADAPTOR_API virtual ~SPlaneList() noexcept;

    //------------------------------------------------------------------------------

    void setPlaneCollectionId(::fwRenderVTK::SRender::VtkObjectIdType id)
    {
        m_planeCollectionId = id;
    }

    //------------------------------------------------------------------------------

    ::fwRenderVTK::SRender::VtkObjectIdType getPlaneCollectionId() const
    {
        return m_planeCollectionId;
    }

protected:

    /// Type of signal emitted when plane selection changed
    typedef ::fwCom::Signal< void (::fwData::Plane::sptr) > SelectedignalType;

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Plane::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Slot: Update plane selection
    void updateSelection(::fwData::Plane::sptr plane);

    vtkCommand* m_rightButtonCommand;
    ::fwRenderVTK::SRender::VtkObjectIdType m_planeCollectionId;

    /// Store connections to planes adaptors.
    ::fwCom::helper::SigSlotConnection m_planeConnections;

private:
    /**
     * @name Slots
     * @{
     */
    /// Update planes (call doStop-soStart())
    void updatePlanes();

    /// Show/hide planes
    void showPlanes(bool visible);
    /**
     * @}
     */
};

} //namespace visuVTKAdaptor
