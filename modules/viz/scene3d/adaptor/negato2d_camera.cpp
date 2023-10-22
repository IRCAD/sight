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

#include "modules/viz/scene3d/adaptor/negato2d_camera.hpp"

#include <core/com/slots.hxx>

#include <data/image.hpp>
#include <data/transfer_function.hpp>

#include <viz/scene3d/helper/camera.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreNode.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t RESET_CAMERA_SLOT       = "resetCamera";
static const core::com::slots::key_t RESIZE_VIEWPORT_SLOT    = "resizeViewport";
static const core::com::slots::key_t CHANGE_ORIENTATION_SLOT = "changeOrientation";

namespace interactor_3d = sight::viz::scene3d::interactor;

//-----------------------------------------------------------------------------

negato2d_camera::negato2d_camera() noexcept
{
    new_slot(RESET_CAMERA_SLOT, &negato2d_camera::resetCamera, this);
    new_slot(RESIZE_VIEWPORT_SLOT, &negato2d_camera::resizeViewport, this);
    new_slot(CHANGE_ORIENTATION_SLOT, &negato2d_camera::changeOrientation, this);
}

//-----------------------------------------------------------------------------

void negato2d_camera::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

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

void negato2d_camera::starting()
{
    this->initialize();

    const auto layer = this->getLayer();
    auto interactor  = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->addInteractor(interactor, m_priority);

    Ogre::Camera* const cam = this->getLayer()->getDefaultCamera();
    cam->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);

    m_layerConnection.connect(
        this->getLayer(),
        sight::viz::scene3d::layer::RESIZE_LAYER_SIG,
        this->get_sptr(),
        RESIZE_VIEWPORT_SLOT
    );

    this->resetCamera();
}

//-----------------------------------------------------------------------------

void negato2d_camera::updating() noexcept
{
    // Only used for the TF helper.
}

//-----------------------------------------------------------------------------

void negato2d_camera::stopping()
{
    m_layerConnection.disconnect();
    const auto layer = this->getLayer();
    auto interactor  = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->removeInteractor(interactor);
}

// ----------------------------------------------------------------------------

service::connections_t negato2d_camera::auto_connections() const
{
    return {
        {s_IMAGE_INOUT, data::image::MODIFIED_SIG, RESET_CAMERA_SLOT},
        {s_IMAGE_INOUT, data::image::SLICE_TYPE_MODIFIED_SIG, CHANGE_ORIENTATION_SLOT}
    };
}

//-----------------------------------------------------------------------------

void negato2d_camera::wheelEvent(Modifier _modifier, double _delta, int _x, int _y)
{
    const auto layer = this->getLayer();

    if(interactor_3d::base::isInLayer(_x, _y, layer, m_layerOrderDependant))
    {
        // CTRL + wheel = Zoom in/out.
        if(_modifier == Modifier::CONTROL)
        {
            const auto* const viewport = layer->getViewport();
            auto* const camera         = layer->getDefaultCamera();
            auto* const cam_node       = camera->getParentNode();

            constexpr float mouse_wheel_scale = 0.05F;
            const float zoom_amount           = static_cast<float>(-_delta) * mouse_wheel_scale;

            // Compute the mouse's position in the camera's view.
            const Ogre::Vector3 screen_pos(static_cast<Ogre::Real>(_x),
                                           static_cast<Ogre::Real>(_y),
                                           Ogre::Real(0));
            const auto mouse_pos_view =
                sight::viz::scene3d::helper::camera::convertScreenSpaceToViewSpace(*camera, screen_pos);

            // Zoom in.
            const float ortho_height     = camera->getOrthoWindowHeight();
            const float new_ortho_height = ortho_height + (ortho_height / zoom_amount);
            const float clamped_height   = std::max(new_ortho_height, 1e-7F); // Make sure the height is strictly
                                                                              // greater
                                                                              // than 0

            const auto vp_width  = static_cast<float>(viewport->getActualWidth());
            const auto vp_height = static_cast<float>(viewport->getActualHeight());

            SIGHT_ASSERT("Width and height should be strictly positive", vp_width > 0 && vp_height > 0);
            camera->setAspectRatio(vp_width / vp_height);
            camera->setOrthoWindowHeight(clamped_height);

            // Compute the mouse's position in the zoomed view.
            const auto new_mouse_pos_view =
                sight::viz::scene3d::helper::camera::convertScreenSpaceToViewSpace(*camera, screen_pos);

            // Translate the camera back to the cursor's previous position.
            cam_node->translate(mouse_pos_view - new_mouse_pos_view);
            m_hasMoved = true;

            this->requestRender();
        }
        // Wheel alone or other modifier -> moving along slices (SHIFT to speed-up)
        else
        {
            namespace imHelper = data::helper::medical_image;

            const auto image = m_image.lock();

            // Get Index
            const int current_index =
                static_cast<int>(imHelper::get_slice_index(*image, m_currentNegatoOrientation).value_or(0));
            const int max_slice = static_cast<int>(image->size()[m_currentNegatoOrientation] - 1);

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

            imHelper::set_slice_index(*image, m_currentNegatoOrientation, new_slice_index);

            // Get up-to-date index values.
            const std::array<int, 3> idx {
                static_cast<int>(imHelper::get_slice_index(*image, imHelper::orientation_t::SAGITTAL).value_or(0)),
                static_cast<int>(imHelper::get_slice_index(*image, imHelper::orientation_t::FRONTAL).value_or(0)),
                static_cast<int>(imHelper::get_slice_index(*image, imHelper::orientation_t::AXIAL).value_or(0))
            };
            m_hasMoved = true;

            // Send signal.
            auto sig = image->signal<data::image::SliceIndexModifiedSignalType>(
                data::image::SLICE_INDEX_MODIFIED_SIG
            );
            sig->async_emit(idx[2], idx[1], idx[0]);
        }
    }
}

