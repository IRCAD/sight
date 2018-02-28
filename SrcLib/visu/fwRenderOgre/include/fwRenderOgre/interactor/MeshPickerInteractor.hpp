/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

    // Render window's width
    int m_width;

    // Render window's height
    int m_height;

    // Control key status (pressed or not)
    bool m_control;
};
} //namespace itneractor
} //namespace fwRenderOgre
