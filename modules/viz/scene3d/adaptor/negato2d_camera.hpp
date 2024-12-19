/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
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

#pragma once

#include <data/helper/medical_image.hpp>

#include <viz/scene3d/adaptor.hpp>

#include <memory>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor lets the user move an orthographic camera to visualize medical images in 2D.
 *
 * The camera can be moved along its screen plane and zoom on a precise scene location.
 * For more convenience this adaptor can make use of an optional input image to reset the camera's orientation
 * when the image's slice type changes.
 * Although this service was designed with negato visualization in mind, it could be used to render regular scenes
 * from a 2D orthographic perspective.
 *
 * @warning may not work as intended when used with another camera adaptor on the same layer and must be started
 * after all others one.
 *
 * @section Slots Slots
 * - \b reset_camera(): zooms out the camera to see the whole scene.
 * - \b changeOrientation(int,int): sets the camera's orientation to one of the image's axes.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::negato2d_camera" >
        <inout key="image" uid="..." auto_connect="true" />
        <inout key="tf" uid="..." />
        <config priority="0" layerOrderDependant="true" orientation="sagittal" margin="0.1" block_width_scaling="true"/>
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::image](mandatory): image viewed in negato mode, used for auto connections only.
 *      Modification signals can be used to reset the camera's position and orientation. Useless without
 *      auto_connect="true".
 * - \b tf [sight::data::transfer_function] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration:
 * - \b priority (optional, int, default=0): interaction priority, higher priority interactions are performed first.
 * - \b layerOrderDependant (optional, bool, default=true): define if interaction must take into account above layers.
 * - \b orientation (optional, sagittal/frontal/axial, default=sagittal): the camera's orientation at start.
 * - \b margin (optional, default=0.1): margin to the border of the viewport, in percentage of the highest of width
 *  or height.
 * - \b scale (optional, default=false):whether to scale the orthographic plane to the viewport or
 *      keep it at its original resolution.
 * - \b interactive (optional, default=true), enable or disable interactions.
 */
class negato2d_camera final : public sight::viz::scene3d::adaptor,
                              public sight::viz::scene3d::interactor::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(negato2d_camera, sight::viz::scene3d::adaptor);

    /// Creates the service and initializes slots.
    negato2d_camera() noexcept;

    /// Destroys the adaptor.
    ~negato2d_camera() noexcept override = default;

    /**
     * @brief Moving along slices (SHIFT to speed-up) or Zooms in the scene at the current cursor position.
     * @param _delta distance that the wheel is rotated.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void wheel_event(modifier /*_modifier*/, double _delta, int _x, int _y) final;

    /**
     * @brief Zooms in the scene at the current cursor position.
     * @param _scale_factor distance of the fingers
     * @param _center_x the width coordinate of the center of the pinch
     * @param _center_y the height coordinate of the center of the pinch
     */
    void pinch_gesture_event(double _scale_factor, int _center_x, int _center_y) final;

    /**
     * @brief Moves the camera along the projection plane.
     * @param _x the last width coordinate of the finger
     * @param _y the last height coordinate of the finger
     * @param _dx width displacement of the finger since the last event.
     * @param _dy height displacement of the finger since the last event.
     */
    void pan_gesture_move_event(int _x, int _y, int _dx, int _dy) final;

    /**
     * @brief Ends Moving the camera along the projection plane.
     * @param _x the last width coordinate of the finger
     * @param _y the last height coordinate of the finger
     * @param _dx width displacement of the finger since the last event.
     * @param _dy height displacement of the finger since the last event.
     */
    void pan_gesture_release_event(int _x, int _y, int _dx, int _dy) final;

    /**
     * @brief Interacts with the negato if it was picked by pressing any mouse button.
     *
     * Interactions will take place while holding down the button. The following actions are available:
     * - Middle mouse click: moves the camera along the projection plane.
     * - Right mouse click: adjust the transfer function level and window by moving
     *                      the mouse up/down and left/right respectively.
     *
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void mouse_move_event(
        sight::viz::scene3d::interactor::base::mouse_button _button,
        modifier /*_mods*/,
        int _x,
        int _y,
        int _dx,
        int _dy
    ) final;

    /**
     * @brief Verifies if the button is pressed within the camera's viewport and enables mouse movements if that is the
     * case.
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void button_press_event(
        sight::viz::scene3d::interactor::base::mouse_button _button,
        modifier /*_mods*/,
        int _x,
        int _y
    ) final;

    /**
     * @brief Disables mouse movements.
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void button_release_event(
        sight::viz::scene3d::interactor::base::mouse_button _button,
        modifier /*_mods*/,
        int _x,
        int _y
    ) final;

    /**
     * @brief Resets the camera when the 'R' key is pressed.
     * @param _key key button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void key_press_event(int _key, modifier /*_mods*/, int _x, int _y) final;

protected:

    /// Configures the layer, interaction priority and camera orientation.
    void configuring() final;

    /// Adds negato camera interactions to the layer.
    void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::MODIFIED_SIG of s_IMAGE_INPUT to RESET_CAMERA_SLOT
     * Connect data::image::SLICE_TYPE_MODIFIED_SIG of s_IMAGE_INPUT to CHANGE_ORIENTATION_SLOT
     * Connect data::image::SLICE_INDEX_MODIFIED_SIG of s_IMAGE_INPUT to MOVE_BACK_SLOT
     */
    service::connections_t auto_connections() const final;

    /// Does nothing.
    void updating() noexcept final;

    /// Removes negato camera interactions from the layer.
    void stopping() final;

private:

    using axis_t = data::helper::medical_image::axis_t;

    /// SLOT: resets the camera's zoom.
    void reset_camera();

    /// SLOT: resets the display when resizing.
    void resize_viewport();

    /**
     * @brief SLOT: sets the camera's orientation to one of the image's axes.
     * @param _from origin of the orientation.
     * @param _to destination of the orientation.
     */
    void change_orientation(int _from, int _to);

    /// Updates the transfer function window and level by adding the input values.
    void update_windowing(double _dw, double _dl);

    /// Get the inverse transformation matrix from camera to origin
    Ogre::Matrix4 get_camera_to_origin_transform() const;

    /// Defines the current interaction status.
    bool m_is_interacting {false};

    /// Defines the image current orientation.
    axis_t m_axis {axis_t::z_axis};

    /// Defines the interaction priority.
    int m_priority {0};

    /// Defines the transfer function window value at the time the interaction started.
    double m_initial_window {0.F};

    /// Defines the transfer function level value at the time the interaction started.
    double m_initial_level {0.F};

    /// Defines the mouse position at the time the windowing interaction started.
    Ogre::Vector2i m_initial_pos {-1, -1};

    /// Defines the margin to the border of the viewport.
    float m_margin {0.1F};

    /// This allows us to reset the camera when Qt refreshes the size of the viewport after the start of the adaptor
    bool m_has_moved {false};

    /// Enable/disable scaling (like video adaptor).
    bool m_scale {false};

    /// Enable/disable interactions.
    bool m_is_interactive {true};

    /// Handles connection with the layer.
    core::com::helper::sig_slot_connection m_layer_connection;

    static constexpr std::string_view IMAGE_INOUT = "image";
    static constexpr std::string_view TF_INOUT    = "tf";

    sight::data::ptr<sight::data::image, sight::data::access::inout> m_image {this, IMAGE_INOUT};
    sight::data::ptr<sight::data::transfer_function, sight::data::access::inout> m_tf {this, TF_INOUT, true};
};

} // namespace sight::module::viz::scene3d::adaptor.
