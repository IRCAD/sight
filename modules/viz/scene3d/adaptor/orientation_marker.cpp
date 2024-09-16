/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
 * Copyright (C) 2020-2021 IHU Strasbourg
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

#include "orientation_marker.hpp"

#include <data/matrix4.hpp>

#include <viz/scene3d/layer.hpp>
#include <viz/scene3d/material.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <OGRE/OgreCamera.h>

#include <OgreEntity.h>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

void orientation_marker::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    // Set the resource this use, if it has been set via xml
    m_patient_mesh_rc = config.get<std::string>(CONFIG + "resource", m_patient_mesh_rc);

    m_marker_depth = config.get<float>(CONFIG + "depth", m_marker_depth);
}

//-----------------------------------------------------------------------------

void orientation_marker::starting()
{
    this->initialize();

    this->render_service()->make_current();

    Ogre::SceneNode* const root_scene_node = this->get_scene_manager()->getRootSceneNode();
    m_scene_node = root_scene_node->createChildSceneNode(this->get_id() + "_mainNode");

    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();

    // Set the material
    m_material = std::make_unique<sight::viz::scene3d::material>(
        this->get_id() + "_patient_mesh_material",
        sight::viz::scene3d::material::DEFAULT_MATERIAL_TEMPLATE_NAME
    );
    m_material->update_shading_mode(
        data::material::shading_t::phong,
        this->layer()->num_lights(),
        false,
        false
    );

    // Loads and attaches the marker
    m_patient_entity = scene_mgr->createEntity(m_patient_mesh_rc);
    m_patient_entity->setMaterialName(m_material->name(), sight::viz::scene3d::RESOURCE_GROUP);
    m_scene_node->attachObject(m_patient_entity);

    auto& mesh_mgr = Ogre::MeshManager::getSingleton();
    auto mesh      = mesh_mgr.createOrRetrieve(m_patient_mesh_rc, sight::viz::scene3d::RESOURCE_GROUP);
    mesh.first->load();

    this->apply_visibility();

    this->request_render();
}

//-----------------------------------------------------------------------------

void orientation_marker::updating()
{
    this->update_camera_matrix();
    this->request_render();
}

//------------------------------------------------------------------------------

void orientation_marker::update_camera_matrix()
{
    // Copy orientation matrix to Ogre.
    Ogre::Matrix3 ogre_matrix;
    {
        const auto transform = m_matrix.lock();

        // Fill the ogreMatrix.
        for(std::size_t lt = 0 ; lt < 3 ; lt++)
        {
            for(std::size_t ct = 0 ; ct < 3 ; ct++)
            {
                ogre_matrix[ct][lt] = static_cast<Ogre::Real>((*transform)(ct, lt));
            }
        }
    }

    // Convert to quaternion.
    Ogre::Quaternion orientation(ogre_matrix);

    const Ogre::Quaternion rotate_x(Ogre::Degree(180), Ogre::Vector3(1, 0, 0));

    // Reset the camera position & orientation, since s_MATRIX_IN is a global transform.
    m_scene_node->setPosition(0, 0, 0);
    // Reverse X axis.
    m_scene_node->setOrientation(rotate_x);

    // Update the camera position
    // Inverse camera matrix (since we move the mesh)
    m_scene_node->rotate(orientation.Inverse());
    // Place it at a fixed position
    m_scene_node->translate(0.F, 0.F, m_marker_depth);
}

//-----------------------------------------------------------------------------

void orientation_marker::stopping()
{
    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();

    m_patient_entity->detachFromParent();
    scene_mgr->destroyEntity(m_patient_entity);
    m_patient_entity = nullptr;

    m_material.reset();
}

//-----------------------------------------------------------------------------

void orientation_marker::set_visible(bool _visible)
{
    if(m_scene_node != nullptr)
    {
        m_scene_node->setVisible(_visible);
    }

    this->updating();
}

//-----------------------------------------------------------------------------

service::connections_t orientation_marker::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();
    connections.push(MATRIX_IN, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
