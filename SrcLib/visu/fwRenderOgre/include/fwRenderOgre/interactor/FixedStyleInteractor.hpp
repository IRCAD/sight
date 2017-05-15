/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_INTERACTOR_FIXEDSTYLEINTERACTOR_HPP__
#define __FWRENDEROGRE_INTERACTOR_FIXEDSTYLEINTERACTOR_HPP__

#include "fwRenderOgre/config.hpp"
#include <fwRenderOgre/interactor/IMovementInteractor.hpp>

namespace fwRenderOgre
{

namespace interactor
{
/**
 * @brief Fixed camera implementation. The mouse movements callbacks do nothing
 */

class FWRENDEROGRE_CLASS_API FixedStyleInteractor : public ::fwRenderOgre::interactor::IMovementInteractor
{

public:

    /// Constructor
    FWRENDEROGRE_API FixedStyleInteractor();

    /// Behaviour on mouseMoveEvent
    FWRENDEROGRE_API void mouseMoveEvent(MouseButton button, int x, int y, int dx, int dy);
    /// Behaviour on a wheelEvent
    FWRENDEROGRE_API void wheelEvent(int delta, int x, int y);
    /// Behaviour on button release.
    FWRENDEROGRE_API virtual void buttonReleaseEvent(MouseButton button, int x, int y);
    /// Behaviour on button press.
    FWRENDEROGRE_API virtual void buttonPressEvent(MouseButton button, int x, int y);

    /**
     * @brief Behaviour on a ResizeEvent
     * @param x The width of the window
     * @param y The height of the window
     * */
    FWRENDEROGRE_API virtual void resizeEvent(int x, int y);

    /// No key press event handling needed here
    FWRENDEROGRE_API virtual void keyPressEvent(int key);
};

}
}
#endif // __FWRENDEROGRE_INTERACTOR_FIXEDSTYLEINTERACTOR_HPP__
