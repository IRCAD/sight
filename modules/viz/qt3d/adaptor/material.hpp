/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <data/material.hpp>

#include <viz/qt3d/adaptor.hpp>
#include <viz/qt3d/data/material.hpp>

namespace sight::module::viz::qt3d::adaptor
{

/**
 * @brief Adapt a data::material.
 *
 * This class handles the conversion of data::material to Qt3D.
 *
 * @warning This adaptor must be started before every adaptor using it.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::qt3d::adaptor::material" >
        <inout key="material" uid="..." />
        <config materialName="..." />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b mesh [sight::data::material]: adapted material.
 *
 * @subsection Configuration Configuration:
 * - \b materialName : used by other adaptors in the scene to retrieve the material handled by this adaptor.
 */
class material : public sight::viz::qt3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(material, sight::viz::qt3d::adaptor);

    /// Sets default parameters and initializes necessary members.
    material() noexcept;

    /// Destroys the adaptor.
    ~material() noexcept override;

    /// Updates associated material.
    void set_material(sight::viz::qt3d::data::material* _material);

    /// @returns associated material.
    sight::viz::qt3d::data::material* get_material();

    /// @returns material name.
    std::string get_material_name();

protected:

    /// Configures the adaptor.
    void configuring() override;

    /// Starts the adaptor.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::MODIFIED_SIG of s_MESH_INOUT to service::slots::UPDATE.
     * Connect data::image::VERTEX_MODIFIED_SIG of s_MESH_INOUT to MODIFY_VERTICES_SLOT.
     */
    service::connections_t auto_connections() const override;

    /// Updates the material.
    void updating() override;

    /// Does nothing.
    void stopping() override;

private:

    /// Contains a Qt3D Material.
    QPointer<sight::viz::qt3d::data::material> m_material;

    /// Specifies adaptor's name.
    std::string m_material_name;

    static constexpr std::string_view MATERIAL_INOUT = "material";
    sight::data::ptr<sight::data::material, sight::data::access::inout> m_material_in_out {this, MATERIAL_INOUT};
};

} // namespace sight::module::viz::qt3d::adaptor
