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
    M_ID(std::move(_id)),
    m_sceneManager(_scene_manager),
    m_camera(_camera),
    m_volumeSceneNode(_parent_scene_node),
    m_widgetSceneNode(m_volumeSceneNode->createChildSceneNode()),
    m_handleMtl(Ogre::MaterialManager::getSingleton().getByName(_handle_mtl_name, RESOURCE_GROUP)),
    m_boxMtl(Ogre::MaterialManager::getSingleton().getByName(_box_mtl_name, RESOURCE_GROUP)),
    m_clippingUpdateCallback(std::move(_clipping_update_callback))
{
    SIGHT_ASSERT("Missing '" + _box_mtl_name + "' material.", m_boxMtl);
    SIGHT_ASSERT("Missing '" + _handle_mtl_name + "' material.", m_handleMtl);

    this->initWidgets();
    this->applyTransform(_clipping_matrix);
}

//-----------------------------------------------------------------------------

clipping_box::~clipping_box()
{
    m_widgetSceneNode->detachAllObjects();

    m_sceneManager->destroyManualObject(m_selectedFace);
    m_sceneManager->destroyManualObject(m_boundingBox);

    for(auto& widget : m_widgets)
    {
        const Ogre::MovableObject* mo = widget.first;
        m_sceneManager->destroyMovableObject(mo->getName(), mo->getMovableType());
    }

    Ogre::MaterialManager::getSingleton().remove(m_handleHightlightMtl->getHandle());
    Ogre::MaterialManager::getSingleton().remove(m_boxHighlightMtl->getHandle());
    Ogre::MaterialManager::getSingleton().remove(m_boxFaceMtl->getHandle());

    m_sceneManager->destroySceneNode(m_widgetSceneNode);
}

//-----------------------------------------------------------------------------

bool clipping_box::belongsToWidget(const Ogre::MovableObject* const _object) const
{
    return m_widgets.find(_object) != m_widgets.end();
}

//-----------------------------------------------------------------------------

std::array<Ogre::Vector3,
           4> clipping_box::getFacePositions(viz::scene3d::vr::volume_renderer::CubeFace _face_name) const
{
    const vr::volume_renderer::CubeFacePositionList position_indices =
        vr::volume_renderer::s_cubeFaces.at(_face_name);
    std::array<Ogre::Vector3, 4> face_positions;

    const auto bb_positions = this->clipping_boxPositions();

    std::transform(
        position_indices.begin(),
        position_indices.end(),
        face_positions.begin(),
        [&](unsigned _i){return bb_positions[_i];});

    return face_positions;
}

//-----------------------------------------------------------------------------

Ogre::Vector3 clipping_box::getFaceCenter(viz::scene3d::vr::volume_renderer::CubeFace _face_name) const
{
    const auto face_positions = this->getFacePositions(_face_name);
    return std::accumulate(face_positions.cbegin() + 1, face_positions.cend(), face_positions[0]) / 4.F;
}

//-----------------------------------------------------------------------------

std::array<Ogre::Vector3, 8> clipping_box::clipping_boxPositions() const
{
    return {{
        Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[1].y, m_clippingCube[1].z),
        Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[0].y, m_clippingCube[1].z),
        Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[1].y, m_clippingCube[0].z),
        Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[1].y, m_clippingCube[1].z),
        Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[0].y, m_clippingCube[1].z),
        Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[0].y, m_clippingCube[0].z),
        Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[1].y, m_clippingCube[0].z),
        Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[0].y, m_clippingCube[0].z)
    }
    };
}

//-----------------------------------------------------------------------------

