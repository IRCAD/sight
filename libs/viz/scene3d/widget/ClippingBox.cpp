/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

#include "viz/scene3d/widget/ClippingBox.hpp"

#include "viz/scene3d/compositor/Core.hpp"
#include "viz/scene3d/helper/Camera.hpp"

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

namespace sight::viz::scene3d::widget
{

//-----------------------------------------------------------------------------

ClippingBox::ClippingBox(
    const std::string& id,
    ::Ogre::SceneNode* parentSceneNode,
    ::Ogre::Camera* camera,
    ::Ogre::SceneManager* sceneManager,
    const ::Ogre::Matrix4& clippingMatrix,
    const ClippingUpdateCallbackType& clippingUpdateCallback,
    const std::string& boxMtlName,
    const std::string& handleMtlName
) :
    m_id(id),
    m_sceneManager(sceneManager),
    m_camera(camera),
    m_volumeSceneNode(parentSceneNode),
    m_widgetSceneNode(m_volumeSceneNode->createChildSceneNode()),
    m_clippingUpdateCallback(clippingUpdateCallback)
{
    m_boxMtl    = ::Ogre::MaterialManager::getSingleton().getByName(boxMtlName, RESOURCE_GROUP);
    m_handleMtl = ::Ogre::MaterialManager::getSingleton().getByName(handleMtlName, RESOURCE_GROUP);
    SIGHT_ASSERT("Missing '" + boxMtlName + "' material.", m_boxMtl);
    SIGHT_ASSERT("Missing '" + handleMtlName + "' material.", m_handleMtl);

    this->initWidgets();
    this->applyTransform(clippingMatrix);
}

//-----------------------------------------------------------------------------

ClippingBox::~ClippingBox()
{
    m_widgetSceneNode->detachAllObjects();

    m_sceneManager->destroyManualObject(m_selectedFace);
    m_sceneManager->destroyManualObject(m_boundingBox);

    for(auto& widget : m_widgets)
    {
        const ::Ogre::MovableObject* mo = widget.first;
        m_sceneManager->destroyMovableObject(mo->getName(), mo->getMovableType());
    }

    ::Ogre::MaterialManager::getSingleton().remove(m_handleHightlightMtl->getHandle());
    ::Ogre::MaterialManager::getSingleton().remove(m_boxHighlightMtl->getHandle());
    ::Ogre::MaterialManager::getSingleton().remove(m_boxFaceMtl->getHandle());

    m_sceneManager->destroySceneNode(m_widgetSceneNode);
}

//-----------------------------------------------------------------------------

bool ClippingBox::belongsToWidget(const Ogre::MovableObject* const _object) const
{
    return m_widgets.find(_object) != m_widgets.end();
}

//-----------------------------------------------------------------------------

std::array< ::Ogre::Vector3,
            4> ClippingBox::getFacePositions(viz::scene3d::vr::IVolumeRenderer::CubeFace _faceName) const
{
    const vr::IVolumeRenderer::CubeFacePositionList positionIndices =
        vr::IVolumeRenderer::s_cubeFaces.at(_faceName);
    std::array< ::Ogre::Vector3, 4> facePositions;

    const auto BBpositions = this->getClippingBoxPositions();

    std::transform(
        positionIndices.begin(),
        positionIndices.end(),
        facePositions.begin(),
        [&](unsigned i){return BBpositions[i];});

    return facePositions;
}

//-----------------------------------------------------------------------------

Ogre::Vector3 ClippingBox::getFaceCenter(viz::scene3d::vr::IVolumeRenderer::CubeFace _faceName) const
{
    const auto facePositions = this->getFacePositions(_faceName);
    return std::accumulate(facePositions.cbegin() + 1, facePositions.cend(), facePositions[0]) / 4.f;
}

//-----------------------------------------------------------------------------

std::array< ::Ogre::Vector3, 8> ClippingBox::getClippingBoxPositions() const
{
    return {{
        ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[1].y, m_clippingCube[1].z),
        ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[0].y, m_clippingCube[1].z),
        ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[1].y, m_clippingCube[0].z),
        ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[1].y, m_clippingCube[1].z),
        ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[0].y, m_clippingCube[1].z),
        ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[0].y, m_clippingCube[0].z),
        ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[1].y, m_clippingCube[0].z),
        ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[0].y, m_clippingCube[0].z)
    }
    };
}

//-----------------------------------------------------------------------------

