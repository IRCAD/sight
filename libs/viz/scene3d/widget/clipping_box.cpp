/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "viz/scene3d/widget/clipping_box.hpp"

#include "viz/scene3d/compositor/core.hpp"
#include "viz/scene3d/helper/camera.hpp"

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgrePlane.h>
#include <OGRE/OgreRay.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreViewport.h>

#include <numeric>
#include <utility>

namespace sight::viz::scene3d::widget
{

//-----------------------------------------------------------------------------

clipping_box::clipping_box(
    std::string _id,
    Ogre::SceneNode* _parent_scene_node,
    Ogre::Camera* _camera,
    Ogre::SceneManager* _scene_manager,
    const Ogre::Matrix4& _clipping_matrix,
    clipping_update_callback_t _clipping_update_callback,
    const std::string& _box_mtl_name,
    const std::string& _handle_mtl_name
) :
    m_id(std::move(_id)),
    m_scene_manager(_scene_manager),
    m_camera(_camera),
    m_volume_scene_node(_parent_scene_node),
    m_widget_scene_node(m_volume_scene_node->createChildSceneNode()),
    m_handle_mtl(Ogre::MaterialManager::getSingleton().getByName(_handle_mtl_name, RESOURCE_GROUP)),
    m_box_mtl(Ogre::MaterialManager::getSingleton().getByName(_box_mtl_name, RESOURCE_GROUP)),
    m_clipping_update_callback(std::move(_clipping_update_callback))
{
    SIGHT_ASSERT("Missing '" + _box_mtl_name + "' material.", m_box_mtl);
    SIGHT_ASSERT("Missing '" + _handle_mtl_name + "' material.", m_handle_mtl);

    this->init_widgets();
    this->apply_transform(_clipping_matrix);
}

//-----------------------------------------------------------------------------

clipping_box::~clipping_box()
{
    m_widget_scene_node->detachAllObjects();

    m_scene_manager->destroyManualObject(m_selected_face);
    m_scene_manager->destroyManualObject(m_bounding_box);

    for(auto& widget : m_widgets)
    {
        const Ogre::MovableObject* mo = widget.first;
        m_scene_manager->destroyMovableObject(mo->getName(), mo->getMovableType());
    }

    Ogre::MaterialManager::getSingleton().remove(m_handle_hightlight_mtl->getHandle());
    Ogre::MaterialManager::getSingleton().remove(m_box_highlight_mtl->getHandle());
    Ogre::MaterialManager::getSingleton().remove(m_box_face_mtl->getHandle());

    m_scene_manager->destroySceneNode(m_widget_scene_node);
}

//-----------------------------------------------------------------------------

bool clipping_box::belongs_to_widget(const Ogre::MovableObject* const _object) const
{
    return m_widgets.find(_object) != m_widgets.end();
}

//-----------------------------------------------------------------------------

std::array<Ogre::Vector3,
           4> clipping_box::get_face_positions(viz::scene3d::vr::volume_renderer::cube_face _face_name) const
{
    auto position_indices = vr::volume_renderer::CUBE_FACES.at(_face_name);
    std::array<Ogre::Vector3, 4> face_positions;

    const auto bb_positions = this->clipping_box_positions();

    std::transform(
        position_indices.begin(),
        position_indices.end(),
        face_positions.begin(),
        [&](unsigned _i){return bb_positions[_i];});

    return face_positions;
}

//-----------------------------------------------------------------------------

Ogre::Vector3 clipping_box::get_face_center(viz::scene3d::vr::volume_renderer::cube_face _face_name) const
{
    const auto face_positions = this->get_face_positions(_face_name);
    return std::accumulate(face_positions.cbegin() + 1, face_positions.cend(), face_positions[0]) / 4.F;
}

//-----------------------------------------------------------------------------

std::array<Ogre::Vector3, 8> clipping_box::clipping_box_positions() const
{
    return {{
        Ogre::Vector3(m_clipping_cube[1].x, m_clipping_cube[1].y, m_clipping_cube[1].z),
        Ogre::Vector3(m_clipping_cube[1].x, m_clipping_cube[0].y, m_clipping_cube[1].z),
        Ogre::Vector3(m_clipping_cube[1].x, m_clipping_cube[1].y, m_clipping_cube[0].z),
        Ogre::Vector3(m_clipping_cube[0].x, m_clipping_cube[1].y, m_clipping_cube[1].z),
        Ogre::Vector3(m_clipping_cube[0].x, m_clipping_cube[0].y, m_clipping_cube[1].z),
        Ogre::Vector3(m_clipping_cube[1].x, m_clipping_cube[0].y, m_clipping_cube[0].z),
        Ogre::Vector3(m_clipping_cube[0].x, m_clipping_cube[1].y, m_clipping_cube[0].z),
        Ogre::Vector3(m_clipping_cube[0].x, m_clipping_cube[0].y, m_clipping_cube[0].z)
    }
    };
}

//-----------------------------------------------------------------------------

void clipping_box::update_widgets()
{
    const auto clipping_box_positions = this->clipping_box_positions();

    m_bounding_box->beginUpdate(0);
    {
        // Box
        for(unsigned i = 0 ; i < 12 ; ++i)
        {
            const auto cube_edge = vr::volume_renderer::CUBE_EDGES[i];

            m_bounding_box->position(clipping_box_positions[cube_edge.first]);
            m_bounding_box->position(clipping_box_positions[cube_edge.second]);
        }

        // Cross
        for(unsigned i = 0 ; i < 6 ; i += 2)
        {
            const auto face_center          = this->get_face_center(static_cast<vr::volume_renderer::cube_face>(i));
            const auto opposite_face_center = this->get_face_center(static_cast<vr::volume_renderer::cube_face>(i + 1));

            m_bounding_box->position(face_center);
            m_bounding_box->position(opposite_face_center);
        }
    }
    m_bounding_box->end();

    // Recenter widgets
    for(auto& widget : m_widgets)
    {
        const auto& widget_face_node_pair = widget.second;
        const Ogre::Vector3 face_center   = this->get_face_center(widget_face_node_pair.first);

        auto* widget_scene_node = widget_face_node_pair.second;
        widget_scene_node->setPosition(face_center);
        widget_scene_node->needUpdate();
    }
}

//-----------------------------------------------------------------------------

void clipping_box::init_widgets()
{
    // Create widget materials
    {
        m_handle_mtl->setAmbient(0.1F, 0.1F, 0.1F);
        m_handle_mtl->setDiffuse(1.F, 1.F, 1.F, 1.F);
        m_handle_mtl->setSpecular(0.F, 0.F, 0.F, 1.F);
        m_handle_mtl->setShininess(1.F);

        m_handle_hightlight_mtl = m_handle_mtl->clone(m_id + "_SphereHighlight", true, RESOURCE_GROUP);
        m_handle_hightlight_mtl->setAmbient(0.3F, 0.F, 0.F);
        m_handle_hightlight_mtl->setDiffuse(0.5F, 0.1F, 0.1F, 1.F);

        m_box_mtl->setAmbient(1.F, 1.F, 1.F);
        m_box_mtl->setDiffuse(0.F, 0.F, 0.F, 1.F);
        m_box_mtl->setSpecular(0.F, 0.F, 0.F, 1.F);

        m_box_highlight_mtl = m_box_mtl->clone(m_id + "_FrameHighlight", true, RESOURCE_GROUP);
        m_box_highlight_mtl->setAmbient(0.F, 1.F, 0.F);

        m_box_face_mtl = m_box_mtl->clone(m_id + "_FaceHighlight", true, RESOURCE_GROUP);
        m_box_face_mtl->setAmbient(1.F, 1.F, 0.F);
        m_box_face_mtl->setDiffuse(0.F, 0.F, 0.F, 0.6F);
        m_box_face_mtl->setSpecular(0.F, 0.F, 0.F, 0.6F);
        m_box_face_mtl->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        m_box_face_mtl->setDepthWriteEnabled(false);
    }

    m_bounding_box  = m_scene_manager->createManualObject(m_id + "_VolumeBB");
    m_selected_face = m_scene_manager->createManualObject(m_id + "_VRSelectedFace");

    const auto clipping_box_positions = this->clipping_box_positions();

    m_bounding_box->begin(m_box_mtl->getName(), Ogre::RenderOperation::OT_LINE_LIST, RESOURCE_GROUP);
    {
        for(unsigned i = 0 ; i < 12 ; ++i)
        {
            const auto cube_edge = vr::volume_renderer::CUBE_EDGES[i];

            m_bounding_box->position(clipping_box_positions[cube_edge.first]);
            m_bounding_box->position(clipping_box_positions[cube_edge.second]);
        }

        // Cross
        for(unsigned i = 0 ; i < 6 ; i += 2)
        {
            const auto face_center          = this->get_face_center(static_cast<vr::volume_renderer::cube_face>(i));
            const auto opposite_face_center = this->get_face_center(static_cast<vr::volume_renderer::cube_face>(i + 1));

            m_bounding_box->position(face_center);
            m_bounding_box->position(opposite_face_center);
        }
    }
    m_bounding_box->end();

    m_widget_scene_node->attachObject(m_bounding_box);

    m_selected_face->begin(m_box_face_mtl->getName(), Ogre::RenderOperation::OT_TRIANGLE_STRIP, RESOURCE_GROUP);
    {
        for(unsigned i = 0 ; i < 4 ; ++i)
        {
            m_selected_face->position(0, 0, 0);
        }
    }
    m_selected_face->end();

    // Render highlighted faces after other surfaces but before volumes.
    m_selected_face->setRenderQueueGroup(viz::scene3d::rq::SURFACE_ID + 1);

    // Create a pickable sphere for each cube face
    for(unsigned i = 0 ; i < 6 ; ++i)
    {
        const auto current_face = static_cast<vr::volume_renderer::cube_face>(i);

        Ogre::Entity* new_widget = m_scene_manager->createEntity(Ogre::SceneManager::PT_SPHERE);
        new_widget->setMaterial(m_handle_mtl);

        Ogre::SceneNode* sphere_scene_node = m_widget_scene_node->createChildSceneNode();

        m_widgets[new_widget] = std::make_pair(current_face, sphere_scene_node);

        const Ogre::Vector3 face_center = this->get_face_center(current_face);

        sphere_scene_node->setPosition(face_center);
        sphere_scene_node->setInheritScale(false);

        const auto vol_scale       = m_volume_scene_node->getScale();
        const Ogre::Real scale_min = std::min(vol_scale[0], std::min(vol_scale[1], vol_scale[2]));

        // Scale the handle to be 1/100th of the volume's initial size.
        const Ogre::Vector3 widget_scale((0.02F * scale_min) / new_widget->getBoundingRadius());
        sphere_scene_node->setScale(widget_scale);

        sphere_scene_node->attachObject(new_widget);
    }
}

//-----------------------------------------------------------------------------

void clipping_box::select_face(viz::scene3d::vr::volume_renderer::cube_face _face_name)
{
    m_selected_face->beginUpdate(0);
    {
        const std::array<Ogre::Vector3, 4> face_positions = this->get_face_positions(_face_name);
        m_selected_face->position(face_positions[1]);
        m_selected_face->position(face_positions[0]);
        m_selected_face->position(face_positions[2]);
        m_selected_face->position(face_positions[3]);
    }
    m_selected_face->end();

    m_widget_scene_node->attachObject(m_selected_face);
}

//-----------------------------------------------------------------------------

void clipping_box::deselect_face()
{
    if(m_selected_face->isAttached())
    {
        m_widget_scene_node->detachObject(m_selected_face->getName());
    }
}

//-----------------------------------------------------------------------------

void clipping_box::widget_picked(Ogre::MovableObject* _picked_widget, int _screen_x, int _screen_y)
{
    SIGHT_ASSERT("The picked widget does not belong to this widget.", this->belongs_to_widget(_picked_widget));
    const auto face = m_widgets.at(_picked_widget);

    this->deselect_face();

    if(this->get_visibility())
    {
        const viz::scene3d::vr::volume_renderer::cube_face widget_face = face.first;
        const Ogre::SceneNode* widget_scene_node                       = face.second;

        const auto vp_pos = helper::camera::convert_from_window_to_viewport_space(*m_camera, _screen_x, _screen_y);

        const Ogre::Ray mouse_ray = m_camera->getCameraToViewportRay(vp_pos.x, vp_pos.y);

        const Ogre::Vector3 old_pos =
            m_volume_scene_node->convertLocalToWorldPosition(widget_scene_node->getPosition());

        const Ogre::Real distance = mouse_ray.getOrigin().distance(old_pos);

        const Ogre::Vector3 new_pos = m_volume_scene_node->convertWorldToLocalPosition(mouse_ray.getPoint(distance));

        std::array<Ogre::Vector3, 2> tmp_clipping_cube;
        std::copy(m_clipping_cube.begin(), m_clipping_cube.end(), tmp_clipping_cube.begin());

        switch(widget_face)
        {
            case vr::volume_renderer::x_negative:
                tmp_clipping_cube[0].x = new_pos.x;
                break;

            case vr::volume_renderer::x_positive:
                tmp_clipping_cube[1].x = new_pos.x;
                break;

            case vr::volume_renderer::y_negative:
                tmp_clipping_cube[0].y = new_pos.y;
                break;

            case vr::volume_renderer::y_positive:
                tmp_clipping_cube[1].y = new_pos.y;
                break;

            case vr::volume_renderer::z_negative:
                tmp_clipping_cube[0].z = new_pos.z;
                break;

            case vr::volume_renderer::z_positive:
                tmp_clipping_cube[1].z = new_pos.z;
                break;
        }

        // Check for overlap.
        const float eps = 0.001F;
        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            if(tmp_clipping_cube[0][i] > m_clipping_cube[1][i])
            {
                tmp_clipping_cube[0][i] = m_clipping_cube[1][i] - eps;
            }

            if(tmp_clipping_cube[1][i] < m_clipping_cube[0][i])
            {
                tmp_clipping_cube[1][i] = m_clipping_cube[0][i] + eps;
            }
        }

        std::copy(tmp_clipping_cube.begin(), tmp_clipping_cube.end(), m_clipping_cube.begin());

        this->update_widgets();
        this->select_face(widget_face);

        m_selected_widget = dynamic_cast<Ogre::Entity*>(_picked_widget);
        m_selected_widget->setMaterialName(m_id + "_SphereHighlight", RESOURCE_GROUP);
        m_selected_widget->setRenderQueueGroupAndPriority(viz::scene3d::rq::SURFACE_ID, 65535);

        m_clipping_update_callback();
    }
}

