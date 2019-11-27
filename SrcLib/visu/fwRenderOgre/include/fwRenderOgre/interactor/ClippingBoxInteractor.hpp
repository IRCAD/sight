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

#pragma once

#include "fwRenderOgre/interactor/IInteractor.hpp"
#include "fwRenderOgre/picker/IPicker.hpp"
#include "fwRenderOgre/widget/ClippingBox.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <OGRE/OgreMovableObject.h>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @brief Picks VR widgets and updates clipping cube.
 */
class FWRENDEROGRE_CLASS_API ClippingBoxInteractor : public IInteractor
{
public:

    /// Constructor.
    FWRENDEROGRE_API ClippingBoxInteractor(SPTR(Layer)_layer,
                                           const std::string& _id,
                                           ::Ogre::SceneNode* _parentSceneNode,
                                           const ::Ogre::Matrix4& _clippingMatrix,
                                           const widget::ClippingBox::ClippingUpdateCallbackType& _clippingUpdateCb,
                                           const std::string& _boxMtlName,
                                           const std::string& _handleMtlName) noexcept;

    /// Destructor.
    FWRENDEROGRE_API virtual ~ClippingBoxInteractor() noexcept final;

    /// Interacts with the widget if it was previously picked, behaves like a trackball otherwise.
    FWRENDEROGRE_API virtual void mouseMoveEvent(MouseButton, int, int, int, int) final;

    /// Ends all interactions with the widget.
    FWRENDEROGRE_API virtual void buttonReleaseEvent(MouseButton, int, int) final;

    /// Picks the object at the (x,y) position on a left click, scales or translates the widget otherwise.
    FWRENDEROGRE_API virtual void buttonPressEvent(MouseButton, int, int) final;

    /// Sets the visibility of the box widget.
    FWRENDEROGRE_API void setBoxVisibility(bool _visibility);

    /// Retrieves the positions of the box in model space.
    FWRENDEROGRE_API ::Ogre::AxisAlignedBox getClippingBox() const;

    /// Retrieves the current clipping transform.
    FWRENDEROGRE_API ::Ogre::Matrix4 getClippingTransform() const;

    /// Sets the current clipping box positions from the input transform.
    FWRENDEROGRE_API void updateFromTransform(const ::Ogre::Matrix4& _clippingTrf);

    /** Unused IInteractor API
     *@{
     */
    FWRENDEROGRE_API virtual void wheelEvent(int, int, int) noexcept final;
    FWRENDEROGRE_API virtual void resizeEvent(int, int) noexcept final;
    FWRENDEROGRE_API virtual void keyPressEvent(int) noexcept final;
    FWRENDEROGRE_API virtual void keyReleaseEvent(int) noexcept final;
    FWRENDEROGRE_API virtual void focusInEvent() noexcept final;
    FWRENDEROGRE_API virtual void focusOutEvent() noexcept final;
    /**
     *@}
     */

private:

    void cancelFurtherLayerInteractions();

    /// Currently selected widget.
    ::Ogre::MovableObject* m_pickedObject { nullptr };

    /// The widget with whom we interact.
    widget::ClippingBox m_widget;

    /// The picker used by this interactor.
    ::fwRenderOgre::picker::IPicker m_picker;

    /// Attempts to pick the first object at screen coordinates (x, y), returns nullptr if no object was picked.
    ::Ogre::MovableObject* pickObject(int x, int y);

};
} // namespace interactor
} // namespace fwRenderOgre
