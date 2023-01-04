/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "modules/viz/qt3d/config.hpp"

#include <data/Material.hpp>

#include <viz/qt3d/data/Material.hpp>
#include <viz/qt3d/IAdaptor.hpp>

namespace sight::module::viz::qt3d::adaptor
{

/**
 * @brief Adapt a data::Material.
 *
 * This class handles the conversion of data::Material to Qt3D.
 *
 * @warning This adaptor must be started before every adaptor using it.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::qt3d::adaptor::SMaterial" >
        <inout key="material" uid="..." />
        <config materialName="..." />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b mesh [sight::data::Material]: adapted material.
 *
 * @subsection Configuration Configuration:
 * - \b materialName : used by other adaptors in the scene to retrieve the material handled by this adaptor.
 */
class MODULE_VIZ_QT3D_CLASS_API SMaterial : public sight::viz::qt3d::IAdaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SMaterial, sight::viz::qt3d::IAdaptor);

    /// Sets default parameters and initializes necessary members.
    MODULE_VIZ_QT3D_API SMaterial() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_QT3D_API ~SMaterial() noexcept override;

    /// Updates associated material.
    MODULE_VIZ_QT3D_API void setMaterial(sight::viz::qt3d::data::Material* _material);

    /// @returns associated material.
    MODULE_VIZ_QT3D_API sight::viz::qt3d::data::Material* getMaterial();

    /// @returns material name.
    MODULE_VIZ_QT3D_API std::string getMaterialName();

protected:

    /// Configures the adaptor.
    MODULE_VIZ_QT3D_API void configuring() override;

    /// Starts the adaptor.
    MODULE_VIZ_QT3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Image::s_MODIFIED_SIG of s_MESH_INOUT to IService::slots::s_UPDATE.
     * Connect data::Image::s_VERTEX_MODIFIED_SIG of s_MESH_INOUT to s_MODIFY_VERTICES_SLOT.
     */
    MODULE_VIZ_QT3D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates the material.
    MODULE_VIZ_QT3D_API void updating() override;

    /// Does nothing.
    MODULE_VIZ_QT3D_API void stopping() override;

private:

    /// Contains a Qt3D Material.
    QPointer<sight::viz::qt3d::data::Material> m_material;

    /// Specifies adaptor's name.
    std::string m_materialName;

    static constexpr std::string_view s_MATERIAL_INOUT = "material";
    sight::data::ptr<sight::data::Material, sight::data::Access::inout> m_materialInOut {this, s_MATERIAL_INOUT};
};

} // namespace sight::module::viz::qt3d::adaptor