void clipping_box::updateWidgets()
{
    const auto clipping_box_positions = this->clipping_boxPositions();

    m_boundingBox->beginUpdate(0);
    {
        // Box
        for(unsigned i = 0 ; i < 12 ; ++i)
        {
            const auto cube_edge = vr::volume_renderer::s_cubeEdges[i];

            m_boundingBox->position(clipping_box_positions[cube_edge.first]);
            m_boundingBox->position(clipping_box_positions[cube_edge.second]);
        }

        // Cross
        for(unsigned i = 0 ; i < 6 ; i += 2)
        {
            const auto face_center          = this->getFaceCenter(static_cast<vr::volume_renderer::CubeFace>(i));
            const auto opposite_face_center = this->getFaceCenter(static_cast<vr::volume_renderer::CubeFace>(i + 1));

            m_boundingBox->position(face_center);
            m_boundingBox->position(opposite_face_center);
        }
    }
    m_boundingBox->end();

    // Recenter widgets
    for(auto& widget : m_widgets)
    {
        const auto& widget_face_node_pair = widget.second;
        const Ogre::Vector3 face_center   = this->getFaceCenter(widget_face_node_pair.first);

        auto* widget_scene_node = widget_face_node_pair.second;
        widget_scene_node->setPosition(face_center);
        widget_scene_node->needUpdate();
    }
}

//-----------------------------------------------------------------------------

void clipping_box::initWidgets()
{
    // Create widget materials
    {
        m_handleMtl->setAmbient(0.1F, 0.1F, 0.1F);
        m_handleMtl->setDiffuse(1.F, 1.F, 1.F, 1.F);
        m_handleMtl->setSpecular(0.F, 0.F, 0.F, 1.F);
        m_handleMtl->setShininess(1.F);

        m_handleHightlightMtl = m_handleMtl->clone(M_ID + "_SphereHighlight", true, RESOURCE_GROUP);
        m_handleHightlightMtl->setAmbient(0.3F, 0.F, 0.F);
        m_handleHightlightMtl->setDiffuse(0.5F, 0.1F, 0.1F, 1.F);

        m_boxMtl->setAmbient(1.F, 1.F, 1.F);
        m_boxMtl->setDiffuse(0.F, 0.F, 0.F, 1.F);
        m_boxMtl->setSpecular(0.F, 0.F, 0.F, 1.F);

        m_boxHighlightMtl = m_boxMtl->clone(M_ID + "_FrameHighlight", true, RESOURCE_GROUP);
        m_boxHighlightMtl->setAmbient(0.F, 1.F, 0.F);

        m_boxFaceMtl = m_boxMtl->clone(M_ID + "_FaceHighlight", true, RESOURCE_GROUP);
        m_boxFaceMtl->setAmbient(1.F, 1.F, 0.F);
        m_boxFaceMtl->setDiffuse(0.F, 0.F, 0.F, 0.6F);
        m_boxFaceMtl->setSpecular(0.F, 0.F, 0.F, 0.6F);
        m_boxFaceMtl->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        m_boxFaceMtl->setDepthWriteEnabled(false);
    }

    m_boundingBox  = m_sceneManager->createManualObject(M_ID + "_VolumeBB");
    m_selectedFace = m_sceneManager->createManualObject(M_ID + "_VRSelectedFace");

    const auto clipping_box_positions = this->clipping_boxPositions();

    m_boundingBox->begin(m_boxMtl->getName(), Ogre::RenderOperation::OT_LINE_LIST, RESOURCE_GROUP);
    {
        for(unsigned i = 0 ; i < 12 ; ++i)
        {
            const auto cube_edge = vr::volume_renderer::s_cubeEdges[i];

            m_boundingBox->position(clipping_box_positions[cube_edge.first]);
            m_boundingBox->position(clipping_box_positions[cube_edge.second]);
        }

        // Cross
        for(unsigned i = 0 ; i < 6 ; i += 2)
        {
            const auto face_center          = this->getFaceCenter(static_cast<vr::volume_renderer::CubeFace>(i));
            const auto opposite_face_center = this->getFaceCenter(static_cast<vr::volume_renderer::CubeFace>(i + 1));

            m_boundingBox->position(face_center);
            m_boundingBox->position(opposite_face_center);
        }
    }
    m_boundingBox->end();

    m_widgetSceneNode->attachObject(m_boundingBox);

    m_selectedFace->begin(m_boxFaceMtl->getName(), Ogre::RenderOperation::OT_TRIANGLE_STRIP, RESOURCE_GROUP);
    {
        for(unsigned i = 0 ; i < 4 ; ++i)
        {
            m_selectedFace->position(0, 0, 0);
        }
    }
    m_selectedFace->end();

    // Render highlighted faces after other surfaces but before volumes.
    m_selectedFace->setRenderQueueGroup(viz::scene3d::rq::s_SURFACE_ID + 1);

    // Create a pickable sphere for each cube face
    for(unsigned i = 0 ; i < 6 ; ++i)
    {
        const auto current_face = static_cast<vr::volume_renderer::CubeFace>(i);

        Ogre::Entity* new_widget = m_sceneManager->createEntity(Ogre::SceneManager::PT_SPHERE);
        new_widget->setMaterial(m_handleMtl);

        Ogre::SceneNode* sphere_scene_node = m_widgetSceneNode->createChildSceneNode();

        m_widgets[new_widget] = std::make_pair(current_face, sphere_scene_node);

        const Ogre::Vector3 face_center = this->getFaceCenter(current_face);

        sphere_scene_node->setPosition(face_center);
        sphere_scene_node->setInheritScale(false);

        const auto vol_scale       = m_volumeSceneNode->getScale();
        const Ogre::Real scale_min = std::min(vol_scale[0], std::min(vol_scale[1], vol_scale[2]));

        // Scale the handle to be 1/100th of the volume's initial size.
        const Ogre::Vector3 widget_scale((0.02F * scale_min) / new_widget->getBoundingRadius());
        sphere_scene_node->setScale(widget_scale);

        sphere_scene_node->attachObject(new_widget);
    }
}