//-----------------------------------------------------------------------------

void clipping_box::widget_released()
{
    if(m_selected_widget != nullptr)
    {
        this->deselect_face();
        m_selected_widget->setMaterial(m_handle_mtl);
        m_selected_widget = nullptr;
    }

    if(m_selection_mode == box)
    {
        m_bounding_box->setMaterialName(0, m_box_mtl->getName(), RESOURCE_GROUP);
    }

    m_selection_mode = none;

    m_clipping_update_callback();
}

//-----------------------------------------------------------------------------

bool clipping_box::move_clipping_box(int _x, int _y, int _dx, int _dy)
{
    const auto vp_pos = helper::camera::convert_from_window_to_viewport_space(*m_camera, _x, _y);

    const Ogre::Ray old_pos_ray = m_camera->getCameraToViewportRay(vp_pos.x, vp_pos.y);

    // Get ray in image space.
    const Ogre::Ray mouse_ray_img_space(
        m_volume_scene_node->convertWorldToLocalPosition(old_pos_ray.getOrigin()),
        m_volume_scene_node->convertWorldToLocalDirection(old_pos_ray.getDirection(), true)
    );

    const Ogre::Vector3 min = m_clipping_cube[0];
    const Ogre::Vector3 max = m_clipping_cube[1];

    // Ray clipping box intersection
    const std::pair<bool, float> inter = mouse_ray_img_space.intersects(Ogre::AxisAlignedBox(min, max));

    const Ogre::Vector3 cam_dir = m_camera->getRealDirection();

    Ogre::Vector3 old_pos;
    Ogre::Plane intersection_plane;

    if(m_selection_mode == none)
    {
        if(inter.first && this->get_visibility())
        {
            // Get picked point in box space.
            m_picked_box_point = (mouse_ray_img_space.getPoint(inter.second) - min) / (max - min);
            m_selection_mode   = box;

            m_bounding_box->setMaterialName(0, m_box_highlight_mtl->getName(), RESOURCE_GROUP);
        }
        else
        {
            m_selection_mode = camera;
        }
    }

    const bool box_selected = (m_selection_mode == box);

    if(box_selected)
    {
        // Box to image space.
        const Ogre::Vector3 box_pos = m_picked_box_point * (max - min) + min;

        // Image to world space.
        old_pos            = m_volume_scene_node->convertLocalToWorldPosition(box_pos);
        intersection_plane = Ogre::Plane(cam_dir, old_pos);

        const auto new_vp_pos = helper::camera::convert_from_window_to_viewport_space(*m_camera, _x + _dx, _y + _dy);

        const Ogre::Ray new_pos_ray = m_camera->getCameraToViewportRay(new_vp_pos.x, new_vp_pos.y);

        const std::pair<bool, float> plane_inter = new_pos_ray.intersects(intersection_plane);

        const Ogre::Vector3 new_pos = new_pos_ray.getPoint(plane_inter.second);

        Ogre::Vector3 d = new_pos - old_pos;

        // Translate clipping box in image space.
        d = m_volume_scene_node->convertWorldToLocalDirection(d, true);

        m_clipping_cube[0] += d;
        m_clipping_cube[1] += d;

        this->update_widgets();

        m_clipping_update_callback();
    }

    return box_selected;
}

