/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
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

#include <viz/scene3d/helper/camera.hpp>
#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OgreCamera.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

frustum_list::frustum_list() noexcept
{
    new_slot(slots::CLEAR, &frustum_list::clear, this);
    new_slot(
        slots::ADD_FRUSTUM,
        &frustum_list::add_frustum,
        this
    );
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

    this->render_service()->make_current();

    m_frustum_list.set_capacity(m_capacity);

    // Create material
    m_material = std::make_unique<sight::viz::scene3d::material::standard>(gen_id("material"));
    m_material->set_shading(sight::data::material::shading_t::ambient, this->layer()->num_lights());

    sight::data::color color(m_color);
    m_material->material()->setDiffuse(Ogre::ColourValue(color[0], color[1], color[2], color[3]));

    // Create the Ogre camera once. It is only used to compute each frustum's corners, never rendered from.
    auto* scene_manager = this->get_scene_manager();
    m_ogre_camera = scene_manager->createCamera(gen_id("camera"));

    if(m_near != 0.F)
    {
        m_ogre_camera->setNearClipDistance(m_near);
    }

    if(m_far != 0.F)
    {
        m_ogre_camera->setFarClipDistance(m_far);
    }
}

//-----------------------------------------------------------------------------

void frustum_list::updating()
{
    this->render_service()->make_current();

    this->update_done();
    this->request_render();
}

//-----------------------------------------------------------------------------

void frustum_list::stopping()
{
    this->render_service()->make_current();

    this->clear();

    this->get_scene_manager()->destroyCamera(m_ogre_camera);
    m_ogre_camera = nullptr;

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

void frustum_list::add_frustum(sight::data::matrix4::sptr _matrix)
{
    this->render_service()->make_current();

    //Get camera parameters
    auto* scene_manager          = this->get_scene_manager();
    const auto camera_data       = m_camera.lock();
    const auto current_index_str = std::to_string(m_current_cam_index);

    if(camera_data->get_is_calibrated())
    {
        // Set data to camera
        const auto width  = static_cast<float>(camera_data->get_width());
        const auto height = static_cast<float>(camera_data->get_height());
        Ogre::Matrix4 m   =
            sight::viz::scene3d::helper::camera::compute_projection_matrix(*camera_data, width, height, m_near, m_far);
        m_ogre_camera->setCustomProjectionMatrix(true, m);

        if(m_frustum_list.full())
        {
            //Remove the oldest one
            auto f = m_frustum_list.back();

            f.first->detachFromParent();
            scene_manager->destroyManualObject(f.first);
            scene_manager->destroySceneNode(f.second);
        }

        auto* const frustum = scene_manager->createManualObject(gen_id("frustum" + current_index_str));
        frustum->setRenderQueueGroup(sight::viz::scene3d::rq::SURFACE);
        auto* const frustum_node = scene_manager->getRootSceneNode()->createChildSceneNode("Node_" + current_index_str);

        sight::viz::scene3d::helper::manual_object::create_frustum(frustum, m_material->name(), *m_ogre_camera);

        set_transform_to_node(frustum_node, _matrix);
        frustum_node->attachObject(frustum);

        //Add the new one
        m_frustum_list.push_front({frustum, frustum_node});

        m_current_cam_index++;

        this->request_render();
    }
    else
    {
        SIGHT_ERROR("Camera isn't calibrated");
    }
}

//-----------------------------------------------------------------------------

void frustum_list::set_transform_to_node(Ogre::SceneNode* _node, const sight::data::matrix4::csptr& _matrix)
{
    Ogre::Affine3 ogre_mat(sight::viz::scene3d::utils::to_ogre_matrix(_matrix));

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
        this->get_scene_manager()->destroySceneNode(f.second);
    }

    m_frustum_list.clear();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
