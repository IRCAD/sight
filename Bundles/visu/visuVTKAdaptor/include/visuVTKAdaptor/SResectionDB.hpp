/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __VISUVTKADAPTOR_SRESECTIONDB_HPP__
#define __VISUVTKADAPTOR_SRESECTIONDB_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

namespace visuVTKAdaptor
{

/**
 * @brief This service displays a ResectionDB (it launchs Resection adators)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SResectionDB" autoConnect="yes">
       <in key="resectionDB" uid="..." />
       <config renderer="default" transform="..." picker="" />
   </service>
   @endcode
 * @subsection Input Input
 * - \b resectionDB [::fwData::ResectionDB]: resectionDB to display.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : renderer where the resectionDB is displayed
 *    - \b picker(optional) : picker used to pick on the resectionDB
 *    - \b autoresetcamera(optional, default: yes) : reset the camera point of view when the data is modified ("yes" or
 *         "no", default: "yes").
 *    - \b clippingplanes(optional) : identifier of the clipping planes
 */
class VISUVTKADAPTOR_CLASS_API SResectionDB : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SResectionDB)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SResectionDB() noexcept;

    VISUVTKADAPTOR_API virtual ~SResectionDB() noexcept;

    void setClippingPlanes(::fwRenderVTK::SRender::VtkObjectIdType id);

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect SResectionDB::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect SResectionDB::s_SAFE_PART_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect SResectionDB::s_RESECTION_ADDED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

private:
    ::fwRenderVTK::SRender::VtkObjectIdType m_clippingPlanes;
    bool m_autoResetCamera;
};

//------------------------------------------------------------------------------

inline void SResectionDB::setClippingPlanes(::fwRenderVTK::SRender::VtkObjectIdType id)
{
    m_clippingPlanes = id;
}

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SRESECTIONDB_HPP__
