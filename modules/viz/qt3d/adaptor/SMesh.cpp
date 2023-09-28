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

#include "SMesh.hpp"

#include "modules/viz/qt3d/adaptor/SMaterial.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/Mesh.hpp>
#include <data/mt/locked_ptr.hpp>

#include <service/macros.hpp>

#include <viz/qt3d/core/GenericScene.hpp>
#include <viz/qt3d/data/Material.hpp>

#include <QQmlEngine>

//-----------------------------------------------------------------------------

namespace sight::module::viz::qt3d::adaptor
{

//-----------------------------------------------------------------------------

static const std::string s_AUTORESET_CAMERA_CONFIG = "autoresetcamera";
static const std::string s_MATERIAL_NAME_CONFIG    = "materialName";

//-----------------------------------------------------------------------------

SMesh::SMesh() noexcept
{
    // Allow using Mesh as QML type when using SMesh service in QML applications.
    qmlRegisterType<sight::viz::qt3d::data::Mesh>("sight.viz.qt3d", 1, 0, "Mesh");
    qRegisterMetaType<sight::viz::qt3d::data::Mesh*>("sight.viz.qt3d.Mesh*");
}

//-----------------------------------------------------------------------------

SMesh::~SMesh() noexcept =
    default;

//-----------------------------------------------------------------------------

void SMesh::configuring()
{
    this->configureParams();

    const ConfigType configTree = this->getConfiguration();
    const auto config           = configTree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_autoResetCamera = config->get<bool>(s_AUTORESET_CAMERA_CONFIG, m_autoResetCamera);
        m_materialName    = config->get<std::string>(s_MATERIAL_NAME_CONFIG, m_materialName);
    }
}

//-----------------------------------------------------------------------------

void SMesh::starting()
{
    this->initialize();

    // Read the mesh from the input as sight data.
    auto mesh = m_meshData.lock();
    SIGHT_ASSERT("input '" << s_MESH_INOUT << "' does not exist.", mesh);

    // Create a Qt3D mesh from sight data.
    m_mesh = new sight::viz::qt3d::data::Mesh(this->getRenderService()->getScene());

    this->update();

    this->updateVisibility(m_isVisible);
}

//-----------------------------------------------------------------------------

service::connections_t SMesh::getAutoConnections() const
{
    service::connections_t connections;
    connections.push(s_MESH_INOUT, data::Mesh::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void SMesh::updating()
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
        auto mtlAdaptors = sight::service::getServices<module::viz::qt3d::adaptor::SMaterial>();

        auto result =
            std::find_if(
                mtlAdaptors.begin(),
                mtlAdaptors.end(),
                [this](const module::viz::qt3d::adaptor::SMaterial::sptr& srv)
            {
                return srv->getMaterialName() == m_materialName;
            });

        const auto& materialAdaptor = *result;

        SIGHT_ASSERT(
            "SMaterial adaptor managing material'" + m_materialName + "' is not found",
            result != mtlAdaptors.end()
        );
        m_mesh->setMaterial(materialAdaptor->getMaterial());
    }
}

//-----------------------------------------------------------------------------

void SMesh::stopping()
{
}

//-----------------------------------------------------------------------------

void SMesh::setVisible(bool _visible)
{
    m_mesh->setEnabled(_visible);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::qt3d::adaptor.