//-----------------------------------------------------------------------------

bool clipping_box::scale_clipping_box(int _x, int _y, int _dy)
{
    const auto vp_pos = helper::camera::convert_from_window_to_viewport_space(*m_camera, _x, _y);

    const Ogre::Ray old_pos_ray = m_camera->getCameraToViewportRay(vp_pos.x, vp_pos.y);

    // Get ray in image space.
    const Ogre::Ray mouse_ray_img_space(
        m_volume_scene_node->convertWorldToLocalPosition(old_pos_ray.getOrigin()),
        m_volume_scene_node->convertWorldToLocalDirection(old_pos_ray.getDirection(), true)
    );

    const Ogre::Vector3 min = m_clipping_cube[0];
    const Ogre::Vector3 max = m_clipping_cube[1];

    // Ray clipping box intersection
    const std::pair<bool, float> inter = mouse_ray_img_space.intersects(Ogre::AxisAlignedBox(min, max));

    if(m_selection_mode == none)
    {
        if(inter.first && this->get_visibility())
        {
            // Get picked point in box space.
            m_selection_mode = box;

            m_bounding_box->setMaterialName(0, m_box_highlight_mtl->getName(), RESOURCE_GROUP);
        }
        else
        {
            m_selection_mode = camera;
        }
    }

    const bool box_selected = m_selection_mode == box;
    if(box_selected)
    {
        const auto volume_size = m_volume_scene_node->getScale();

        const int height = m_camera->getViewport()->getActualHeight();

        // A displacement of 1 pixel along the height axis scales the box by 1/100th of the image's length.
        const float speed = (volume_size.length() / 100.F) / static_cast<float>(height);
        const float scale = 1.0F + static_cast<float>(_dy) * speed;

        const Ogre::Vector3 cc_center = (m_clipping_cube[1] + m_clipping_cube[0]) / 2.F;

        // Scale clipping cube along its center.
        m_clipping_cube[0] = (m_clipping_cube[0] - cc_center) * scale + cc_center;
        m_clipping_cube[1] = (m_clipping_cube[1] - cc_center) * scale + cc_center;

        this->update_widgets();

        m_clipping_update_callback();
    }

    return box_selected;
}

