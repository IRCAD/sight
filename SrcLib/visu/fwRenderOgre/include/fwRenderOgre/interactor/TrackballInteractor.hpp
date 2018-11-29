/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/interactor/IMovementInteractor.hpp"

#include <fwThread/Timer.hpp>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @brief Trackball camera implementation
 */
class FWRENDEROGRE_CLASS_API TrackballInteractor : public ::fwRenderOgre::interactor::IMovementInteractor
{

public:

    /// Nothing done in the constructor
    FWRENDEROGRE_API TrackballInteractor();

    /// Destructor
    virtual FWRENDEROGRE_API ~TrackballInteractor();

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
    /**
     * @brief The camera's scene node will rotate around it's point of interest (lookAt).
     * @param dx The mouse's X displacement
     * @param dy The mouse's Y displacement
     */
    void cameraRotate(int, int);

    /**
     * @brief The camera's scene node will translate along it's local vertical and horizontal space.
     * @param dx The horizontal displacement
     * @param dy The vertical displacement
     */
    void cameraTranslate(int, int);

    /// Current width of the render window
    int m_width;

    /// Current height of the render window
    int m_height;

    /// Animate
    bool m_animate;

    /// Timer
    ::fwThread::Timer::sptr m_timer;
};

}
}
