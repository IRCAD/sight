/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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
static const core::com::Slots::SlotKeyType s_RESIZE_VIEWPORT_SLOT    = "resizeViewport";
static const core::com::Slots::SlotKeyType s_CHANGE_ORIENTATION_SLOT = "changeOrientation";

//-----------------------------------------------------------------------------

SNegato2DCamera::SNegato2DCamera() noexcept
{
    newSlot(s_RESET_CAMERA_SLOT, &SNegato2DCamera::resetCamera, this);
    newSlot(s_RESIZE_VIEWPORT_SLOT, &SNegato2DCamera::resizeViewport, this);
    newSlot(s_CHANGE_ORIENTATION_SLOT, &SNegato2DCamera::changeOrientation, this);
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    m_priority            = config.get<int>(s_CONFIG + "priority", m_priority);
    m_layerOrderDependant = config.get<bool>(s_CONFIG + "layerOrderDependant", m_layerOrderDependant);

    const std::string orientation = config.get<std::string>(s_CONFIG + "orientation", "sagittal");

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

    m_margin = config.get<float>(s_CONFIG + "margin", m_margin);
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

    m_layerConnection.connect(
        this->getLayer(),
        sight::viz::scene3d::Layer::s_RESIZE_LAYER_SIG,
        this->getSptr(),
        s_RESIZE_VIEWPORT_SLOT
    );

    this->resetCamera();
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::updating() noexcept
{
    // Only used for the TF helper.
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::stopping()
{
    m_layerConnection.disconnect();
    const auto layer = this->getLayer();
    auto interactor  = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(this->getSptr());
    layer->removeInteractor(interactor);
}

// ----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SNegato2DCamera::getAutoConnections() const
{
    return {
        {s_IMAGE_INOUT, data::Image::s_MODIFIED_SIG, s_RESET_CAMERA_SLOT},
        {s_IMAGE_INOUT, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_CHANGE_ORIENTATION_SLOT}
    };
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::wheelEvent(Modifier _modifier, double _delta, int _x, int _y)
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

            constexpr float mouseWheelScale = 0.05F;
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
            const float clampedHeight  = std::max(newOrthoHeight, 1e-7F); // Make sure the height is strictly greater
                                                                          // than 0

            const auto vpWidth  = static_cast<float>(viewport->getActualWidth());
            const auto vpHeight = static_cast<float>(viewport->getActualHeight());

            SIGHT_ASSERT("Width and height should be strictly positive", vpWidth > 0 && vpHeight > 0);
            camera->setAspectRatio(vpWidth / vpHeight);
            camera->setOrthoWindowHeight(clampedHeight);

            // Compute the mouse's position in the zoomed view.
            const auto newMousePosView =
                sight::viz::scene3d::helper::Camera::convertScreenSpaceToViewSpace(*camera, screenPos);

            // Translate the camera back to the cursor's previous position.
            camNode->translate(mousePosView - newMousePosView);
            m_hasMoved = true;

            this->requestRender();
        }
        // Wheel alone or other modifier -> moving along slices (SHIFT to speed-up)
        else
        {
            namespace imHelper = data::helper::MedicalImage;

            const auto image = m_image.lock();

            // Get Index
            const int current_index =
                static_cast<int>(imHelper::getSliceIndex(*image, m_currentNegatoOrientation).value_or(0));
            const int max_slice = static_cast<int>(image->getSize()[m_currentNegatoOrientation] - 1);

            if(max_slice <= 0)
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

            int slice_move = static_cast<int>(_delta) / 120;

            // Speed up SHIFT+ wheel: "scrolls" 5% of total slices at each wheel move.
            if(_modifier == Modifier::SHIFT)
            {
                slice_move *= int(std::round(static_cast<float>(max_slice) * 5.F / 100.F));
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
            const std::array<int, 3> idx {
                static_cast<int>(imHelper::getSliceIndex(*image, imHelper::orientation_t::SAGITTAL).value_or(0)),
                static_cast<int>(imHelper::getSliceIndex(*image, imHelper::orientation_t::FRONTAL).value_or(0)),
                static_cast<int>(imHelper::getSliceIndex(*image, imHelper::orientation_t::AXIAL).value_or(0))
            };
            m_hasMoved = true;

            // Send signal.
            auto sig = image->signal<data::Image::SliceIndexModifiedSignalType>(
                data::Image::s_SLICE_INDEX_MODIFIED_SIG
            );
            sig->asyncEmit(idx[2], idx[1], idx[0]);
        }
    }
}

//------------------------------------------------------------------------------

void SNegato2DCamera::pinchGestureEvent(double _scaleFactor, int _centerX, int _centerY)
{
    // * 42 / 0.05 is a magic number to get a similar behavior as the mouse wheel
    wheelEvent(Modifier::CONTROL, (_scaleFactor * 42) / 0.05, _centerX, _centerY);
}

//------------------------------------------------------------------------------

void SNegato2DCamera::panGestureMoveEvent(int _x, int _y, int _dx, int _dy)
{
    m_isInteracting = true;

    mouseMoveEvent(MouseButton::MIDDLE, {}, _x, _y, _dx, _dy);
}

//------------------------------------------------------------------------------

void SNegato2DCamera::panGestureReleaseEvent(int /*_x*/, int /*_y*/, int /*_dx*/, int /*_dy*/)
{
    m_isInteracting = false;
}

// ----------------------------------------------------------------------------

void SNegato2DCamera::mouseMoveEvent(
    IInteractor::MouseButton _button,
    Modifier /*_mods*/,
    int _x,
    int _y,
    int _dx,
    int _dy
)
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
        m_hasMoved = true;
    }
    else if(m_isInteracting && _button == MouseButton::RIGHT)
    {
        const auto dx = static_cast<double>(_x - m_initialPos[0]);
        const auto dy = static_cast<double>(m_initialPos[1] - _y);

        this->updateWindowing(dx, dy);
    }
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::buttonPressEvent(IInteractor::MouseButton _button, Modifier /*_mods*/, int _x, int _y)
{
    const auto layer = this->getLayer();
    if(_button == MouseButton::MIDDLE)
    {
        m_isInteracting = IInteractor::isInLayer(_x, _y, layer, m_layerOrderDependant);
    }
    else if(_button == MouseButton::RIGHT && IInteractor::isInLayer(_x, _y, layer, m_layerOrderDependant))
    {
        m_isInteracting = true;

        const auto tf = m_tf.const_lock();

        m_initialLevel  = tf->level();
        m_initialWindow = tf->window();

        m_initialPos = {_x, _y};
    }
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::buttonReleaseEvent(
    IInteractor::MouseButton /*_button*/,
    Modifier /*_mods*/,
    int /*_x*/,
    int /*_y*/
)
{
    m_isInteracting = false;
}

//-----------------------------------------------------------------------------

void SNegato2DCamera::keyPressEvent(int _key, Modifier /*_mods*/, int _x, int _y)
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
    const auto layer           = this->getLayer();
    const auto* const viewport = layer->getViewport();
    auto* const camera         = layer->getDefaultCamera();
    auto* const camNode        = camera->getParentNode();

    const auto vpWidth  = static_cast<float>(viewport->getActualWidth());
    const auto vpHeight = static_cast<float>(viewport->getActualHeight());
    SIGHT_ASSERT("Width and height should be strictly positive", vpWidth > 0 && vpHeight > 0);
    const float vpRatio = vpWidth / vpHeight;
    camera->setAspectRatio(vpRatio);

    // HACK: Temporarily set the near clip distance here because the Layer doesn't handle orthographic cameras.
    camera->setNearClipDistance(1e-3F);

    camNode->setPosition(Ogre::Vector3::ZERO);
    camNode->resetOrientation();

    const auto image   = m_image.const_lock();
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
                camNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(-90.F));
                camNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Degree(-90.F));
                width  = static_cast<double>(size[1]) * spacing[1];
                height = static_cast<double>(size[2]) * spacing[2];
                ratio  = static_cast<float>(width / height);
                break;

            case Orientation::Y_AXIS:
                camNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(90.F));
                width  = static_cast<double>(size[0]) * spacing[0];
                height = static_cast<double>(size[2]) * spacing[2];
                ratio  = static_cast<float>(width / height);
                break;

            case Orientation::Z_AXIS:
                camNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Degree(180.F));
                camNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(180.F));
                width  = static_cast<double>(size[0]) * spacing[0];
                height = static_cast<double>(size[1]) * spacing[1];
                ratio  = static_cast<float>(width / height);
                break;
        }

        if(vpRatio > ratio)
        {
            const auto h = static_cast<Ogre::Real>(height);
            // Zoom out the camera with the margin, allow the image to not be stuck on the viewport.
            camera->setOrthoWindowHeight(h + h * m_margin);
        }
        else
        {
            const auto w = static_cast<Ogre::Real>(width);
            // Zoom out the camera with the margin, allow the image to not be stuck on the viewport.
            camera->setOrthoWindowWidth(w + w * m_margin);
        }

        const auto orientation = static_cast<std::size_t>(m_currentNegatoOrientation);
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

        m_hasMoved = false;
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SNegato2DCamera::resizeViewport()
{
    if(!m_hasMoved)
    {
        this->resetCamera();
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

//------------------------------------------------------------------------------

void SNegato2DCamera::updateWindowing(double _dw, double _dl)
{
    const double newWindow = m_initialWindow + _dw;
    const double newLevel  = m_initialLevel - _dl;

    {
        const auto image = m_image.const_lock();
        const auto tf    = m_tf.lock();

        tf->setWindow(newWindow);
        tf->setLevel(newLevel);
        const auto sig = tf->template signal<data::TransferFunction::WindowingModifiedSignalType>(
            data::TransferFunction::s_WINDOWING_MODIFIED_SIG
        );
        {
            sig->asyncEmit(newWindow, newLevel);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