//-----------------------------------------------------------------------------

bool clipping_box::get_visibility() const
{
    return m_widget_scene_node->getAttachedObject(0)->getVisible();
}

//-----------------------------------------------------------------------------

void clipping_box::set_visibility(bool _visibility)
{
    m_widget_scene_node->setVisible(_visibility);
}

//-----------------------------------------------------------------------------

Ogre::AxisAlignedBox clipping_box::get_clipping_box() const
{
    return {m_clipping_cube[0], m_clipping_cube[1]};
}

//-----------------------------------------------------------------------------

Ogre::Matrix4 clipping_box::get_clipping_transform() const
{
    const auto aa_box = this->get_clipping_box();

    const Ogre::Vector3 initial_center = Ogre::Vector3(0.5F);
    Ogre::Matrix4 inv_center_trans_mx  = Ogre::Matrix4::IDENTITY;
    inv_center_trans_mx.setTrans(-initial_center);

    const Ogre::Vector3 scale = aa_box.getSize();
    Ogre::Matrix4 scale_mx    = Ogre::Matrix4::IDENTITY;
    scale_mx.setScale(scale);

    const Ogre::Vector3 center    = aa_box.getCenter();
    Ogre::Matrix4 center_trans_mx = Ogre::Matrix4::IDENTITY;
    center_trans_mx.setTrans(center);

    const Ogre::Matrix4 clipping_mx = center_trans_mx * scale_mx * inv_center_trans_mx;

    return clipping_mx;
}

//-----------------------------------------------------------------------------

void clipping_box::update_from_transform(const Ogre::Matrix4& _clipping_mx)
{
    this->apply_transform(_clipping_mx);
    m_clipping_update_callback();
}

//-----------------------------------------------------------------------------

void clipping_box::apply_transform(const Ogre::Matrix4& _clipping_mx)
{
    // The clipping matrix is the transform from the initial box position to the new one.
    // Convert to world position because that's how VTK stores its crop matrices...
    const Ogre::Vector3 world_cube_min = Ogre::Vector3::ZERO;
    const Ogre::Vector3 world_cube_max = Ogre::Vector3::UNIT_SCALE;

    m_clipping_cube[0] = _clipping_mx * world_cube_min;
    m_clipping_cube[1] = _clipping_mx * world_cube_max;

    this->update_widgets();
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::widget
