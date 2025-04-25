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

#include "module/viz/scene3d/adaptor/frustum.hpp"

#include <core/com/slots.hxx>

#include <viz/scene3d/helper/camera.hpp>
#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreMaterial.h>
#include <OgreMeshManager.h>

namespace sight::module::viz::scene3d::adaptor
{

//------------------------------------------------------------------------------

void frustum::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    this->set_transform_id(
        config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            gen_id("transform")
        )
    );

    static const std::string s_NEAR_CONFIG  = CONFIG + "near";
    static const std::string s_FAR_CONFIG   = CONFIG + "far";
    static const std::string s_COLOR_CONFIG = CONFIG + "color";

    m_near  = config.get<float>(s_NEAR_CONFIG, m_near);
    m_far   = config.get<float>(s_FAR_CONFIG, m_far);
    m_color = config.get<std::string>(s_COLOR_CONFIG, m_color);
}

//-----------------------------------------------------------------------------

void frustum::starting()
{
    adaptor::init();

    // Create camera
    m_ogre_camera = this->get_scene_manager()->createCamera(gen_id(CAMERA_INPUT));
    m_ogre_camera->setVisible(visible());

    // Clipping
    if(m_near != 0.F)
    {
        m_ogre_camera->setNearClipDistance(m_near);
    }

    if(m_far != 0.F)
    {
        m_ogre_camera->setFarClipDistance(m_far);
    }

    // Set data to camera
    this->set_ogre_cam_from_data();

    // Add camera to ogre scene
    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);
    trans_node->attachObject(m_ogre_camera);

    // Set position
    trans_node->setPosition(Ogre::Vector3(0, 0, 0));
    trans_node->setDirection(Ogre::Vector3(Ogre::Real(0), Ogre::Real(0), Ogre::Real(1)));

    // Create material for the frustum
    m_material = std::make_unique<sight::viz::scene3d::material::standard>(gen_id("material"));
    m_material->set_shading(sight::data::material::shading_t::ambient, this->layer()->num_lights());

    sight::data::color color(m_color);
    m_material->material()->setDiffuse(Ogre::ColourValue(color[0], color[1], color[2], color[3]));

    m_frustum = this->get_scene_manager()->createManualObject(gen_id("frustum"));
    sight::viz::scene3d::helper::manual_object::create_frustum(m_frustum, m_material->name(), *m_ogre_camera);
    trans_node->attachObject(m_frustum);

    this->request_render();
}

//-----------------------------------------------------------------------------

service::connections_t frustum::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();
    connections.push(CAMERA_INPUT, data::camera::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);
    connections.push(CAMERA_INPUT, data::camera::INTRINSIC_CALIBRATED_SIG, adaptor::slots::LAZY_UPDATE);

    return connections;
}

//-----------------------------------------------------------------------------

void frustum::updating()
{
    this->set_ogre_cam_from_data();
    m_frustum->setVisible(visible());

    this->update_done();
    this->request_render();
}

//-----------------------------------------------------------------------------

void frustum::stopping()
{
    m_frustum->detachFromParent();
    m_ogre_camera->detachFromParent();
    this->get_scene_manager()->destroyManualObject(m_frustum);
    this->get_scene_manager()->destroyCamera(m_ogre_camera);

    m_ogre_camera = nullptr;
    m_material.reset();

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void frustum::set_ogre_cam_from_data()
{
    const auto camera = m_camera.lock();

    if(camera->get_is_calibrated())
    {
        const auto width  = static_cast<float>(camera->get_width());
        const auto height = static_cast<float>(camera->get_height());

        Ogre::Matrix4 m =
            sight::viz::scene3d::helper::camera::compute_projection_matrix(*camera, width, height, m_near, m_far);

        m_ogre_camera->setCustomProjectionMatrix(true, m);
    }
    else
    {
        SIGHT_WARN("The camera '" + std::string(CAMERA_INPUT) + "' is not calibrated");
    }
}

//-----------------------------------------------------------------------------

void frustum::set_visible(bool _is_visible)
{
    m_frustum->setVisible(_is_visible);
    this->request_render();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