//-----------------------------------------------------------------------------

void clipping_box::selectFace(viz::scene3d::vr::volume_renderer::CubeFace _face_name)
{
    m_selectedFace->beginUpdate(0);
    {
        const std::array<Ogre::Vector3, 4> face_positions = this->getFacePositions(_face_name);
        m_selectedFace->position(face_positions[1]);
        m_selectedFace->position(face_positions[0]);
        m_selectedFace->position(face_positions[2]);
        m_selectedFace->position(face_positions[3]);
    }
    m_selectedFace->end();

    m_widgetSceneNode->attachObject(m_selectedFace);
}

//-----------------------------------------------------------------------------

void clipping_box::deselectFace()
{
    if(m_selectedFace->isAttached())
    {
        m_widgetSceneNode->detachObject(m_selectedFace->getName());
    }
}

//-----------------------------------------------------------------------------

void clipping_box::widgetPicked(Ogre::MovableObject* _picked_widget, int _screen_x, int _screen_y)
{
    SIGHT_ASSERT("The picked widget does not belong to this widget.", this->belongsToWidget(_picked_widget));
    const auto face = m_widgets.at(_picked_widget);

    this->deselectFace();

    if(this->getVisibility())
    {
        const viz::scene3d::vr::volume_renderer::CubeFace widget_face = face.first;
        const Ogre::SceneNode* widget_scene_node                      = face.second;

        const auto vp_pos = helper::camera::convertFromWindowToViewportSpace(*m_camera, _screen_x, _screen_y);

        const Ogre::Ray mouse_ray = m_camera->getCameraToViewportRay(vp_pos.x, vp_pos.y);

        const Ogre::Vector3 old_pos = m_volumeSceneNode->convertLocalToWorldPosition(widget_scene_node->getPosition());

        const Ogre::Real distance = mouse_ray.getOrigin().distance(old_pos);

        const Ogre::Vector3 new_pos = m_volumeSceneNode->convertWorldToLocalPosition(mouse_ray.getPoint(distance));

        std::array<Ogre::Vector3, 2> tmp_clipping_cube;
        std::copy(m_clippingCube.begin(), m_clippingCube.end(), tmp_clipping_cube.begin());

        switch(widget_face)
        {
            case vr::volume_renderer::X_NEGATIVE:
                tmp_clipping_cube[0].x = new_pos.x;
                break;

            case vr::volume_renderer::X_POSITIVE:
                tmp_clipping_cube[1].x = new_pos.x;
                break;

            case vr::volume_renderer::Y_NEGATIVE:
                tmp_clipping_cube[0].y = new_pos.y;
                break;

            case vr::volume_renderer::Y_POSITIVE:
                tmp_clipping_cube[1].y = new_pos.y;
                break;

            case vr::volume_renderer::Z_NEGATIVE:
                tmp_clipping_cube[0].z = new_pos.z;
                break;

            case vr::volume_renderer::Z_POSITIVE:
                tmp_clipping_cube[1].z = new_pos.z;
                break;
        }

        // Check for overlap.
        const float eps = 0.001F;
        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            if(tmp_clipping_cube[0][i] > m_clippingCube[1][i])
            {
                tmp_clipping_cube[0][i] = m_clippingCube[1][i] - eps;
            }

            if(tmp_clipping_cube[1][i] < m_clippingCube[0][i])
            {
                tmp_clipping_cube[1][i] = m_clippingCube[0][i] + eps;
            }
        }

        std::copy(tmp_clipping_cube.begin(), tmp_clipping_cube.end(), m_clippingCube.begin());

        this->updateWidgets();
        this->selectFace(widget_face);

        m_selectedWidget = dynamic_cast<Ogre::Entity*>(_picked_widget);
        m_selectedWidget->setMaterialName(M_ID + "_SphereHighlight", RESOURCE_GROUP);
        m_selectedWidget->setRenderQueueGroupAndPriority(viz::scene3d::rq::s_SURFACE_ID, 65535);

        m_clippingUpdateCallback();
    }
}

