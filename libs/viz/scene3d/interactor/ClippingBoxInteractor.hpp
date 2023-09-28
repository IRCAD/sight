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

#pragma once

#include "viz/scene3d/interactor/base.hpp"
#include "viz/scene3d/widget/ClippingBox.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <OGRE/OgreMovableObject.h>

namespace sight::viz::scene3d::interactor
{

/// Picks VR widgets and updates clipping cube.
class VIZ_SCENE3D_CLASS_API ClippingBoxInteractor final : public base
{
public:

    /// Creates the widget.
    VIZ_SCENE3D_API ClippingBoxInteractor(
        SPTR(Layer)_layer,
        bool _layerOrderDependant,
        const std::string& _id,
        Ogre::SceneNode* _parentSceneNode,
        const Ogre::Matrix4& _clippingMatrix,
        const widget::ClippingBox::ClippingUpdateCallbackType& _clippingUpdateCb,
        const std::string& _boxMtlName,
        const std::string& _handleMtlName
    ) noexcept;

    /// Destroys the widget.
    VIZ_SCENE3D_API ~ClippingBoxInteractor() noexcept final = default;

    /// Interacts with the widget if it was previously picked, behaves like a trackball otherwise.
    VIZ_SCENE3D_API void mouseMoveEvent(
        MouseButton /*button*/,
        Modifier /*_mods*/,
        int /*x*/,
        int /*y*/,
        int /*dx*/,
        int /*dy*/
    ) final;

    /// Ends all interactions with the widget.
    VIZ_SCENE3D_API void buttonReleaseEvent(
        MouseButton /*_button*/,
        Modifier /*_mods*/,
        int /*_x*/,
        int /*_y*/
    ) final;

    /// Picks the object at the (x,y) position on a left click, scales or translates the widget otherwise.
    VIZ_SCENE3D_API void buttonPressEvent(MouseButton /*button*/, Modifier /*_mods*/, int /*x*/, int /*y*/) final;

    /// Scales the widget.
    /// @param _scalingFactor distance of the fingers
    /// @param _centerX the width coordinate of the center of the pinch
    /// @param _centerY the height coordinate of the center of the pinch
    VIZ_SCENE3D_API void pinchGestureEvent(double _scaleFactor, int _centerX, int _centerY) final;

    /// Ends all interactions with the widget.
    /// @param _x current width coordinate.
    /// @param _y current height coordinate.
    /// @param _dx the width displacement since the last event.
    /// @param _dy the height displacement since the last event.
    VIZ_SCENE3D_API void panGestureMoveEvent(int _x, int _y, int _dx, int _dy) final;

    /**
     * @brief Listens to pan gesture release events.
     * @param _x the last width coordinate of the finger
     * @param _y the last height coordinate of the finger
     * @param _dx width displacement of the finger since the last event.
     * @param _dy height displacement of the finger since the last event.
     */
    VIZ_SCENE3D_API void panGestureReleaseEvent(int _x, int _y, int _dx, int _dy) final;

    /// Sets the visibility of the box widget.
    VIZ_SCENE3D_API void setBoxVisibility(bool _visibility);

    /// Retrieves the positions of the box in model space.
    [[nodiscard]] VIZ_SCENE3D_API Ogre::AxisAlignedBox getClippingBox() const;

    /// Retrieves the current clipping transform.
    [[nodiscard]] VIZ_SCENE3D_API Ogre::Matrix4 getClippingTransform() const;

    /// Sets the current clipping box positions from the input transform.
    VIZ_SCENE3D_API void updateFromTransform(const Ogre::Matrix4& _clippingTrf);

private:

    /// Cancels further interaction on the layer.
    void cancelFurtherLayerInteractions();

    /// Containst the currently selected widget.
    Ogre::MovableObject* m_pickedObject {nullptr};

    /// Defines the widget with whom we interact.
    widget::ClippingBox m_widget;

    /// Attempts to pick the first object at screen coordinates (x, y), returns nullptr if no object was picked.
    Ogre::MovableObject* pickObject(int x, int y);
};

} // namespace sight::viz::scene3d::interactor