//------------------------------------------------------------------------------

void negato2d_camera::pinchGestureEvent(double _scale_factor, int _center_x, int _center_y)
{
    // * 42 / 0.05 is a magic number to get a similar behavior as the mouse wheel
    wheelEvent(Modifier::CONTROL, (_scale_factor * 42) / 0.05, _center_x, _center_y);
}

//------------------------------------------------------------------------------

void negato2d_camera::panGestureMoveEvent(int _x, int _y, int _dx, int _dy)
{
    m_isInteracting = true;

    mouseMoveEvent(MouseButton::MIDDLE, {}, _x, _y, _dx, _dy);
}

//------------------------------------------------------------------------------

void negato2d_camera::panGestureReleaseEvent(int /*_x*/, int /*_y*/, int /*_dx*/, int /*_dy*/)
{
    m_isInteracting = false;
}

// ----------------------------------------------------------------------------

void negato2d_camera::mouseMoveEvent(
    interactor_3d::base::MouseButton _button,
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

        auto* const camera   = layer->getDefaultCamera();
        auto* const cam_node = camera->getParentNode();

        const Ogre::Vector3 delta_screen_pos(static_cast<Ogre::Real>(_x - _dx),
                                             static_cast<Ogre::Real>(_y - _dy),
                                             Ogre::Real(0));
        const Ogre::Vector3 screen_pos(static_cast<Ogre::Real>(_x),
                                       static_cast<Ogre::Real>(_y),
                                       Ogre::Real(0));

        const auto previous_mouse_pos_view =
            sight::viz::scene3d::helper::camera::convertScreenSpaceToViewSpace(*camera, delta_screen_pos);
        const auto mouse_pos_view =
            sight::viz::scene3d::helper::camera::convertScreenSpaceToViewSpace(*camera, screen_pos);

        cam_node->translate(mouse_pos_view - previous_mouse_pos_view);
        m_hasMoved = true;
        this->requestRender();
    }
    else if(m_isInteracting && _button == MouseButton::RIGHT)
    {
        const auto dx = static_cast<double>(_x - m_initialPos[0]);
        const auto dy = static_cast<double>(m_initialPos[1] - _y);

        this->updateWindowing(dx, dy);
    }
}

//-----------------------------------------------------------------------------

void negato2d_camera::buttonPressEvent(interactor_3d::base::MouseButton _button, Modifier /*_mods*/, int _x, int _y)
{
    const auto layer = this->getLayer();
    if(_button == MouseButton::MIDDLE)
    {
        m_isInteracting = interactor_3d::base::isInLayer(_x, _y, layer, m_layerOrderDependant);
    }
    else if(_button == MouseButton::RIGHT && interactor_3d::base::isInLayer(_x, _y, layer, m_layerOrderDependant))
    {
        m_isInteracting = true;

        const auto tf = m_tf.const_lock();

        m_initialLevel  = tf->level();
        m_initialWindow = tf->window();

        m_initialPos = {_x, _y};
    }
}

//-----------------------------------------------------------------------------

void negato2d_camera::buttonReleaseEvent(
    interactor_3d::base::MouseButton /*_button*/,
    Modifier /*_mods*/,
    int /*_x*/,
    int /*_y*/
)
{
    m_isInteracting = false;
}

//-----------------------------------------------------------------------------

