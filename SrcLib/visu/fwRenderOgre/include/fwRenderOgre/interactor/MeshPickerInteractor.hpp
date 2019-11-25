/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include <fwRenderOgre/config.hpp>
#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/picker/IPicker.hpp>

#include <fwCom/Signal.hpp>

#include <fwData/Point.hpp>

#include <OGRE/OgreSceneManager.h>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @brief Implementation for selection of points on Mesh
 */
class FWRENDEROGRE_CLASS_API MeshPickerInteractor : public ::fwRenderOgre::interactor::IInteractor
{

public:

    /// Type of signal sent when a picking query succeeded/
    using PointClickedSigType = ::fwCom::Signal< void ( ::fwDataTools::PickingInfo ) >;

    /// Constructor. Initializes the picker.
    FWRENDEROGRE_API MeshPickerInteractor(SPTR(Layer)_layer = nullptr) noexcept;

    /// Destructor. Destroys the interactor.
    FWRENDEROGRE_API ~MeshPickerInteractor() noexcept;

    /// Runs a picking query when a mouse button is released @see MeshPickerInteractor::pick().
    FWRENDEROGRE_API virtual void buttonReleaseEvent(MouseButton _button, Modifier _mods, int _x, int _y) override;

    /// Runs a picking query when a mouse button is pressed @see MeshPickerInteractor::pick().
    FWRENDEROGRE_API virtual void buttonPressEvent(MouseButton _button, Modifier _mods, int _x, int _y) override;

    /// Called when the window is resized.
    FWRENDEROGRE_API virtual void resizeEvent(int _w, int _h) override;

    /// Sets the signal to be called when picking succeeded.
    FWRENDEROGRE_API void setPointClickedSig(const PointClickedSigType::sptr& _sig);

    /// Sets the mask to filter out entities with mismathing query flags.
    FWRENDEROGRE_API void setQueryMask(std::uint32_t _queryMask);

private:

    /**
     * @brief Triggers a picking query and sends a signal with the corresponding @see fwDataTools::PickingInfo.
     *
     * @param _button mouse button pressed.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     * @param _pressed whether the button is pressed (true) or released (false).
     */
    void pick(MouseButton _button, Modifier _mods, int _x, int _y, bool _pressed);

    /// Executes ray picking operations.
    ::fwRenderOgre::picker::IPicker m_picker;

    /// Picking query mask. Filters out objects with mismatching flags.
    std::uint32_t m_queryMask { 0xffffffff };

    /// Current width of the render window.
    int m_width {0};

    /// Current height of the render window.
    int m_height {0};

    /// Signal sent when picking succeeded.
    PointClickedSigType::sptr m_pointClickedSig;

};
} //namespace itneractor
} //namespace fwRenderOgre
