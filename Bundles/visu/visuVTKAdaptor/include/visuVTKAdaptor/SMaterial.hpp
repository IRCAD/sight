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

#include <fwRenderVTK/IAdaptor.hpp>

class vtkProperty;

namespace fwData
{
class Material;
}

namespace visuVTKAdaptor
{

/**
 * @brief Manage material representation of meshes
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SMaterial">
       <in key="material" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b input [::fwData::Material]: material applied on the mesh.
 */
class VISUVTKADAPTOR_CLASS_API SMaterial : public ::fwRenderVTK::IAdaptor
{

public:

    static const ::fwServices::IService::KeyType s_MATERIAL_INPUT;

    fwCoreServiceClassDefinitionsMacro( (SMaterial)(::fwRenderVTK::IAdaptor) )

    VISUVTKADAPTOR_API SMaterial() noexcept;

    VISUVTKADAPTOR_API virtual ~SMaterial() noexcept override;

    VISUVTKADAPTOR_API void setVtkProperty(vtkProperty* property);

    VISUVTKADAPTOR_API vtkProperty* getVtkProperty() const;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Material::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    void updateMaterial( CSPTR(::fwData::Material) material );

    vtkProperty* m_property;
    bool m_manageProperty;
    bool m_lighting;

};

} //namespace visuVTKAdaptor
