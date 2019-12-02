/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SNegato2DCamera.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwRenderOgre/helper/Camera.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreNode.h>

namespace visuOgreAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_RESET_CAMERA_SLOT       = "resetCamera";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_ORIENTATION_SLOT = "changeOrientation";
static const ::fwCom::Slots::SlotKeyType s_MOVE_BACK_SLOT          = "moveBack";

static const ::fwServices::IService::KeyType s_IMAGE_INPUT = "image";

//-----------------------------------------------------------------------------

SNegato2DCamera::SNegato2DCamera() noexcept
{
    newSlot(s_RESET_CAMERA_SLOT, &SNegato2DCamera::resetCamera, this);
    newSlot(s_CHANGE_ORIENTATION_SLOT, &SNegato2DCamera::changeOrientation, this);
    newSlot(s_MOVE_BACK_SLOT, &SNegato2DCamera::moveBack, this);
}

//-----------------------------------------------------------------------------

SNegato2DCamera::~SNegato2DCamera() noexcept
{

}

//-----------------------------------------------------------------------------

void SNegato2DCamera::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_priority = config.get<int>("priority", m_priority);

    const std::string orientation = config.get<std::string>("orientation", "sagittal");

    SLM_ERROR_IF("Unknown orientation: '" + orientation +
                 "'. Orientation can be either 'axial', 'frontal' or 'sagittal'.",
                 orientation != "axial" && orientation != "frontal" && orientation != "sagittal");

    if(orientation == "axial")
    {
        m_currentNegatoOrientation = Orientation::Z_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_currentNegatoOrientation = Orientation::Y_AXIS;
    }
    else if(orientation == "sagittal")
    {
        m_currentNegatoOrientation = Orientation::X_AXIS;
    }
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::starting()
{
    this->initialize();

    const auto layer = this->getLayer();
    auto interactor  = std::dynamic_pointer_cast< ::fwRenderOgre::interactor::IInteractor >(this->getSptr());
    layer->addInteractor(interactor, m_priority);

    ::Ogre::Camera* const cam = this->getLayer()->getDefaultCamera();
    cam->setProjectionType( ::Ogre::ProjectionType::PT_ORTHOGRAPHIC );

    this->resetCamera();
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::updating() noexcept
{

}

//-----------------------------------------------------------------------------

void SNegato2DCamera::stopping()
{
    const auto layer = this->getLayer();
    auto interactor  = std::dynamic_pointer_cast< ::fwRenderOgre::interactor::IInteractor >(this->getSptr());
    layer->removeInteractor(interactor);
}

// ----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SNegato2DCamera::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_RESET_CAMERA_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_CHANGE_ORIENTATION_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_MOVE_BACK_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::wheelEvent(Modifier, int delta, int mouseX, int mouseY)
{
    const auto layer           = this->getLayer();
    const auto* const viewport = layer->getViewport();
    auto* const camera         = layer->getDefaultCamera();
    auto* const camNode        = camera->getParentNode();

    constexpr float mouseWheelScale = 0.05f;
    const float zoomAmount          = static_cast<float>(-delta) * mouseWheelScale;

    // Compute the mouse's position in the camera's view.
    const auto mousePosView = ::fwRenderOgre::helper::Camera::convertPixelToViewSpace(*camera, mouseX, mouseY);

    // Zoom in.
    const float orthoHeight    = camera->getOrthoWindowHeight();
    const float newOrthoHeight = orthoHeight + (orthoHeight / zoomAmount);
    const float clampedHeight  = std::max(newOrthoHeight, 1e-7f); // Make sure the height is strictly greater than 0.

    const float vpWidth  = static_cast<float>(viewport->getActualWidth());
    const float vpHeight = static_cast<float>(viewport->getActualHeight());

    camera->setAspectRatio(vpWidth / vpHeight);
    camera->setOrthoWindowHeight(clampedHeight);

    // Compute the mouse's position in the zoomed view.
    const auto newMousePosView = ::fwRenderOgre::helper::Camera::convertPixelToViewSpace(*camera, mouseX, mouseY);

    // Translate the camera back to the cursor's previous position.
    camNode->translate(mousePosView - newMousePosView);
}

// ----------------------------------------------------------------------------

void SNegato2DCamera::mouseMoveEvent(IInteractor::MouseButton button, Modifier, int x, int y, int dx, int dy)
{
    if(button == MIDDLE)
    {
        const auto layer    = this->getLayer();
        auto* const camera  = layer->getDefaultCamera();
        auto* const camNode = camera->getParentNode();

        const auto previousMousePosView =
            ::fwRenderOgre::helper::Camera::convertPixelToViewSpace(*camera, x - dx, y - dy);
        const auto mousePosView = ::fwRenderOgre::helper::Camera::convertPixelToViewSpace(*camera, x, y);

        camNode->translate(mousePosView - previousMousePosView);
    }
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::keyPressEvent(int key, Modifier)
{
    if(key == 'R' || key == 'r')
    {
        this->resetCamera();
    }
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::resetCamera()
{
    const auto layer           = this->getLayer();
    const auto* const viewport = layer->getViewport();
    auto* const camera         = layer->getDefaultCamera();
    auto* const camNode        = camera->getParentNode();

    const float vpWidth  = static_cast<float>(viewport->getActualWidth());
    const float vpHeight = static_cast<float>(viewport->getActualHeight());
    camera->setAspectRatio(vpWidth / vpHeight);

    const auto worldBoundingBox = layer->computeWorldBoundingBox();
    const auto worldSize        = worldBoundingBox.getSize();

    camNode->setPosition(::Ogre::Vector3::ZERO);
    camNode->resetOrientation();
    switch(m_currentNegatoOrientation)
    {
        case Orientation::X_AXIS:
            camNode->rotate(::Ogre::Vector3::UNIT_Y, ::Ogre::Degree(-90.f));
            camNode->rotate(::Ogre::Vector3::UNIT_Z, ::Ogre::Degree(-90.f));
            camera->setOrthoWindowHeight(worldSize.z);
            break;
        case Orientation::Y_AXIS:
            camNode->rotate(::Ogre::Vector3::UNIT_X, ::Ogre::Degree(90.f));
            camera->setOrthoWindowHeight(worldSize.z);
            break;
        case Orientation::Z_AXIS:
            camNode->rotate(::Ogre::Vector3::UNIT_Z, ::Ogre::Degree(180.f));
            camNode->rotate(::Ogre::Vector3::UNIT_Y, ::Ogre::Degree(180.f));
            camera->setOrthoWindowHeight(worldSize.y);
            break;
    }

    if(worldBoundingBox.isFinite())
    {
        const size_t orientation = static_cast<size_t>(m_currentNegatoOrientation);
        auto camPos              = worldBoundingBox.getCenter();

        // HACK: Temporarily set the near clip distance here because the Layer doesn't handle orthographic cameras.
        camera->setNearClipDistance(1e-3f);
        camPos[orientation] = worldBoundingBox.getMinimum()[orientation] - 1.f;

        camNode->setPosition(camPos);
        layer->resetCameraClippingRange(worldBoundingBox);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::changeOrientation(int /*_from*/, int _to)
{
    m_currentNegatoOrientation = static_cast<Orientation>(_to);
    this->resetCamera();
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::moveBack()
{
    const auto layer    = this->getLayer();
    auto* const camera  = layer->getDefaultCamera();
    auto* const camNode = camera->getParentNode();

    const auto worldBoundingBox = layer->computeWorldBoundingBox();

    if(worldBoundingBox.isFinite())
    {
        const size_t orientation = static_cast<size_t>(m_currentNegatoOrientation);

        auto camPos = camNode->getPosition();

        const float backupPos = worldBoundingBox.getMinimum()[orientation] - 1.f;
        camPos[orientation] = std::min(camPos[orientation], backupPos);

        camNode->setPosition(camPos);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

} // namespace visuOgreAdaptor