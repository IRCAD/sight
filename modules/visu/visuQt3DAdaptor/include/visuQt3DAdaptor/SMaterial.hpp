/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "visuQt3DAdaptor/config.hpp"

#include <fwData/Material.hpp>

#include <fwRenderQt3D/data/Material.hpp>
#include <fwRenderQt3D/IAdaptor.hpp>

namespace visuQt3DAdaptor
{

/**
 * @brief Adapt a ::fwData::Material.
 *
 * This class handles the conversion of ::fwData::Material to Qt3D.
 *
 * @warning This adaptor must be started before every adaptor using it.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuQt3DAdaptor::SMaterial" >
        <inout key="material" uid="..." />
        <config materialName="..." />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b mesh [::fwData::Material]: adapted material.
 *
 * @subsection Configuration Configuration:
 * - \b materialName : used by other adaptors in the scene to retrieve the material handled by this adaptor.
 */
class VISUQT3DADAPTOR_CLASS_API SMaterial : public ::fwRenderQt3D::IAdaptor
{

public:

    fwCoreServiceMacro(SMaterial, ::fwRenderQt3D::IAdaptor)

    /// Sets default parameters and initializes necessary members.
    VISUQT3DADAPTOR_API SMaterial() noexcept;

    VISUQT3DADAPTOR_API virtual ~SMaterial() noexcept;

    VISUQT3DADAPTOR_API void setMaterial(::fwRenderQt3D::data::Material* _material);

    VISUQT3DADAPTOR_API void setMaterialName(std::string _materialName);

    VISUQT3DADAPTOR_API ::fwRenderQt3D::data::Material* getMaterial();

    VISUQT3DADAPTOR_API std::string getMaterialName();

    /**
     * @brief Returns proposals to connect service slots to associated object signals.
     * @return The connection map proposals.
     */
    VISUQT3DADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

private:

    /// Configures the adaptor
    void configuring() override;

    /// Manually creates a Mesh in the Default Ogre Ressource group.
    void starting() override;

    /// Deletes the mesh after unregistering the service, and shutting connections.
    void stopping() override;

    /// Called when the mesh is modified.
    void updating() override;

    ::fwRenderQt3D::data::Material* m_material;

    std::string m_materialName;
};

} // namespace visuQt3DAdaptor
