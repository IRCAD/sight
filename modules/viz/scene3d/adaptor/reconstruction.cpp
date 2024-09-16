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
    new_slot(CHANGE_MESH_SLOT, &reconstruction::change_mesh, this);
    new_slot(VISIBILITY_SLOT, &reconstruction::modify_visibility, this);
}

//------------------------------------------------------------------------------

void reconstruction::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    this->set_transform_id(
        config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );
    m_auto_reset_camera = config.get<bool>(CONFIG + "autoresetcamera", true);

    const std::string hexa_mask = config.get<std::string>(CONFIG + "queryFlags", "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_query_flags = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }
}

//------------------------------------------------------------------------------

void reconstruction::starting()
{
    this->initialize();

    create_mesh_service();
}

//------------------------------------------------------------------------------

service::connections_t module::viz::scene3d::adaptor::reconstruction::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();
    connections.push(RECONSTRUCTION_INPUT, data::reconstruction::MESH_CHANGED_SIG, CHANGE_MESH_SLOT);
    connections.push(RECONSTRUCTION_INPUT, data::reconstruction::VISIBILITY_MODIFIED_SIG, VISIBILITY_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void reconstruction::updating()
{
    if(!m_mesh_adaptor.expired())
    {
        const auto reconstruction = m_reconstruction.lock();

        module::viz::scene3d::adaptor::mesh::sptr mesh_adaptor = this->get_mesh_adaptor();

        // Do nothing if the mesh is identical
        auto mesh = mesh_adaptor->input<sight::data::mesh>("mesh").lock();
        if(mesh.get_shared() != reconstruction->get_mesh())
        {
            // Updates the mesh adaptor according to the reconstruction
            mesh_adaptor->set_material(std::const_pointer_cast<data::material>(reconstruction->get_material()));
            mesh_adaptor->set_visible(reconstruction->get_is_visible());
        }
    }
    else
    {
        // If m_meshService does not exists, we have to create it
        this->create_mesh_service();
    }
}

//------------------------------------------------------------------------------

void reconstruction::stopping()
{
    this->unregister_services();
}

//------------------------------------------------------------------------------

void reconstruction::create_mesh_service()
{
    // Retrieves the associated Reconstruction object
    const auto reconstruction = m_reconstruction.lock();
    data::mesh::csptr mesh    = reconstruction->get_mesh();
    if(mesh)
    {
        // Creates an Ogre adaptor and associates it with the Sight mesh object
        auto mesh_adaptor = this->register_service<module::viz::scene3d::adaptor::mesh>(
            "sight::module::viz::scene3d::adaptor::mesh"
        );
        mesh_adaptor->set_input(mesh, "mesh", true);

        mesh_adaptor->configure();
        mesh_adaptor->set_id(this->get_id() + mesh_adaptor->get_id());
        mesh_adaptor->set_layer_id(m_layer_id);
        mesh_adaptor->set_render_service(this->render_service());

        mesh_adaptor->set_is_reconstruction_managed(true);
        // Here Material cannot be const since material created by mesh can modify it.
        mesh_adaptor->set_material(std::const_pointer_cast<data::material>(reconstruction->get_material()));
        mesh_adaptor->set_material_template_name(m_material_template_name);
        mesh_adaptor->set_auto_reset_camera(m_auto_reset_camera);
        mesh_adaptor->set_transform_id(this->get_transform_id());
        mesh_adaptor->set_visible(reconstruction->get_is_visible());
        mesh_adaptor->set_dynamic(m_is_dynamic);
        mesh_adaptor->set_dynamic_vertices(m_is_dynamic_vertices);
        mesh_adaptor->set_query_flags(m_query_flags);

        mesh_adaptor->start();

        m_mesh_adaptor = mesh_adaptor;
    }
}

//------------------------------------------------------------------------------

void reconstruction::set_visible(bool _visible)
{
    if(!m_mesh_adaptor.expired())
    {
        module::viz::scene3d::adaptor::mesh::sptr mesh_adaptor = this->get_mesh_adaptor();

        if(mesh_adaptor)
        {
            const auto reconstruction = m_reconstruction.lock();
            mesh_adaptor->set_visible(_visible ? false : reconstruction->get_is_visible());
        }
    }
}

//------------------------------------------------------------------------------

void reconstruction::change_mesh(data::mesh::sptr /*unused*/)
{
    this->updating();
}

//------------------------------------------------------------------------------

void reconstruction::modify_visibility()
{
    if(!m_mesh_adaptor.expired())
    {
        const auto reconstruction = m_reconstruction.lock();
        this->update_visibility(!reconstruction->get_is_visible());
    }
}

//------------------------------------------------------------------------------

adaptor::mesh::sptr reconstruction::get_mesh_adaptor()
{
    // Retrieves the associated mesh adaptor
    auto adaptor      = m_mesh_adaptor.lock();
    auto mesh_adaptor = std::dynamic_pointer_cast<module::viz::scene3d::adaptor::mesh>(adaptor);

    return mesh_adaptor;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
