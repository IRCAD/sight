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

#include "modules/viz/scene3d/adaptor/reconstruction.hpp"

#include <core/com/slots.hxx>

#include <data/mesh.hpp>

#include <service/op.hpp>

#include <viz/scene3d/material.hpp>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t CHANGE_MESH_SLOT = "changeMesh";
static const core::com::slots::key_t VISIBILITY_SLOT  = "modifyVisibility";

//------------------------------------------------------------------------------

reconstruction::reconstruction() noexcept
{
    new_slot(CHANGE_MESH_SLOT, &reconstruction::changeMesh, this);
    new_slot(VISIBILITY_SLOT, &reconstruction::modifyVisibility, this);
}

//------------------------------------------------------------------------------

void reconstruction::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );
    m_autoResetCamera = config.get<bool>(s_CONFIG + "autoresetcamera", true);

    const std::string hexaMask = config.get<std::string>(s_CONFIG + "queryFlags", "");
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

void reconstruction::starting()
{
    this->initialize();

    createMeshService();
}

//------------------------------------------------------------------------------

service::connections_t module::viz::scene3d::adaptor::reconstruction::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_RECONSTRUCTION_INPUT, data::reconstruction::MESH_CHANGED_SIG, CHANGE_MESH_SLOT);
    connections.push(s_RECONSTRUCTION_INPUT, data::reconstruction::VISIBILITY_MODIFIED_SIG, VISIBILITY_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void reconstruction::updating()
{
    if(!m_meshAdaptor.expired())
    {
        const auto reconstruction = m_reconstruction.lock();

        module::viz::scene3d::adaptor::mesh::sptr meshAdaptor = this->getMeshAdaptor();

        // Do nothing if the mesh is identical
        auto mesh = meshAdaptor->input<sight::data::mesh>("mesh").lock();
        if(mesh.get_shared() != reconstruction->getMesh())
        {
            // Updates the mesh adaptor according to the reconstruction
            meshAdaptor->setMaterial(std::const_pointer_cast<data::material>(reconstruction->getMaterial()));
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

void reconstruction::stopping()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void reconstruction::createMeshService()
{
    // Retrieves the associated Reconstruction object
    const auto reconstruction = m_reconstruction.lock();
    data::mesh::csptr mesh    = reconstruction->getMesh();
    if(mesh)
    {
        // Creates an Ogre adaptor and associates it with the Sight mesh object
        auto meshAdaptor = this->registerService<module::viz::scene3d::adaptor::mesh>(
            "sight::module::viz::scene3d::adaptor::mesh"
        );
        meshAdaptor->set_input(mesh, "mesh", true);

        meshAdaptor->set_id(this->get_id() + meshAdaptor->get_id());
        meshAdaptor->setLayerID(m_layerID);
        meshAdaptor->setRenderService(this->getRenderService());

        meshAdaptor->setIsReconstructionManaged(true);
        // Here Material cannot be const since material created by mesh can modify it.
        meshAdaptor->setMaterial(std::const_pointer_cast<data::material>(reconstruction->getMaterial()));
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

void reconstruction::setVisible(bool _hide)
{
    if(!m_meshAdaptor.expired())
    {
        module::viz::scene3d::adaptor::mesh::sptr meshAdaptor = this->getMeshAdaptor();

        if(meshAdaptor)
        {
            const auto reconstruction = m_reconstruction.lock();
            meshAdaptor->setVisible(_hide ? false : reconstruction->getIsVisible());
        }
    }
}

//------------------------------------------------------------------------------

void reconstruction::changeMesh(data::mesh::sptr /*unused*/)
{
    this->updating();
}

//------------------------------------------------------------------------------

void reconstruction::modifyVisibility()
{
    if(!m_meshAdaptor.expired())
    {
        const auto reconstruction = m_reconstruction.lock();
        this->updateVisibility(!reconstruction->getIsVisible());
    }
}

//------------------------------------------------------------------------------

adaptor::mesh::sptr reconstruction::getMeshAdaptor()
{
    // Retrieves the associated mesh adaptor
    auto adaptor     = m_meshAdaptor.lock();
    auto meshAdaptor = std::dynamic_pointer_cast<module::viz::scene3d::adaptor::mesh>(adaptor);

    return meshAdaptor;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
