/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "mesh.hpp"

#include "modules/viz/qt3d/adaptor/material.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/mesh.hpp>
#include <data/mt/locked_ptr.hpp>

#include <service/registry.hpp>

#include <viz/qt3d/core/GenericScene.hpp>
#include <viz/qt3d/data/material.hpp>

#include <QQmlEngine>

//-----------------------------------------------------------------------------

namespace sight::module::viz::qt3d::adaptor
{

//-----------------------------------------------------------------------------

static const std::string s_AUTORESET_CAMERA_CONFIG = "autoresetcamera";
static const std::string s_MATERIAL_NAME_CONFIG    = "materialName";

//-----------------------------------------------------------------------------

mesh::mesh() noexcept
{
    // Allow using Mesh as QML type when using mesh service in QML applications.
    qmlRegisterType<sight::viz::qt3d::data::mesh>("sight.viz.qt3d", 1, 0, "Mesh");
    qRegisterMetaType<sight::viz::qt3d::data::mesh*>("sight.viz.qt3d.Mesh*");
}

//-----------------------------------------------------------------------------

mesh::~mesh() noexcept =
    default;

//-----------------------------------------------------------------------------

void mesh::configuring()
{
    this->configureParams();

    const config_t config_tree = this->get_config();
    const auto config          = config_tree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_autoResetCamera = config->get<bool>(s_AUTORESET_CAMERA_CONFIG, m_autoResetCamera);
        m_materialName    = config->get<std::string>(s_MATERIAL_NAME_CONFIG, m_materialName);
    }
}

//-----------------------------------------------------------------------------

void mesh::starting()
{
    this->initialize();

    // Read the mesh from the input as sight data.
    auto mesh = m_meshData.lock();
    SIGHT_ASSERT("input '" << s_MESH_INOUT << "' does not exist.", mesh);

    // Create a Qt3D mesh from sight data.
    m_mesh = new sight::viz::qt3d::data::mesh(this->getRenderService()->getScene());

    this->update();

    this->updateVisibility(m_isVisible);
}

//-----------------------------------------------------------------------------

service::connections_t mesh::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_MESH_INOUT, data::mesh::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void mesh::updating()
{
    // Read the mesh from the input as sight data.
    auto mesh = m_meshData.lock();
    SIGHT_ASSERT("input '" << s_MESH_INOUT << "' does not exist.", mesh);

    // Update the mesh and center camera if necessary.
    m_mesh->setMesh(mesh.get_shared());
    if(m_autoResetCamera)
    {
        m_mesh->centerCameraOnMesh();
    }

    // Update mesh visibility.
    m_mesh->setEnabled(m_isVisible);

    if(!m_materialName.empty())
    {
        // A material adaptor has been configured in the XML scene
        auto mtl_adaptors = sight::service::get_services<module::viz::qt3d::adaptor::material>();

        auto result =
            std::find_if(
                mtl_adaptors.begin(),
                mtl_adaptors.end(),
                [this](const module::viz::qt3d::adaptor::material::sptr& _srv)
            {
                return _srv->getMaterialName() == m_materialName;
            });

        const auto& material_adaptor = *result;

        SIGHT_ASSERT(
            "material adaptor managing material'" + m_materialName + "' is not found",
            result != mtl_adaptors.end()
        );
        m_mesh->setMaterial(material_adaptor->getMaterial());
    }
}

//-----------------------------------------------------------------------------

void mesh::stopping()
{
}

//-----------------------------------------------------------------------------

void mesh::setVisible(bool _visible)
{
    m_mesh->setEnabled(_visible);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::qt3d::adaptor.