void negato2d_camera::keyPressEvent(int _key, Modifier /*_mods*/, int _x, int _y)
{
    const auto layer = this->getLayer();
    if(interactor_3d::base::isInLayer(_x, _y, layer, m_layerOrderDependant) && (_key == 'R' || _key == 'r'))
    {
        this->resetCamera();
    }
}

//-----------------------------------------------------------------------------

void negato2d_camera::resetCamera()
{
    // This method is called when the image buffer is modified,
    // we need to retrieve the TF here if it came from the image.
    const auto layer           = this->getLayer();
    const auto* const viewport = layer->getViewport();
    auto* const camera         = layer->getDefaultCamera();
    auto* const cam_node       = camera->getParentNode();

    const auto vp_width  = static_cast<float>(viewport->getActualWidth());
    const auto vp_height = static_cast<float>(viewport->getActualHeight());
    SIGHT_ASSERT("Width and height should be strictly positive", vp_width > 0 && vp_height > 0);
    const float vp_ratio = vp_width / vp_height;
    camera->setAspectRatio(vp_ratio);

    // HACK: Temporarily set the near clip distance here because the layer doesn't handle orthographic cameras.
    camera->setNearClipDistance(1e-3F);

    cam_node->setPosition(Ogre::Vector3::ZERO);
    cam_node->resetOrientation();

    const auto image   = m_image.const_lock();
    const auto origin  = image->getOrigin();
    const auto size    = image->size();
    const auto spacing = image->getSpacing();

    if(size[0] > 0 && size[1] > 0 && size[2] > 0)
    {
        float ratio   = 0;
        double width  = 0.;
        double height = 0.;
        switch(m_currentNegatoOrientation)
        {
            case Orientation::X_AXIS:
                cam_node->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(-90.F));
                cam_node->rotate(Ogre::Vector3::UNIT_Z, Ogre::Degree(-90.F));
                width  = static_cast<double>(size[1]) * spacing[1];
                height = static_cast<double>(size[2]) * spacing[2];
                ratio  = static_cast<float>(width / height);
                break;

            case Orientation::Y_AXIS:
                cam_node->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(90.F));
                width  = static_cast<double>(size[0]) * spacing[0];
                height = static_cast<double>(size[2]) * spacing[2];
                ratio  = static_cast<float>(width / height);
                break;

            case Orientation::Z_AXIS:
                cam_node->rotate(Ogre::Vector3::UNIT_Z, Ogre::Degree(180.F));
                cam_node->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(180.F));
                width  = static_cast<double>(size[0]) * spacing[0];
                height = static_cast<double>(size[1]) * spacing[1];
                ratio  = static_cast<float>(width / height);
                break;
        }

        if(vp_ratio > ratio)
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
        Ogre::Vector3 cam_pos(
            static_cast<Ogre::Real>(origin[0] + static_cast<double>(size[0]) * spacing[0] * 0.5),
            static_cast<Ogre::Real>(origin[1] + static_cast<double>(size[1]) * spacing[1] * 0.5),
            static_cast<Ogre::Real>(origin[2] + static_cast<double>(size[2]) * spacing[2] * 0.5)
        );

        cam_pos[orientation] =
            static_cast<Ogre::Real>(origin[orientation] - static_cast<double>(size[orientation])
                                    * spacing[orientation]);
        cam_node->setPosition(cam_pos);

        m_hasMoved = false;
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void negato2d_camera::resizeViewport()
{
    if(!m_hasMoved)
    {
        this->resetCamera();
    }
}

//-----------------------------------------------------------------------------

void negato2d_camera::changeOrientation(int _from, int _to)
{
    const auto to_orientation   = static_cast<Orientation>(_to);
    const auto from_orientation = static_cast<Orientation>(_from);

    if(m_currentNegatoOrientation == to_orientation)
    {
        m_currentNegatoOrientation = from_orientation;
        this->resetCamera();
    }
    else if(m_currentNegatoOrientation == from_orientation)
    {
        m_currentNegatoOrientation = to_orientation;
        this->resetCamera();
    }
}

//------------------------------------------------------------------------------

void negato2d_camera::updateWindowing(double _dw, double _dl)
{
    const double new_window = m_initialWindow + _dw;
    const double new_level  = m_initialLevel - _dl;

    {
        const auto image = m_image.const_lock();
        const auto tf    = m_tf.lock();

        tf->setWindow(new_window);
        tf->setLevel(new_level);
        const auto sig = tf->template signal<data::transfer_function::windowing_modified_signal_t>(
            data::transfer_function::WINDOWING_MODIFIED_SIG
        );
        {
            sig->async_emit(new_window, new_level);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
