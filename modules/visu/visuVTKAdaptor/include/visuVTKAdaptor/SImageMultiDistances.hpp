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

#include <fwData/PointList.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor to display distance on an image
 *
 * @section Slots Slots
 * - \b createDistance() : creates a new distance attached to this adaptor
 * - \b removeDistance(::fwData::PointList::sptr) : remove the distance
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SImageMultiDistances" autoConnect="yes">
       <inout key="image" uid="..." />
       <config renderer="default" picker="negatodefault" filter="false" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image containing the distance field.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the distances
 *    - \b picker (mandatory): identifier of the picker
 *    - \b filter (optional, default: false): if true, this adaptor only displays the distance associated to the scene
 *         (it uses a field in the image containing the uid of the scene)
 */
class VISUVTKADAPTOR_CLASS_API SImageMultiDistances : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceMacro(SImageMultiDistances, ::fwRenderVTK::IAdaptor);

    VISUVTKADAPTOR_API SImageMultiDistances() noexcept;

    VISUVTKADAPTOR_API ~SImageMultiDistances() noexcept;

    VISUVTKADAPTOR_API void setNeedSubservicesDeletion( bool _needSubservicesDeletion );

    VISUVTKADAPTOR_API virtual void show(bool showDistances = true);

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_DISTANCE_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_DISTANCE_REMOVED_SIG to this::s_REMOVE_DISTANCE_SLOT
     * Connect Image::s_DISTANCE_DISPLAYED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    void installSubServices( ::fwData::PointList::sptr pl );
    ::fwData::Point::sptr screenToWorld(int X, int Y);

    void createNewDistance( std::string sceneId );

    std::list< ::fwRenderVTK::IAdaptor::sptr > m_subServices;

    vtkCommand* m_rightButtonCommand;

    bool m_needSubservicesDeletion;

    bool m_filter;

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: remove distance
    void removeDistance(fwData::PointList::csptr pointList);

    /// Slot: to create a new distance attached to this adaptor
    void createDistance();
    /**
     * @}
     */
};

} //namespace visuVTKAdaptor