//-----------------------------------------------------------------------------

void clipping_box::widgetReleased()
{
    if(m_selectedWidget != nullptr)
    {
        this->deselectFace();
        m_selectedWidget->setMaterial(m_handleMtl);
        m_selectedWidget = nullptr;
    }

    if(m_selectionMode == BOX)
    {
        m_boundingBox->setMaterialName(0, m_boxMtl->getName(), RESOURCE_GROUP);
    }

    m_selectionMode = NONE;

    m_clippingUpdateCallback();
}

//-----------------------------------------------------------------------------

bool clipping_box::move_clipping_box(int _x, int _y, int _dx, int _dy)
{
    const auto vp_pos = helper::camera::convertFromWindowToViewportSpace(*m_camera, _x, _y);

    const Ogre::Ray old_pos_ray = m_camera->getCameraToViewportRay(vp_pos.x, vp_pos.y);

    // Get ray in image space.
    const Ogre::Ray mouse_ray_img_space(
        m_volumeSceneNode->convertWorldToLocalPosition(old_pos_ray.getOrigin()),
        m_volumeSceneNode->convertWorldToLocalDirection(old_pos_ray.getDirection(), true)
    );

    const Ogre::Vector3 min = m_clippingCube[0];
    const Ogre::Vector3 max = m_clippingCube[1];

    // Ray clipping box intersection
    const std::pair<bool, float> inter = mouse_ray_img_space.intersects(Ogre::AxisAlignedBox(min, max));

    const Ogre::Vector3 cam_dir = m_camera->getRealDirection();

    Ogre::Vector3 old_pos;
    Ogre::Plane intersection_plane;

    if(m_selectionMode == NONE)
    {
        if(inter.first && this->getVisibility())
        {
            // Get picked point in box space.
            m_pickedBoxPoint = (mouse_ray_img_space.getPoint(inter.second) - min) / (max - min);
            m_selectionMode  = BOX;

            m_boundingBox->setMaterialName(0, m_boxHighlightMtl->getName(), RESOURCE_GROUP);
        }
        else
        {
            m_selectionMode = CAMERA;
        }
    }

    const bool box_selected = (m_selectionMode == BOX);

    if(box_selected)
    {
        // Box to image space.
        const Ogre::Vector3 box_pos = m_pickedBoxPoint * (max - min) + min;

        // Image to world space.
        old_pos            = m_volumeSceneNode->convertLocalToWorldPosition(box_pos);
        intersection_plane = Ogre::Plane(cam_dir, old_pos);

        const auto new_vp_pos = helper::camera::convertFromWindowToViewportSpace(*m_camera, _x + _dx, _y + _dy);

        const Ogre::Ray new_pos_ray = m_camera->getCameraToViewportRay(new_vp_pos.x, new_vp_pos.y);

        const std::pair<bool, float> plane_inter = new_pos_ray.intersects(intersection_plane);

        const Ogre::Vector3 new_pos = new_pos_ray.getPoint(plane_inter.second);

        Ogre::Vector3 d = new_pos - old_pos;

        // Translate clipping box in image space.
        d = m_volumeSceneNode->convertWorldToLocalDirection(d, true);

        m_clippingCube[0] += d;
        m_clippingCube[1] += d;

        this->updateWidgets();

        m_clippingUpdateCallback();
    }

    return box_selected;
}

