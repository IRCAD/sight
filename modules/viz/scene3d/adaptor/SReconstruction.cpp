/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SReconstruction.hpp"

#include <core/com/Slots.hxx>

#include <data/Mesh.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>

#include <viz/scene3d/Material.hpp>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::Slots::SlotKeyType s_CHANGE_MESH_SLOT = "changeMesh";
static const core::com::Slots::SlotKeyType s_VISIBILITY_SLOT  = "modifyVisibility";

static const std::string s_AUTORESET_CAMERA_CONFIG = "autoresetcamera";
static const std::string s_QUERY_CONFIG            = "queryFlags";

//------------------------------------------------------------------------------

SReconstruction::SReconstruction() noexcept
{
    m_slots(s_CHANGE_MESH_SLOT, &SReconstruction::changeMesh, this);
    m_slots(s_VISIBILITY_SLOT, &SReconstruction::modifyVisibility, this);
}

//------------------------------------------------------------------------------

SReconstruction::~SReconstruction() noexcept
{
}

//------------------------------------------------------------------------------

void SReconstruction::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG,
            this->getID() + "_transform"
        )
    );
    m_autoResetCamera = config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);

    const std::string hexaMask = config.get<std::string>(s_QUERY_CONFIG, "");
    if(!hexaMask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2
            && hexaMask.substr(0, 2) == "0x"
        );
        m_queryFlags = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }
}

//------------------------------------------------------------------------------

void SReconstruction::starting()
{
    this->initialize();

    createMeshService();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap module::viz::scene3d::adaptor::SReconstruction::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_RECONSTRUCTION_INPUT, data::Reconstruction::s_MESH_CHANGED_SIG, s_CHANGE_MESH_SLOT);
    connections.push(s_RECONSTRUCTION_INPUT, data::Reconstruction::s_VISIBILITY_MODIFIED_SIG, s_VISIBILITY_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void SReconstruction::updating()
{
    if(!m_meshAdaptor.expired())
    {
        const auto reconstruction = m_reconstruction.lock();

        module::viz::scene3d::adaptor::SMesh::sptr meshAdaptor = this->getMeshAdaptor();

        // Do nothing if the mesh is identical
        auto mesh = meshAdaptor->getInOut<sight::data::Mesh>("mesh").lock();
        if(mesh.get_shared() != reconstruction->getMesh())
        {
            // Updates the mesh adaptor according to the reconstruction
            meshAdaptor->setMaterial(reconstruction->getMaterial());
            meshAdaptor->setVisible(reconstruction->getIsVisible());
        }
    }
    else
    {
        // If m_meshService does not exists, we have to create it
        this->createMeshService();
    }
}

//------------------------------------------------------------------------------

void SReconstruction::stopping()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SReconstruction::createMeshService()
{
    // Retrieves the associated Reconstruction object
    const auto reconstruction = m_reconstruction.lock();
    data::Mesh::sptr mesh     = reconstruction->getMesh();
    if(mesh)
    {
        // Creates an Ogre adaptor and associates it with the Sight mesh object
        auto meshAdaptor = this->registerService<module::viz::scene3d::adaptor::SMesh>(
            "sight::module::viz::scene3d::adaptor::SMesh"
        );
        meshAdaptor->setInOut(mesh, "mesh", true);

        meshAdaptor->setID(this->getID() + meshAdaptor->getID());
        meshAdaptor->setLayerID(m_layerID);
        meshAdaptor->setRenderService(this->getRenderService());

        meshAdaptor->setIsReconstructionManaged(true);
        meshAdaptor->setMaterial(reconstruction->getMaterial());
        meshAdaptor->setMaterialTemplateName(m_materialTemplateName);
        meshAdaptor->setAutoResetCamera(m_autoResetCamera);
        meshAdaptor->setTransformId(this->getTransformId());
        meshAdaptor->setVisible(reconstruction->getIsVisible());
        meshAdaptor->setDynamic(m_isDynamic);
        meshAdaptor->setDynamicVertices(m_isDynamicVertices);
        meshAdaptor->setQueryFlags(m_queryFlags);

        meshAdaptor->start();

        m_meshAdaptor = meshAdaptor;
    }
}

//------------------------------------------------------------------------------

void SReconstruction::setVisible(bool _hide)
{
    if(!m_meshAdaptor.expired())
    {
        module::viz::scene3d::adaptor::SMesh::sptr meshAdaptor = this->getMeshAdaptor();

        if(meshAdaptor)
        {
            const auto reconstruction = m_reconstruction.lock();
            meshAdaptor->setVisible(_hide ? false : reconstruction->getIsVisible());
        }
    }
}

//------------------------------------------------------------------------------

void SReconstruction::changeMesh(data::Mesh::sptr)
{
    this->updating();
}

//------------------------------------------------------------------------------

void SReconstruction::modifyVisibility()
{
    if(!m_meshAdaptor.expired())
    {
        const auto reconstruction = m_reconstruction.lock();
        this->updateVisibility(!reconstruction->getIsVisible());
    }
}

//------------------------------------------------------------------------------

adaptor::SMesh::sptr SReconstruction::getMeshAdaptor()
{
    // Retrieves the associated mesh adaptor
    auto adaptor     = m_meshAdaptor.lock();
    auto meshAdaptor = module::viz::scene3d::adaptor::SMesh::dynamicCast(adaptor);

    return meshAdaptor;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