void ClippingBox::updateWidgets()
{
    const auto clippingBoxPositions = this->getClippingBoxPositions();

    m_boundingBox->beginUpdate(0);
    {
        // Box
        for(unsigned i = 0 ; i < 12 ; ++i)
        {
            const auto cubeEdge = vr::IVolumeRenderer::s_cubeEdges[i];

            m_boundingBox->position(clippingBoxPositions[cubeEdge.first]);
            m_boundingBox->position(clippingBoxPositions[cubeEdge.second]);
        }

        // Cross
        for(unsigned i = 0 ; i < 6 ; i += 2)
        {
            const auto faceCenter         = this->getFaceCenter(static_cast<vr::IVolumeRenderer::CubeFace>(i));
            const auto oppositeFaceCenter = this->getFaceCenter(static_cast<vr::IVolumeRenderer::CubeFace>(i + 1));

            m_boundingBox->position(faceCenter);
            m_boundingBox->position(oppositeFaceCenter);
        }
    }
    m_boundingBox->end();

    // Recenter widgets
    for(auto& widget : m_widgets)
    {
        const auto& widgetFaceNodePair   = widget.second;
        const ::Ogre::Vector3 faceCenter = this->getFaceCenter(widgetFaceNodePair.first);

        auto* widgetSceneNode = widgetFaceNodePair.second;
        widgetSceneNode->setPosition(faceCenter);
        widgetSceneNode->needUpdate();
    }
}

//-----------------------------------------------------------------------------

