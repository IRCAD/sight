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
#include <fwRenderOgre/interactor/IPickerInteractor.hpp>
#include <fwRenderOgre/picker/IPicker.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Point.hpp>

#include <OGRE/OgreSceneManager.h>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @brief Implementation for selection of points on Mesh
 */
class FWRENDEROGRE_CLASS_API MeshPickerInteractor : public ::fwRenderOgre::interactor::IPickerInteractor
{

public:

    /// Constructor.
    FWRENDEROGRE_API MeshPickerInteractor() noexcept;

    /// Destructor. Does nothing
    FWRENDEROGRE_API ~MeshPickerInteractor() noexcept;

    /// Behaviour on mouseMoveEvent
    FWRENDEROGRE_API virtual void mouseMoveEvent(MouseButton, int, int, int, int) override;

    /// Behaviour on a wheelEvent
    FWRENDEROGRE_API virtual void wheelEvent(int, int, int) override;

    /// Behaviour on button release.
    FWRENDEROGRE_API virtual void buttonReleaseEvent(MouseButton, int, int) override;

    /// Behaviour on button press.
    FWRENDEROGRE_API virtual void buttonPressEvent(MouseButton, int, int) override;

    /// Called when the window is resized
    FWRENDEROGRE_API virtual void resizeEvent(int, int) override;

    /// Called when a key is press
    FWRENDEROGRE_API virtual void keyPressEvent(int) override;

    /// Called when a key is release
    FWRENDEROGRE_API virtual void keyReleaseEvent(int) override;

    /// Called when the focus is win
    FWRENDEROGRE_API virtual void focusInEvent() override;

    /// Called when the focus is lost
    FWRENDEROGRE_API virtual void focusOutEvent() override;

private:

    /// Render window's width
    int m_width {0};

    /// Render window's height
    int m_height {0};

    /// Modifier key status (pressed or not)
    bool m_control {false};
};
} //namespace itneractor
} //namespace fwRenderOgre
