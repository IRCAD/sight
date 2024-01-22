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

#include "viz/scene3d/interactor/base.hpp"
#include "viz/scene3d/widget/clipping_box.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <OGRE/OgreMovableObject.h>

namespace sight::viz::scene3d::interactor
{

/// Picks VR widgets and updates clipping cube.
class VIZ_SCENE3D_CLASS_API clipping_box_interactor final : public base
{
public:

    /// Creates the widget.
    VIZ_SCENE3D_API clipping_box_interactor(
        SPTR(layer)_layer,
        bool _layer_order_dependant,
        const std::string& _id,
        Ogre::SceneNode* _parent_scene_node,
        const Ogre::Matrix4& _clipping_matrix,
        const widget::clipping_box::clipping_update_callback_t& _clipping_update_cb,
        const std::string& _box_mtl_name,
        const std::string& _handle_mtl_name
    ) noexcept;

    /// Destroys the widget.
    VIZ_SCENE3D_API ~clipping_box_interactor() noexcept final = default;

    /// Interacts with the widget if it was previously picked, behaves like a trackball otherwise.
    VIZ_SCENE3D_API void mouse_move_event(
        mouse_button /*button*/,
        modifier /*_mods*/,
        int /*x*/,
        int /*y*/,
        int /*dx*/,
        int /*dy*/
    ) final;

    /// Ends all interactions with the widget.
    VIZ_SCENE3D_API void button_release_event(
        mouse_button /*_button*/,
        modifier /*_mods*/,
        int /*_x*/,
        int /*_y*/
    ) final;

    /// Picks the object at the (x,y) position on a left click, scales or translates the widget otherwise.
    VIZ_SCENE3D_API void button_press_event(mouse_button /*button*/, modifier /*_mods*/, int /*x*/, int /*y*/) final;

    /// Scales the widget.
    /// @param _scale_factor distance of the fingers
    /// @param _center_x the width coordinate of the center of the pinch
    /// @param _center_y the height coordinate of the center of the pinch
    VIZ_SCENE3D_API void pinch_gesture_event(double _scale_factor, int _center_x, int _center_y) final;

    /// Ends all interactions with the widget.
    /// @param _x current width coordinate.
    /// @param _y current height coordinate.
    /// @param _dx the width displacement since the last event.
    /// @param _dy the height displacement since the last event.
    VIZ_SCENE3D_API void pan_gesture_move_event(int _x, int _y, int _dx, int _dy) final;

    /**
     * @brief Listens to pan gesture release events.
     * @param _x the last width coordinate of the finger
     * @param _y the last height coordinate of the finger
     * @param _dx width displacement of the finger since the last event.
     * @param _dy height displacement of the finger since the last event.
     */
    VIZ_SCENE3D_API void pan_gesture_release_event(int _x, int _y, int _dx, int _dy) final;

    /// Sets the visibility of the box widget.
    VIZ_SCENE3D_API void set_box_visibility(bool _visibility);

    /// Retrieves the positions of the box in model space.
    [[nodiscard]] VIZ_SCENE3D_API Ogre::AxisAlignedBox get_clipping_box() const;

    /// Retrieves the current clipping transform.
    [[nodiscard]] VIZ_SCENE3D_API Ogre::Matrix4 get_clipping_transform() const;

    /// Sets the current clipping box positions from the input transform.
    VIZ_SCENE3D_API void update_from_transform(const Ogre::Matrix4& _clipping_trf);

private:

    /// Cancels further interaction on the layer.
    void cancel_further_layer_interactions();

    /// Containst the currently selected widget.
    Ogre::MovableObject* m_picked_object {nullptr};

    /// Defines the widget with whom we interact.
    widget::clipping_box m_widget;

    /// Attempts to pick the first object at screen coordinates (x, y), returns nullptr if no object was picked.
    Ogre::MovableObject* pick_object(int _x, int _y);
};

} // namespace sight::viz::scene3d::interactor
