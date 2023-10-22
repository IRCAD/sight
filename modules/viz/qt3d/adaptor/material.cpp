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

#include "material.hpp"

#include <data/material.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <QQmlEngine>

//-----------------------------------------------------------------------------

namespace sight::module::viz::qt3d::adaptor
{

//-----------------------------------------------------------------------------

static const std::string s_MATERIAL_NAME_CONFIG = "materialName";

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
        m_materialName = config->get<std::string>(s_MATERIAL_NAME_CONFIG, m_materialName);
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
    connections.push(s_MATERIAL_INOUT, data::material::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void material::updating()
{
    // Reads the material from the input as sight data.
    auto material = m_materialInOut.lock();

    m_material->updatePolygonMode(material->getRepresentationMode());
    m_material->updateOptionsMode(material->getOptionsMode());
    m_material->updateShadingMode(material->getShadingMode());
    m_material->updateRGBAMode(material.get_shared());
}

//-----------------------------------------------------------------------------

void material::stopping()
{
}

//-----------------------------------------------------------------------------

void material::setMaterial(sight::viz::qt3d::data::material* _material)
{
    m_material = _material;
}

//------------------------------------------------------------------------------

sight::viz::qt3d::data::material* material::getMaterial()
{
    return m_material;
}

//------------------------------------------------------------------------------

std::string material::getMaterialName()
{
    return m_materialName;
}

} // namespace sight::module::viz::qt3d::adaptor.
