/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_INTERACTOR_TRACKBALLINTERACTOR_HPP__
#define __FWRENDEROGRE_INTERACTOR_TRACKBALLINTERACTOR_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/interactor/IMovementInteractor.hpp"

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

    /**
     * @brief Mouse's left button press + movement callback.
     *        The camera's scene node will rotate around it's point of interest (lookAt).
     * @param dx The mouse's X displacement
     * @param dy The mouse's Y displacement
     */
    FWRENDEROGRE_API void mouseMoveEvent(MouseButton button, int x, int y, int dx, int dy);

    /**
     * @brief Mouse's wheel rotation callback.
     *        This callback will prompt the camera's scene node to zoom in or zoom out in the scene.
     * @param delta The mouse's wheel displacement
     * @param x
     * @param y
     */
    FWRENDEROGRE_API void wheelEvent(int delta, int x, int y);

    /// Not used here.
    FWRENDEROGRE_API virtual void buttonReleaseEvent(MouseButton button, int x, int y);

    /// Not used here.
    FWRENDEROGRE_API virtual void buttonPressEvent(MouseButton button, int x, int y);

    /**
     * @brief Called when the window is resized
     * @param x The width of the window
     * @param y The height of the window
     * */
    FWRENDEROGRE_API virtual void resizeEvent(int x, int y);

    /**
     * @brief Called when a key is pressed
     * @param key key pressed on the keyboard
     * */
    FWRENDEROGRE_API virtual void keyPressEvent(int key);

private:
    /**
     * @brief The camera's scene node will rotate around it's point of interest (lookAt).
     * @param dx The mouse's X displacement
     * @param dy The mouse's Y displacement
     */
    void cameraRotate(int dx, int dy);

    /**
     * @brief The camera's scene node will translate along it's local vertical and horizontal space.
     * @param dx The horizontal displacement
     * @param dy The vertical displacement
     */
    void cameraTranslate(int dx, int dy);

    /// Current width of the render window
    int m_width;
    /// Current height of the render window
    int m_height;
};

}
}
#endif // __FWRENDEROGRE_INTERACTOR_TRACKBALLINTERACTOR_HPP__
