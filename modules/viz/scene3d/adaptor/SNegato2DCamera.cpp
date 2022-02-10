/************************************************************************
 *
 * Copyright (C) 2019-2022 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SNegato2DCamera.hpp"

#include <core/com/Slots.hxx>

#include <data/Image.hpp>
#include <data/TransferFunction.hpp>

#include <viz/scene3d/helper/Camera.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreNode.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::Slots::SlotKeyType s_RESET_CAMERA_SLOT       = "resetCamera";
static const core::com::Slots::SlotKeyType s_CHANGE_ORIENTATION_SLOT = "changeOrientation";
static const core::com::Slots::SlotKeyType s_MOVE_BACK_SLOT          = "moveBack";

static const std::string s_PRIORITY_CONFIG              = "priority";
static const std::string s_LAYER_ORDER_DEPENDANT_CONFIG = "layerOrderDependant";
static const std::string s_ORIENTATION_CONFIG           = "orientation";

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

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_priority            = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_layerOrderDependant = config.get<bool>(s_LAYER_ORDER_DEPENDANT_CONFIG, m_layerOrderDependant);

    const std::string orientation = config.get<std::string>(s_ORIENTATION_CONFIG, "sagittal");

    SIGHT_ERROR_IF(
        "Unknown orientation: '" + orientation
        + "'. Orientation can be either 'axial', 'frontal' or 'sagittal'.",
        orientation != "axial" && orientation != "frontal" && orientation != "sagittal"
    );

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
    auto interactor  = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(this->getSptr());
    layer->addInteractor(interactor, m_priority);

    Ogre::Camera* const cam = this->getLayer()->getDefaultCamera();
    cam->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);

    this->resetCamera();
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::updating() noexcept
{
    // Only used for the TF helper.
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::swapping(std::string_view _key)
{
    if(_key == s_TF_INOUT)
    {
        const auto image = m_image.lock();
        const auto tf    = m_tf.lock();
        m_helperTF.setOrCreateTF(tf.get_shared(), image.get_shared());
    }
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::stopping()
{
    const auto layer = this->getLayer();
    auto interactor  = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(this->getSptr());
    layer->removeInteractor(interactor);
}

// ----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SNegato2DCamera::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, data::Image::s_MODIFIED_SIG, s_RESET_CAMERA_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_CHANGE_ORIENTATION_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_INDEX_MODIFIED_SIG, s_MOVE_BACK_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::wheelEvent(Modifier _modifier, int _delta, int _x, int _y)
{
    const auto layer = this->getLayer();

    if(IInteractor::isInLayer(_x, _y, layer, m_layerOrderDependant))
    {
        // CTRL + wheel = Zoom in/out.
        if(_modifier == Modifier::CONTROL)
        {
            const auto* const viewport = layer->getViewport();
            auto* const camera         = layer->getDefaultCamera();
            auto* const camNode        = camera->getParentNode();

            constexpr float mouseWheelScale = 0.05f;
            const float zoomAmount          = static_cast<float>(-_delta) * mouseWheelScale;

            // Compute the mouse's position in the camera's view.
            const Ogre::Vector3 screenPos(static_cast<Ogre::Real>(_x),
                                          static_cast<Ogre::Real>(_y),
                                          Ogre::Real(0));
            const auto mousePosView =
                sight::viz::scene3d::helper::Camera::convertScreenSpaceToViewSpace(*camera, screenPos);

            // Zoom in.
            const float orthoHeight    = camera->getOrthoWindowHeight();
            const float newOrthoHeight = orthoHeight + (orthoHeight / zoomAmount);
            const float clampedHeight  = std::max(newOrthoHeight, 1e-7f); // Make sure the height is strictly greater
                                                                          // than 0

            const float vpWidth  = static_cast<float>(viewport->getActualWidth());
            const float vpHeight = static_cast<float>(viewport->getActualHeight());

            camera->setAspectRatio(vpWidth / vpHeight);
            camera->setOrthoWindowHeight(clampedHeight);

            // Compute the mouse's position in the zoomed view.
            const auto newMousePosView =
                sight::viz::scene3d::helper::Camera::convertScreenSpaceToViewSpace(*camera, screenPos);

            // Translate the camera back to the cursor's previous position.
            camNode->translate(mousePosView - newMousePosView);
        }
        // Wheel alone or other modifier -> moving along slices (SHIFT to speed-up)
        else
        {
            namespace imHelper = data::helper::MedicalImage;

            const auto image = m_image.lock();

            // Get Index
            const int current_index = static_cast<int>(imHelper::getSliceIndex(*image, m_currentNegatoOrientation));
            const int max_slice     = static_cast<int>(image->getSize()[m_currentNegatoOrientation] - 1);

            if(max_slice < 0)
            {
                // Do nothing, image doesn't have slices.
                return;
            }

            SIGHT_ASSERT("Slice index field missing", current_index != -1);

            // From: https://doc.qt.io/qt-5/qwheelevent.html#angleDelta
            // Most mouse types work in steps of 15 degrees,
            // in which case the delta value is a multiple of 120; i.e., 120 units * 1/8 = 15 degrees.

            // Here we assume that each 120 units of delta correspond to one increment of mouse wheel
            // So we move forward/backward of 1 slice each 120 units.

            int slice_move = _delta / 120;

            // Speed up SHIFT+ wheel: "scrolls" 5% of total slices at each wheel move.
            if(_modifier == Modifier::SHIFT)
            {
                slice_move *= std::round(static_cast<float>(max_slice) * 5.f / 100.f);
            }

            // TODO: We may test for finer-resolution wheels and wait for another event before moving.

            auto new_slice_index = current_index + slice_move;

            // Limit to [0: max_slice].
            if(new_slice_index > max_slice)
            {
                new_slice_index = max_slice;
            }
            else if(new_slice_index < 0)
            {
                new_slice_index = 0;
            }

            imHelper::setSliceIndex(*image, m_currentNegatoOrientation, new_slice_index);

            // Get up-to-date index values.
            const int idx[3] = {
                static_cast<int>(imHelper::getSliceIndex(*image, imHelper::orientation_t::SAGITTAL)),
                static_cast<int>(imHelper::getSliceIndex(*image, imHelper::orientation_t::FRONTAL)),
                static_cast<int>(imHelper::getSliceIndex(*image, imHelper::orientation_t::AXIAL))
            };

            // Send signal.
            auto sig = image->signal<data::Image::SliceIndexModifiedSignalType>(
                data::Image::s_SLICE_INDEX_MODIFIED_SIG
            );
            // this->moveBack() will be automatically called due to auto-connection.
            sig->asyncEmit(idx[2], idx[1], idx[0]);
        }
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

        const Ogre::Vector3 deltaScreenPos(static_cast<Ogre::Real>(_x - _dx),
                                           static_cast<Ogre::Real>(_y - _dy),
                                           Ogre::Real(0));
        const Ogre::Vector3 screenPos(static_cast<Ogre::Real>(_x),
                                      static_cast<Ogre::Real>(_y),
                                      Ogre::Real(0));

        const auto previousMousePosView =
            sight::viz::scene3d::helper::Camera::convertScreenSpaceToViewSpace(*camera, deltaScreenPos);
        const auto mousePosView =
            sight::viz::scene3d::helper::Camera::convertScreenSpaceToViewSpace(*camera, screenPos);

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
        m_isInteracting = IInteractor::isInLayer(_x, _y, layer, m_layerOrderDependant);
    }
    else if(_button == MouseButton::RIGHT && IInteractor::isInLayer(_x, _y, layer, m_layerOrderDependant))
    {
        m_isInteracting = true;

        const data::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
        const data::mt::locked_ptr lock(tf);

        m_initialLevel  = tf->getLevel();
        m_initialWindow = tf->getWindow();

        m_initialPos = {_x, _y};
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
    if(IInteractor::isInLayer(_x, _y, layer, m_layerOrderDependant) && (_key == 'R' || _key == 'r'))
    {
        this->resetCamera();
    }
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::resetCamera()
{
    // This method is called when the image buffer is modified,
    // we need to retrieve the TF here if it came from the image.
    const auto image = m_image.lock();
    const auto tf    = m_tf.lock();
    m_helperTF.setOrCreateTF(tf.get_shared(), image.get_shared());

    const auto layer           = this->getLayer();
    const auto* const viewport = layer->getViewport();
    auto* const camera         = layer->getDefaultCamera();
    auto* const camNode        = camera->getParentNode();

    const float vpWidth  = static_cast<float>(viewport->getActualWidth());
    const float vpHeight = static_cast<float>(viewport->getActualHeight());
    const float vpRatio  = vpWidth / vpHeight;
    camera->setAspectRatio(vpRatio);

    // HACK: Temporarily set the near clip distance here because the Layer doesn't handle orthographic cameras.
    camera->setNearClipDistance(1e-3f);

    camNode->setPosition(Ogre::Vector3::ZERO);
    camNode->resetOrientation();

    const auto origin  = image->getOrigin();
    const auto size    = image->getSize();
    const auto spacing = image->getSpacing();

    if(size[0] > 0 && size[1] > 0 && size[2] > 0)
    {
        float ratio   = 0;
        double width  = 0.;
        double height = 0.;
        switch(m_currentNegatoOrientation)
        {
            case Orientation::X_AXIS:
                camNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(-90.f));
                camNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Degree(-90.f));
                width  = static_cast<double>(size[1]) * spacing[1];
                height = static_cast<double>(size[2]) * spacing[2];
                ratio  = static_cast<float>(width / height);
                break;

            case Orientation::Y_AXIS:
                camNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(90.f));
                width  = static_cast<double>(size[0]) * spacing[0];
                height = static_cast<double>(size[2]) * spacing[2];
                ratio  = static_cast<float>(width / height);
                break;

            case Orientation::Z_AXIS:
                camNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Degree(180.f));
                camNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(180.f));
                height = static_cast<double>(size[0]) * spacing[0];
                width  = static_cast<double>(size[1]) * spacing[1];
                ratio  = static_cast<float>(width / height);
                break;
        }

        if(vpRatio > ratio)
        {
            const Ogre::Real h = static_cast<Ogre::Real>(height);
            // Zoom out the camera (add 10% of the height), allow the image to not be stuck on the viewport.
            camera->setOrthoWindowHeight(h + h * 0.1f);
        }
        else
        {
            const Ogre::Real w = static_cast<Ogre::Real>(width);
            // Zoom out the camera (add 10% of the width), allow the image to not be stuck on the viewport.
            camera->setOrthoWindowWidth(w + w * 0.1f);
        }

        const std::size_t orientation = static_cast<std::size_t>(m_currentNegatoOrientation);
        Ogre::Vector3 camPos(
            static_cast<Ogre::Real>(origin[0] + static_cast<double>(size[0]) * spacing[0] * 0.5),
            static_cast<Ogre::Real>(origin[1] + static_cast<double>(size[1]) * spacing[1] * 0.5),
            static_cast<Ogre::Real>(origin[2] + static_cast<double>(size[2]) * spacing[2] * 0.5)
        );

        camPos[orientation] =
            static_cast<Ogre::Real>(origin[orientation] - static_cast<double>(size[orientation])
                                    * spacing[orientation]);
        camNode->setPosition(camPos);

        const auto worldBoundingBox = layer->computeWorldBoundingBox();
        if(worldBoundingBox.isFinite())
        {
            layer->resetCameraClippingRange(worldBoundingBox);
        }

        this->requestRender();
    }
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
        const std::size_t orientation = static_cast<std::size_t>(m_currentNegatoOrientation);

        auto camPos = camNode->getPosition();

        const float backupPos = worldBoundingBox.getMinimum()[orientation] - 1.f;
        camPos[orientation] = std::min(camPos[orientation], backupPos);

        camNode->setPosition(camPos);

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SNegato2DCamera::updateWindowing(double _dw, double _dl)
{
    const double newWindow = m_initialWindow + _dw;
    const double newLevel  = m_initialLevel - _dl;

    const data::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
    {
        const data::mt::locked_ptr lock(tf);

        tf->setWindow(newWindow);
        tf->setLevel(newLevel);
        const auto sig = tf->signal<data::TransferFunction::WindowingModifiedSignalType>(
            data::TransferFunction::s_WINDOWING_MODIFIED_SIG
        );
        {
            sig->asyncEmit(newWindow, newLevel);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
