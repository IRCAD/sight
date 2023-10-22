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

    const std::string hexa_mask = config.get<std::string>(s_CONFIG + "queryFlags", "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_queryFlags = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
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

        module::viz::scene3d::adaptor::mesh::sptr mesh_adaptor = this->getMeshAdaptor();

        // Do nothing if the mesh is identical
        auto mesh = mesh_adaptor->input<sight::data::mesh>("mesh").lock();
        if(mesh.get_shared() != reconstruction->getMesh())
        {
            // Updates the mesh adaptor according to the reconstruction
            mesh_adaptor->setMaterial(std::const_pointer_cast<data::material>(reconstruction->getMaterial()));
            mesh_adaptor->setVisible(reconstruction->getIsVisible());
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
        auto mesh_adaptor = this->registerService<module::viz::scene3d::adaptor::mesh>(
            "sight::module::viz::scene3d::adaptor::mesh"
        );
        mesh_adaptor->set_input(mesh, "mesh", true);

        mesh_adaptor->set_id(this->get_id() + mesh_adaptor->get_id());
        mesh_adaptor->setLayerID(m_layerID);
        mesh_adaptor->setRenderService(this->getRenderService());

        mesh_adaptor->setIsReconstructionManaged(true);
        // Here Material cannot be const since material created by mesh can modify it.
        mesh_adaptor->setMaterial(std::const_pointer_cast<data::material>(reconstruction->getMaterial()));
        mesh_adaptor->setMaterialTemplateName(m_materialTemplateName);
        mesh_adaptor->setAutoResetCamera(m_autoResetCamera);
        mesh_adaptor->setTransformId(this->getTransformId());
        mesh_adaptor->setVisible(reconstruction->getIsVisible());
        mesh_adaptor->setDynamic(m_isDynamic);
        mesh_adaptor->setDynamicVertices(m_isDynamicVertices);
        mesh_adaptor->setQueryFlags(m_queryFlags);

        mesh_adaptor->start();

        m_meshAdaptor = mesh_adaptor;
    }
}

//------------------------------------------------------------------------------

void reconstruction::setVisible(bool _hide)
{
    if(!m_meshAdaptor.expired())
    {
        module::viz::scene3d::adaptor::mesh::sptr mesh_adaptor = this->getMeshAdaptor();

        if(mesh_adaptor)
        {
            const auto reconstruction = m_reconstruction.lock();
            mesh_adaptor->setVisible(_hide ? false : reconstruction->getIsVisible());
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
    auto adaptor      = m_meshAdaptor.lock();
    auto mesh_adaptor = std::dynamic_pointer_cast<module::viz::scene3d::adaptor::mesh>(adaptor);

    return mesh_adaptor;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