void ClippingBox::initWidgets()
{
    // Create widget materials
    {
        m_handleMtl->setAmbient(0.1f, 0.1f, 0.1f);
        m_handleMtl->setDiffuse(1.f, 1.f, 1.f, 1.f);
        m_handleMtl->setSpecular(0.f, 0.f, 0.f, 1.f);
        m_handleMtl->setShininess(1.f);

        m_handleHightlightMtl = m_handleMtl->clone(m_id + "_SphereHighlight", true, RESOURCE_GROUP);
        m_handleHightlightMtl->setAmbient(0.3f, 0.f, 0.f);
        m_handleHightlightMtl->setDiffuse(0.5f, 0.1f, 0.1f, 1.f);

        m_boxMtl->setAmbient(1.f, 1.f, 1.f);
        m_boxMtl->setDiffuse(0.f, 0.f, 0.f, 1.f);
        m_boxMtl->setSpecular(0.f, 0.f, 0.f, 1.f);

        m_boxHighlightMtl = m_boxMtl->clone(m_id + "_FrameHighlight", true, RESOURCE_GROUP);
        m_boxHighlightMtl->setAmbient(0.f, 1.f, 0.f);

        m_boxFaceMtl = m_boxMtl->clone(m_id + "_FaceHighlight", true, RESOURCE_GROUP);
        m_boxFaceMtl->setAmbient(1.f, 1.f, 0.f);
        m_boxFaceMtl->setDiffuse(0.f, 0.f, 0.f, 0.6f);
        m_boxFaceMtl->setSpecular(0.f, 0.f, 0.f, 0.6f);
        m_boxFaceMtl->setSceneBlending(::Ogre::SBT_TRANSPARENT_ALPHA);
        m_boxFaceMtl->setDepthWriteEnabled(false);
    }

    m_boundingBox  = m_sceneManager->createManualObject(m_id + "_VolumeBB");
    m_selectedFace = m_sceneManager->createManualObject(m_id + "_VRSelectedFace");

    const auto clippingBoxPositions = this->getClippingBoxPositions();

    m_boundingBox->begin(m_boxMtl->getName(), Ogre::RenderOperation::OT_LINE_LIST, RESOURCE_GROUP);
    {
        for(unsigned i = 0 ; i < 12 ; ++i)
        {
            const auto cubeEdge = vr::IVolumeRenderer::s_cubeEdges[i];

            m_boundingBox->position(clippingBoxPositions[cubeEdge.first]);
            m_boundingBox->position(clippingBoxPositions[cubeEdge.second]);
        }

        // Cross
        for(unsigned i = 0 ; i < 6 ; i += 2)
        {
            const auto faceCenter         = this->getFaceCenter(static_cast<vr::IVolumeRenderer::CubeFace>(i));
            const auto oppositeFaceCenter = this->getFaceCenter(static_cast<vr::IVolumeRenderer::CubeFace>(i + 1));

            m_boundingBox->position(faceCenter);
            m_boundingBox->position(oppositeFaceCenter);
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
    m_selectedFace->setRenderQueueGroup(compositor::Core::s_SURFACE_RQ_GROUP_ID + 1);

    // Create a pickable sphere for each cube face
    for(unsigned i = 0 ; i < 6 ; ++i)
    {
        const auto currentFace = static_cast<vr::IVolumeRenderer::CubeFace>(i);

        ::Ogre::Entity* newWidget = m_sceneManager->createEntity(::Ogre::SceneManager::PT_SPHERE);
        newWidget->setMaterial(m_handleMtl);

        ::Ogre::SceneNode* sphereSceneNode = m_widgetSceneNode->createChildSceneNode();

        m_widgets[newWidget] = std::make_pair(currentFace, sphereSceneNode);

        const ::Ogre::Vector3 faceCenter = this->getFaceCenter(currentFace);

        sphereSceneNode->setPosition(faceCenter);
        sphereSceneNode->setInheritScale(false);

        const auto volScale         = m_volumeSceneNode->getScale();
        const ::Ogre::Real scaleMin = std::min(volScale[0], std::min(volScale[1], volScale[2]));

        // Scale the handle to be 1/100th of the volume's initial size.
        const ::Ogre::Vector3 widgetScale((0.01f * scaleMin) / newWidget->getBoundingRadius());
        sphereSceneNode->setScale(widgetScale);

        sphereSceneNode->attachObject(newWidget);
    }
}

//-----------------------------------------------------------------------------

void ClippingBox::selectFace(viz::scene3d::vr::IVolumeRenderer::CubeFace _faceName)
{
    m_selectedFace->beginUpdate(0);
    {
        const std::array< ::Ogre::Vector3, 4> facePositions = this->getFacePositions(_faceName);
        m_selectedFace->position(facePositions[1]);
        m_selectedFace->position(facePositions[0]);
        m_selectedFace->position(facePositions[2]);
        m_selectedFace->position(facePositions[3]);
    }
    m_selectedFace->end();

    m_widgetSceneNode->attachObject(m_selectedFace);
}

//-----------------------------------------------------------------------------

void ClippingBox::deselectFace()
{
    if(m_selectedFace->isAttached())
    {
        m_widgetSceneNode->detachObject(m_selectedFace->getName());
    }
}

//-----------------------------------------------------------------------------

void ClippingBox::widgetPicked(::Ogre::MovableObject* _pickedWidget, int _screenX, int _screenY)
{
    SIGHT_ASSERT("The picked widget does not belong to this widget.", this->belongsToWidget(_pickedWidget));
    const auto face = m_widgets.at(_pickedWidget);

    this->deselectFace();

    if(this->getVisibility())
    {
        const viz::scene3d::vr::IVolumeRenderer::CubeFace widgetFace = face.first;
        const ::Ogre::SceneNode* widgetSceneNode                     = face.second;

        const auto vpPos = helper::Camera::convertFromWindowToViewportSpace(*m_camera, _screenX, _screenY);

        const ::Ogre::Ray mouseRay = m_camera->getCameraToViewportRay(vpPos.x, vpPos.y);

        const ::Ogre::Vector3 oldPos = m_volumeSceneNode->convertLocalToWorldPosition(widgetSceneNode->getPosition());

        const ::Ogre::Real distance = mouseRay.getOrigin().distance(oldPos);

        const ::Ogre::Vector3 newPos = m_volumeSceneNode->convertWorldToLocalPosition(mouseRay.getPoint(distance));

        std::array< ::Ogre::Vector3, 2> tmpClippingCube;
        std::copy(m_clippingCube.begin(), m_clippingCube.end(), tmpClippingCube.begin());

        switch(widgetFace)
        {
            case vr::IVolumeRenderer::X_NEGATIVE:
                tmpClippingCube[0].x = newPos.x;
                break;

            case vr::IVolumeRenderer::X_POSITIVE:
                tmpClippingCube[1].x = newPos.x;
                break;

            case vr::IVolumeRenderer::Y_NEGATIVE:
                tmpClippingCube[0].y = newPos.y;
                break;

            case vr::IVolumeRenderer::Y_POSITIVE:
                tmpClippingCube[1].y = newPos.y;
                break;

            case vr::IVolumeRenderer::Z_NEGATIVE:
                tmpClippingCube[0].z = newPos.z;
                break;

            case vr::IVolumeRenderer::Z_POSITIVE:
                tmpClippingCube[1].z = newPos.z;
                break;
        }

        // Check for overlap.
        const float eps = 0.001f;
        for(size_t i = 0 ; i < 3 ; ++i)
        {
            if(tmpClippingCube[0][i] > m_clippingCube[1][i])
            {
                tmpClippingCube[0][i] = m_clippingCube[1][i] - eps;
            }

            if(tmpClippingCube[1][i] < m_clippingCube[0][i])
            {
                tmpClippingCube[1][i] = m_clippingCube[0][i] + eps;
            }
        }

        std::copy(tmpClippingCube.begin(), tmpClippingCube.end(), m_clippingCube.begin());

        this->updateWidgets();
        this->selectFace(widgetFace);

        m_selectedWidget = dynamic_cast< ::Ogre::Entity*>(_pickedWidget);
        m_selectedWidget->setMaterialName(m_id + "_SphereHighlight", RESOURCE_GROUP);
        m_selectedWidget->setRenderQueueGroupAndPriority(compositor::Core::s_SURFACE_RQ_GROUP_ID, 65535);

        m_clippingUpdateCallback();
    }
}

//-----------------------------------------------------------------------------

void ClippingBox::widgetReleased()
{
    if(m_selectedWidget)
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

bool ClippingBox::moveClippingBox(int x, int y, int dx, int dy)
{
    const auto vpPos = helper::Camera::convertFromWindowToViewportSpace(*m_camera, x, y);

    const ::Ogre::Ray oldPosRay = m_camera->getCameraToViewportRay(vpPos.x, vpPos.y);

    // Get ray in image space.
    const ::Ogre::Ray mouseRayImgSpace(
        m_volumeSceneNode->convertWorldToLocalPosition(oldPosRay.getOrigin()),
        m_volumeSceneNode->convertWorldToLocalDirection(oldPosRay.getDirection(), true)
    );

    const ::Ogre::Vector3 min = m_clippingCube[0];
    const ::Ogre::Vector3 max = m_clippingCube[1];

    // Ray clipping box intersection
    const std::pair<bool, float> inter = mouseRayImgSpace.intersects(::Ogre::AxisAlignedBox(min, max));

    const ::Ogre::Vector3 camDir = m_camera->getRealDirection();

    ::Ogre::Vector3 oldPos;
    ::Ogre::Plane intersectionPlane;

    if(m_selectionMode == NONE)
    {
        if(inter.first && this->getVisibility())
        {
            // Get picked point in box space.
            m_pickedBoxPoint = (mouseRayImgSpace.getPoint(inter.second) - min) / (max - min);
            m_selectionMode  = BOX;

            m_boundingBox->setMaterialName(0, m_boxHighlightMtl->getName(), RESOURCE_GROUP);
        }
        else
        {
            m_selectionMode = CAMERA;
        }
    }

    const bool boxSelected = (m_selectionMode == BOX);

    if(boxSelected)
    {
        // Box to image space.
        const ::Ogre::Vector3 boxPos = m_pickedBoxPoint * (max - min) + min;

        // Image to world space.
        oldPos            = m_volumeSceneNode->convertLocalToWorldPosition(boxPos);
        intersectionPlane = ::Ogre::Plane(camDir, oldPos);

        const auto newVpPos = helper::Camera::convertFromWindowToViewportSpace(*m_camera, x + dx, y + dy);

        const ::Ogre::Ray newPosRay = m_camera->getCameraToViewportRay(newVpPos.x, newVpPos.y);

        const std::pair<bool, float> planeInter = newPosRay.intersects(intersectionPlane);

        const ::Ogre::Vector3 newPos = newPosRay.getPoint(planeInter.second);

        ::Ogre::Vector3 d = newPos - oldPos;

        // Translate clipping box in image space.
        d = m_volumeSceneNode->convertWorldToLocalDirection(d, true);

        m_clippingCube[0] += d;
        m_clippingCube[1] += d;

        this->updateWidgets();

        m_clippingUpdateCallback();
    }

    return boxSelected;
}

//-----------------------------------------------------------------------------

bool ClippingBox::scaleClippingBox(int x, int y, int dy)
{
    const auto vpPos = helper::Camera::convertFromWindowToViewportSpace(*m_camera, x, y);

    const ::Ogre::Ray oldPosRay = m_camera->getCameraToViewportRay(vpPos.x, vpPos.y);

    // Get ray in image space.
    const ::Ogre::Ray mouseRayImgSpace(
        m_volumeSceneNode->convertWorldToLocalPosition(oldPosRay.getOrigin()),
        m_volumeSceneNode->convertWorldToLocalDirection(oldPosRay.getDirection(), true)
    );

    const ::Ogre::Vector3 min = m_clippingCube[0];
    const ::Ogre::Vector3 max = m_clippingCube[1];

    // Ray clipping box intersection
    const std::pair<bool, float> inter = mouseRayImgSpace.intersects(::Ogre::AxisAlignedBox(min, max));

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

    const bool boxSelected = m_selectionMode == BOX;
    if(boxSelected)
    {
        const auto volumeSize = m_volumeSceneNode->getScale();

        const int height = m_camera->getViewport()->getActualHeight();

        // A displacement of 1 pixel along the height axis scales the box by 1/100th of the image's length.
        const float speed = (volumeSize.length() / 100.f) / static_cast<float>(height);
        const float scale = 1.0f + static_cast<float>(dy) * speed;

        const ::Ogre::Vector3 ccCenter = (m_clippingCube[1] + m_clippingCube[0]) / 2.f;

        // Scale clipping cube along its center.
        m_clippingCube[0] = (m_clippingCube[0] - ccCenter) * scale + ccCenter;
        m_clippingCube[1] = (m_clippingCube[1] - ccCenter) * scale + ccCenter;

        this->updateWidgets();

        m_clippingUpdateCallback();
    }

    return boxSelected;
}

//-----------------------------------------------------------------------------

bool ClippingBox::getVisibility() const
{
    return m_widgetSceneNode->getAttachedObject(0)->getVisible();
}

//-----------------------------------------------------------------------------

void ClippingBox::setVisibility(bool visibility)
{
    m_widgetSceneNode->setVisible(visibility);
}

//-----------------------------------------------------------------------------

::Ogre::AxisAlignedBox ClippingBox::getClippingBox() const
{
    return ::Ogre::AxisAlignedBox(m_clippingCube[0], m_clippingCube[1]);
}

//-----------------------------------------------------------------------------

::Ogre::Matrix4 ClippingBox::getClippingTransform() const
{
    const auto aaBox = this->getClippingBox();

    const ::Ogre::Vector3 initialCenter = m_volumeSceneNode->convertLocalToWorldPosition(::Ogre::Vector3(0.5f));
    ::Ogre::Matrix4 invCenterTransMx    = ::Ogre::Matrix4::IDENTITY;
    invCenterTransMx.setTrans(-initialCenter);

    const ::Ogre::Vector3 scale = aaBox.getSize();
    ::Ogre::Matrix4 scaleMx     = ::Ogre::Matrix4::IDENTITY;
    scaleMx.setScale(scale);

    const ::Ogre::Vector3 center  = m_volumeSceneNode->convertLocalToWorldPosition(aaBox.getCenter());
    ::Ogre::Matrix4 centerTransMx = ::Ogre::Matrix4::IDENTITY;
    centerTransMx.setTrans(center);

    const ::Ogre::Matrix4 clippingMx = centerTransMx * scaleMx * invCenterTransMx;

    return clippingMx;
}

//-----------------------------------------------------------------------------

void ClippingBox::updateFromTransform(const ::Ogre::Matrix4& _clippingMx)
{
    this->applyTransform(_clippingMx);
    m_clippingUpdateCallback();
}

//-----------------------------------------------------------------------------

void ClippingBox::applyTransform(const ::Ogre::Matrix4& _clippingMx)
{
    // The clipping matrix is the transform from the initial box position to the new one.
    // Convert to world position because that's how VTK stores its crop matrices...
    const ::Ogre::Vector3 worldCubeMin = m_volumeSceneNode->convertLocalToWorldPosition(::Ogre::Vector3::ZERO);
    const ::Ogre::Vector3 worldCubeMax = m_volumeSceneNode->convertLocalToWorldPosition(::Ogre::Vector3::UNIT_SCALE);

    m_clippingCube[0] = m_volumeSceneNode->convertWorldToLocalPosition(_clippingMx * worldCubeMin);
    m_clippingCube[1] = m_volumeSceneNode->convertWorldToLocalPosition(_clippingMx * worldCubeMax);

    this->updateWidgets();
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::widget
