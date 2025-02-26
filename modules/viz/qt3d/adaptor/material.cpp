/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include "material.hpp"

#include <data/material.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <QQmlEngine>

//-----------------------------------------------------------------------------

namespace sight::module::viz::qt3d::adaptor
{

//-----------------------------------------------------------------------------

static const std::string MATERIAL_NAME_CONFIG = "materialName";

//-----------------------------------------------------------------------------

material::material() noexcept
{
    // Allows using Material as QML type when using material service in QML applications.
    qmlRegisterType<sight::viz::qt3d::data::material>("sight.viz.qt3d", 1, 0, "Material");
    qRegisterMetaType<sight::viz::qt3d::data::material*>("sight.viz.qt3d.Material*");
}

//-----------------------------------------------------------------------------

material::~material() noexcept =
    default;

//-----------------------------------------------------------------------------

void material::configuring()
{
    const auto config_tree = this->get_config();
    const auto config      = config_tree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_material_name = config->get<std::string>(MATERIAL_NAME_CONFIG, m_material_name);
    }
}

//-----------------------------------------------------------------------------

void material::starting()
{
    this->initialize();

    // Creates a Qt3D mesh from sight data.
    m_material = new sight::viz::qt3d::data::material();

    this->updating();
}

//------------------------------------------------------------------------------

service::connections_t material::auto_connections() const
{
    service::connections_t connections;
    connections.push(MATERIAL_INOUT, data::material::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void material::updating()
{
    // Reads the material from the input as sight data.
    auto material = m_material_in_out.lock();

    m_material->update_polygon_mode(material->get_representation_mode());
    m_material->update_options_mode(material->get_options_mode());
    m_material->set_shading(static_cast<int>(material->get_shading_mode()));
    m_material->update_rgba_mode(material.get_shared());
}

//-----------------------------------------------------------------------------

void material::stopping()
{
}

//-----------------------------------------------------------------------------

void material::set_material(sight::viz::qt3d::data::material* _material)
{
    m_material = _material;
}

//------------------------------------------------------------------------------

sight::viz::qt3d::data::material* material::get_material()
{
    return m_material;
}

//------------------------------------------------------------------------------

std::string material::get_material_name()
{
    return m_material_name;
}

} // namespace sight::module::viz::qt3d::adaptor.
