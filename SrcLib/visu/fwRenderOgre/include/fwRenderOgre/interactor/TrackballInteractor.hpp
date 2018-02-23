/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

    /// No key press event handling needed here
    FWRENDEROGRE_API virtual void keyPressEvent(int) override;

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
