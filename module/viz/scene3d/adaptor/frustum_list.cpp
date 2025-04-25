/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "module/viz/scene3d/adaptor/frustum_list.hpp"

#include <core/com/slots.hxx>

#include <viz/scene3d/helper/camera.hpp>
#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>

#include <OgreCamera.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t CLEAR_SLOT = "clear";

//-----------------------------------------------------------------------------

frustum_list::frustum_list() noexcept
{
    new_slot(CLEAR_SLOT, &frustum_list::clear, this);
}

//-----------------------------------------------------------------------------

void frustum_list::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    this->set_transform_id(
        config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            this->gen_id("transform")
        )
    );

    static const std::string s_NEAR_CONFIG   = CONFIG + "near";
    static const std::string s_FAR_CONFIG    = CONFIG + "far";
    static const std::string s_COLOR_CONFIG  = CONFIG + "color";
    static const std::string s_NB_MAX_CONFIG = CONFIG + "nbMax";

    m_near     = config.get<float>(s_NEAR_CONFIG, m_near);
    m_far      = config.get<float>(s_FAR_CONFIG, m_far);
    m_color    = config.get<std::string>(s_COLOR_CONFIG, m_color);
    m_capacity = config.get<unsigned int>(s_NB_MAX_CONFIG, m_capacity);
}

//-----------------------------------------------------------------------------

void frustum_list::starting()
{
    adaptor::init();

    m_frustum_list.set_capacity(m_capacity);

    // Create material
    m_material = std::make_unique<sight::viz::scene3d::material::standard>(gen_id("material"));
    m_material->set_shading(sight::data::material::shading_t::ambient, this->layer()->num_lights());

    sight::data::color color(m_color);
    m_material->material()->setDiffuse(Ogre::ColourValue(color[0], color[1], color[2], color[3]));
}

//-----------------------------------------------------------------------------

service::connections_t frustum_list::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();
    connections.push(TRANSFORM_INPUT, data::matrix4::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void frustum_list::updating()
{
    this->add_frustum();
    this->update_done();
    this->request_render();
}

//-----------------------------------------------------------------------------

void frustum_list::stopping()
{
    this->clear();

    m_material.reset();

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void frustum_list::set_visible(bool _visible)
{
    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);

    trans_node->setVisible(_visible);
}

//-----------------------------------------------------------------------------

void frustum_list::add_frustum()
{
    //Get camera parameters
    auto* scene_manager          = this->get_scene_manager();
    const auto camera_data       = m_camera.lock();
    const auto current_index_str = std::to_string(m_current_cam_index);
    Ogre::Camera* ogre_camera    = scene_manager->createCamera(gen_id("camera" + current_index_str));

    Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);
    trans_node->attachObject(ogre_camera);

    // Clipping
    if(m_near != 0.F)
    {
        ogre_camera->setNearClipDistance(m_near);
    }

    if(m_far != 0.F)
    {
        ogre_camera->setFarClipDistance(m_far);
    }

    if(camera_data->get_is_calibrated())
    {
        // Set data to camera
        const auto width  = static_cast<float>(camera_data->get_width());
        const auto height = static_cast<float>(camera_data->get_height());
        Ogre::Matrix4 m   =
            sight::viz::scene3d::helper::camera::compute_projection_matrix(*camera_data, width, height, m_near, m_far);
        ogre_camera->setCustomProjectionMatrix(true, m);

        if(m_frustum_list.full())
        {
            //Remove the oldest one
            auto f = m_frustum_list.back();

            f.first->detachFromParent();
            scene_manager->destroyManualObject(f.first);
        }

        auto* const frustum      = scene_manager->createManualObject(gen_id("frustum" + current_index_str));
        auto* const frustum_node = root_scene_node->createChildSceneNode("Node_" + current_index_str);

        sight::viz::scene3d::helper::manual_object::create_frustum(frustum, m_material->name(), *ogre_camera);

        this->set_transfrom_to_node(frustum_node);
        frustum_node->attachObject(frustum);

        //Add the new one
        m_frustum_list.push_front({frustum, frustum_node});

        m_current_cam_index++;

        this->updating();
    }
    else
    {
        SIGHT_ERROR("Camera isn't calibrated");
    }
}

//-----------------------------------------------------------------------------

void frustum_list::set_transfrom_to_node(Ogre::SceneNode* _node)
{
    const auto transform = m_transform.lock();
    Ogre::Affine3 ogre_mat;

    for(std::size_t lt = 0 ; lt < 4 ; lt++)
    {
        for(std::size_t ct = 0 ; ct < 4 ; ct++)
        {
            ogre_mat[ct][lt] = static_cast<Ogre::Real>((*transform)(ct, lt));
        }
    }

    // Decompose the matrix
    Ogre::Vector3 position;
    Ogre::Vector3 scale;
    Ogre::Quaternion orientation;
    ogre_mat.decomposition(position, scale, orientation);

    const Ogre::Quaternion rotate_x(Ogre::Degree(180), Ogre::Vector3(1, 0, 0));
    const Ogre::Quaternion rotate_z(Ogre::Degree(180), Ogre::Vector3(0, 0, 1));
    orientation = orientation * rotate_z * rotate_x;

    _node->setOrientation(orientation);
    _node->setPosition(position);
}

//-----------------------------------------------------------------------------

void frustum_list::clear()
{
    for(const auto& f : m_frustum_list)
    {
        f.first->detachFromParent();
        this->get_scene_manager()->destroyManualObject(f.first);
    }

    m_frustum_list.clear();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
