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

#pragma once

#include "viz/scene3d/interactor/IInteractor.hpp"
#include "viz/scene3d/widget/ClippingBox.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

#include <OGRE/OgreMovableObject.h>

namespace sight::viz::scene3d::interactor
{

/// Picks VR widgets and updates clipping cube.
class VIZ_SCENE3D_CLASS_API ClippingBoxInteractor final : public IInteractor
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
    VIZ_SCENE3D_API ~ClippingBoxInteractor() noexcept override = default;

    /// Interacts with the widget if it was previously picked, behaves like a trackball otherwise.
    VIZ_SCENE3D_API void mouseMoveEvent(
        MouseButton /*button*/,
        Modifier /*_mods*/,
        int /*x*/,
        int /*y*/,
        int /*dx*/,
        int /*dy*/
    ) override;

    /// Ends all interactions with the widget.
    VIZ_SCENE3D_API void buttonReleaseEvent(
        MouseButton /*_button*/,
        Modifier /*_mods*/,
        int /*_x*/,
        int /*_y*/
    ) override;

    /// Picks the object at the (x,y) position on a left click, scales or translates the widget otherwise.
    VIZ_SCENE3D_API void buttonPressEvent(MouseButton /*button*/, Modifier /*_mods*/, int /*x*/, int /*y*/) override;

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
