/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwRenderOgre/helper/Camera.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreNode.h>

namespace visuOgreAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_RESET_CAMERA_SLOT       = "resetCamera";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_ORIENTATION_SLOT = "changeOrientation";
static const ::fwCom::Slots::SlotKeyType s_MOVE_BACK_SLOT          = "moveBack";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";
static const ::fwServices::IService::KeyType s_TF_INOUT    = "tf";

//-----------------------------------------------------------------------------

SNegato2DCamera::SNegato2DCamera() noexcept :
    // This connection is useless here but needed to create the TF helper.
    m_helperTF(std::bind(&SNegato2DCamera::updating, this))
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
    // Only used for the TF helper.
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::swapping(const KeyType& _key)
{
    if(_key == s_TF_INOUT)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing.", image);

        ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
        m_helperTF.setOrCreateTF(tf, image);
    }
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
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_RESET_CAMERA_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_CHANGE_ORIENTATION_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_MOVE_BACK_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::wheelEvent(Modifier, int _delta, int _x, int _y)
{
    const auto layer = this->getLayer();

    if(IInteractor::isInLayer(_x, _y, layer))
    {
        const auto* const viewport = layer->getViewport();
        auto* const camera         = layer->getDefaultCamera();
        auto* const camNode        = camera->getParentNode();

        constexpr float mouseWheelScale = 0.05f;
        const float zoomAmount          = static_cast<float>(-_delta) * mouseWheelScale;

        // Compute the mouse's position in the camera's view.
        const auto mousePosView = ::fwRenderOgre::helper::Camera::convertPixelToViewSpace(*camera, _x, _y);

        // Zoom in.
        const float orthoHeight    = camera->getOrthoWindowHeight();
        const float newOrthoHeight = orthoHeight + (orthoHeight / zoomAmount);
        const float clampedHeight  = std::max(newOrthoHeight, 1e-7f); // Make sure the height is strictly greater than 0

        const float vpWidth  = static_cast<float>(viewport->getActualWidth());
        const float vpHeight = static_cast<float>(viewport->getActualHeight());

        camera->setAspectRatio(vpWidth / vpHeight);
        camera->setOrthoWindowHeight(clampedHeight);

        // Compute the mouse's position in the zoomed view.
        const auto newMousePosView = ::fwRenderOgre::helper::Camera::convertPixelToViewSpace(*camera, _x, _y);

        // Translate the camera back to the cursor's previous position.
        camNode->translate(mousePosView - newMousePosView);
    }
}

// ----------------------------------------------------------------------------

void SNegato2DCamera::mouseMoveEvent(IInteractor::MouseButton _button, Modifier, int _x, int _y, int _dx, int _dy)
{
    if(m_isInteracting && _button == MouseButton::MIDDLE)
    {
        const auto layer = this->getLayer();

        auto* const camera  = layer->getDefaultCamera();
        auto* const camNode = camera->getParentNode();

        const auto previousMousePosView =
            ::fwRenderOgre::helper::Camera::convertPixelToViewSpace(*camera, _x - _dx, _y - _dy);
        const auto mousePosView = ::fwRenderOgre::helper::Camera::convertPixelToViewSpace(*camera, _x, _y);

        camNode->translate(mousePosView - previousMousePosView);
    }
    else if(m_isInteracting && _button == MouseButton::RIGHT)
    {
        const double dx = static_cast<double>(_x - m_initialPos[0]);
        const double dy = static_cast<double>(m_initialPos[1] - _y);

        this->updateWindowing(dx, dy);
    }
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::buttonPressEvent(IInteractor::MouseButton _button, Modifier, int _x, int _y)
{
    const auto layer = this->getLayer();
    if(_button == MouseButton::MIDDLE)
    {
        m_isInteracting = IInteractor::isInLayer(_x, _y, layer);
    }
    else if(_button == MouseButton::RIGHT && IInteractor::isInLayer(_x, _y, layer))
    {
        m_isInteracting = true;

        const ::fwData::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
        const ::fwData::mt::ObjectReadLock tfLock(tf);

        m_initialLevel  = tf->getLevel();
        m_initialWindow = tf->getWindow();

        m_initialPos = { _x, _y };
    }
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::buttonReleaseEvent(IInteractor::MouseButton, Modifier, int, int)
{
    m_isInteracting = false;
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::keyPressEvent(int _key, Modifier, int _x, int _y)
{
    const auto layer = this->getLayer();
    if(IInteractor::isInLayer(_x, _y, layer) && (_key == 'R' || _key == 'r'))
    {
        this->resetCamera();
    }
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::resetCamera()
{
    // This method is called when the image buffer is modified,
    // we need to retrieve the TF here if it came from the image.
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing.", image);

    const ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
    m_helperTF.setOrCreateTF(tf, image);

    const auto layer           = this->getLayer();
    const auto* const viewport = layer->getViewport();
    auto* const camera         = layer->getDefaultCamera();
    auto* const camNode        = camera->getParentNode();

    const float vpWidth  = static_cast<float>(viewport->getActualWidth());
    const float vpHeight = static_cast<float>(viewport->getActualHeight());
    camera->setAspectRatio(vpWidth / vpHeight);

    const auto worldBoundingBox = layer->computeWorldBoundingBox();

    if(worldBoundingBox.isFinite())
    {
        const auto worldSize = worldBoundingBox.getSize();

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

void SNegato2DCamera::changeOrientation(int _from, int _to)
{
    const auto toOrientation   = static_cast<Orientation>(_to);
    const auto fromOrientation = static_cast<Orientation>(_from);

    if(m_currentNegatoOrientation == toOrientation)
    {
        m_currentNegatoOrientation = fromOrientation;
        this->resetCamera();
    }
    else if(m_currentNegatoOrientation == fromOrientation)
    {
        m_currentNegatoOrientation = toOrientation;
        this->resetCamera();
    }
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

//------------------------------------------------------------------------------

void SNegato2DCamera::updateWindowing( double _dw, double _dl )
{
    const double newWindow = m_initialWindow + _dw;
    const double newLevel  = m_initialLevel - _dl;

    const ::fwData::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
    {
        const ::fwData::mt::ObjectWriteLock tfLock(tf);

        tf->setWindow( newWindow );
        tf->setLevel( newLevel );
        const auto sig = tf->signal< ::fwData::TransferFunction::WindowingModifiedSignalType >(
            ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG);
        {
            sig->asyncEmit(newWindow, newLevel);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace visuOgreAdaptor