//-----------------------------------------------------------------------------

bool clipping_box::scale_clipping_box(int _x, int _y, int _dy)
{
    const auto vp_pos = helper::camera::convertFromWindowToViewportSpace(*m_camera, _x, _y);

    const Ogre::Ray old_pos_ray = m_camera->getCameraToViewportRay(vp_pos.x, vp_pos.y);

    // Get ray in image space.
    const Ogre::Ray mouse_ray_img_space(
        m_volumeSceneNode->convertWorldToLocalPosition(old_pos_ray.getOrigin()),
        m_volumeSceneNode->convertWorldToLocalDirection(old_pos_ray.getDirection(), true)
    );

    const Ogre::Vector3 min = m_clippingCube[0];
    const Ogre::Vector3 max = m_clippingCube[1];

    // Ray clipping box intersection
    const std::pair<bool, float> inter = mouse_ray_img_space.intersects(Ogre::AxisAlignedBox(min, max));

    if(m_selectionMode == NONE)
    {
        if(inter.first && this->getVisibility())
        {
            // Get picked point in box space.
            m_selectionMode = BOX;

            m_boundingBox->setMaterialName(0, m_boxHighlightMtl->getName(), RESOURCE_GROUP);
        }
        else
        {
            m_selectionMode = CAMERA;
        }
    }

    const bool box_selected = m_selectionMode == BOX;
    if(box_selected)
    {
        const auto volume_size = m_volumeSceneNode->getScale();

        const int height = m_camera->getViewport()->getActualHeight();

        // A displacement of 1 pixel along the height axis scales the box by 1/100th of the image's length.
        const float speed = (volume_size.length() / 100.F) / static_cast<float>(height);
        const float scale = 1.0F + static_cast<float>(_dy) * speed;

        const Ogre::Vector3 cc_center = (m_clippingCube[1] + m_clippingCube[0]) / 2.F;

        // Scale clipping cube along its center.
        m_clippingCube[0] = (m_clippingCube[0] - cc_center) * scale + cc_center;
        m_clippingCube[1] = (m_clippingCube[1] - cc_center) * scale + cc_center;

        this->updateWidgets();

        m_clippingUpdateCallback();
    }

    return box_selected;
}

//-----------------------------------------------------------------------------

bool clipping_box::getVisibility() const
{
    return m_widgetSceneNode->getAttachedObject(0)->getVisible();
}

//-----------------------------------------------------------------------------

void clipping_box::setVisibility(bool _visibility)
{
    m_widgetSceneNode->setVisible(_visibility);
}

//-----------------------------------------------------------------------------

Ogre::AxisAlignedBox clipping_box::get_clipping_box() const
{
    return {m_clippingCube[0], m_clippingCube[1]};
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

void clipping_box::updateFromTransform(const Ogre::Matrix4& _clipping_mx)
{
    this->applyTransform(_clipping_mx);
    m_clippingUpdateCallback();
}

//-----------------------------------------------------------------------------

void clipping_box::applyTransform(const Ogre::Matrix4& _clipping_mx)
{
    // The clipping matrix is the transform from the initial box position to the new one.
    // Convert to world position because that's how VTK stores its crop matrices...
    const Ogre::Vector3 world_cube_min = Ogre::Vector3::ZERO;
    const Ogre::Vector3 world_cube_max = Ogre::Vector3::UNIT_SCALE;

    m_clippingCube[0] = _clipping_mx * world_cube_min;
    m_clippingCube[1] = _clipping_mx * world_cube_max;

    this->updateWidgets();
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::widget
