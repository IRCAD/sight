/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

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

static const core::com::Slots::SlotKeyType s_MODIFY_VERTICES_SLOT = "modifyVertices";

static const std::string s_MESH_INOUT = "mesh";

static const std::string s_AUTORESET_CAMERA_CONFIG = "autoresetcamera";
static const std::string s_MATERIAL_NAME_CONFIG    = "materialName";
static const std::string s_VISIBLE_CONFIG          = "visible";

//-----------------------------------------------------------------------------

SMesh::SMesh() noexcept
{
    // Allow using Mesh as QML type when using SMesh service in QML applications.
    qmlRegisterType< sight::viz::qt3d::data::Mesh >("sight::viz::qt3d", 1, 0, "Mesh");
    qRegisterMetaType< sight::viz::qt3d::data::Mesh* >("sight::viz::qt3d::data::Mesh*");

    newSlot(s_MODIFY_VERTICES_SLOT, &SMesh::modifyVertices, this);
}

//-----------------------------------------------------------------------------

SMesh::~SMesh() noexcept
{
}

//-----------------------------------------------------------------------------

void SMesh::configuring()
{
    const ConfigType configTree = this->getConfigTree();
    const auto config           = configTree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_autoResetCamera = config->get<bool>(s_AUTORESET_CAMERA_CONFIG, m_autoResetCamera);
        m_isVisible       = config->get<bool>(s_VISIBLE_CONFIG, m_isVisible);
        m_materialName    = config->get<std::string>(s_MATERIAL_NAME_CONFIG, m_materialName);
    }
}

//-----------------------------------------------------------------------------

void SMesh::starting()
{
    this->initialize();

    // Read the mesh from the input as sight data.
    auto mesh = this->getLockedInOut< data::Mesh >(s_MESH_INOUT);
    SIGHT_ASSERT("input '" + s_MESH_INOUT + "' does not exist.", mesh.get_shared());

    // Create a Qt3D mesh from sight data.
    m_mesh = new sight::viz::qt3d::data::Mesh(this->getRenderService()->getScene());
    m_mesh->setMesh(mesh.get_shared());

    if(!m_materialName.empty())
    {
        // A material adaptor has been configured in the XML scene
        auto mtlAdaptors = service::OSR::getServices< module::viz::qt3d::adaptor::SMaterial >();

        auto result =
            std::find_if(mtlAdaptors.begin(), mtlAdaptors.end(),
                         [this](const module::viz::qt3d::adaptor::SMaterial::sptr srv)
            {
                return srv->getMaterialName() == m_materialName;
            });

        SIGHT_ASSERT("SMaterial adaptor managing material'" + m_materialName + "' is not found",
                     result != mtlAdaptors.end());

        const module::viz::qt3d::adaptor::SMaterial::sptr materialAdaptor = *result;

        m_mesh->setMaterial(materialAdaptor->getMaterial());
    }

    // Center camera on mesh if necessary.
    if(m_autoResetCamera)
    {
        m_mesh->centerCameraOnMesh();
    }

    // Set mesh visibility.
    m_mesh->setEnabled(m_isVisible);
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SMesh::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_MESH_INOUT, data::Mesh::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MESH_INOUT, data::Mesh::s_VERTEX_MODIFIED_SIG, s_MODIFY_VERTICES_SLOT);
    return connections;
}

//-----------------------------------------------------------------------------

void SMesh::updating()
{
    // Read the mesh from the input as sight data.
    auto mesh = this->getLockedInOut< data::Mesh >(s_MESH_INOUT);
    SIGHT_ASSERT("input '" + s_MESH_INOUT + "' does not exist.", mesh.get_shared());

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
        auto mtlAdaptors = service::OSR::getServices< module::viz::qt3d::adaptor::SMaterial >();

        auto result =
            std::find_if(mtlAdaptors.begin(), mtlAdaptors.end(),
                         [this](const module::viz::qt3d::adaptor::SMaterial::sptr& srv)
            {
                return srv->getMaterialName() == m_materialName;
            });

        auto materialAdaptor = *result;

        SIGHT_ASSERT("SMaterial adaptor managing material'" + m_materialName + "' is not found",
                     result != mtlAdaptors.end());
        m_mesh->setMaterial(materialAdaptor->getMaterial());
    }
}

//-----------------------------------------------------------------------------

void SMesh::stopping()
{
}

//-----------------------------------------------------------------------------

void SMesh::updateVisibility(bool _visibility)
{
    // Enable/disable qt3d entity according to _visibility.
    m_isVisible = _visibility;
    m_mesh->setEnabled(m_isVisible);
}

//-----------------------------------------------------------------------------

void SMesh::modifyVertices()
{
    // Read the mesh from the input as sight data.
    auto mesh = this->getLockedInOut< data::Mesh >(s_MESH_INOUT);
    SIGHT_ASSERT("input '" + s_MESH_INOUT + "' does not exist.", mesh.get_shared());

    // Update mesh position and normal buffers.
    m_mesh->buildBuffers(mesh.get_shared());
}

} // namespace sight::module::viz::qt3d::adaptor.
