/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_INTERACTOR_NEGATO2DINTERACTOR_HPP__
#define __FWRENDEROGRE_INTERACTOR_NEGATO2DINTERACTOR_HPP__

#include "fwRenderOgre/interactor/IMovementInteractor.hpp"
#include "fwRenderOgre/config.hpp"

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @class Negato2DInteractor
 * @brief Manages the camera movements (zoom/displacement) over a 2D negato
 */
class FWRENDEROGRE_CLASS_API Negato2DInteractor :
    public ::fwRenderOgre::interactor::IMovementInteractor
{

public:

    /// Constructor
    FWRENDEROGRE_API Negato2DInteractor();

    /// Mouse's wheel rotation callback.
    /// It will prompt the camera's scene node to zoom in or zoom out in the scene.
    FWRENDEROGRE_API virtual void wheelEvent(int delta, int mouseX, int mouseY);

    /// No mouse movement handling needed here
    FWRENDEROGRE_API virtual void mouseMoveEvent(MouseButton button, int x, int y, int dx, int dy);

    /// No resize event handling needed here
    FWRENDEROGRE_API virtual void resizeEvent(int x, int y)
    {
    }

    /// No key press event handling needed here
    FWRENDEROGRE_API virtual void keyPressEvent(int key)
    {
    }

    /// No need to handle button release events.
    FWRENDEROGRE_API virtual void buttonReleaseEvent(MouseButton button, int x, int y)
    {
    }

    /// No need to handle button press events.
    FWRENDEROGRE_API virtual void buttonPressEvent(MouseButton button, int x, int y)
    {
    }

private:

    /// Zoom related attributes. Equals the number of zooms that have been done forward.
    int m_zoomCoefficient;
    /// Zoom coefficient for every time you zoom on the picture. Must be between 0 and 1, default is 0.9
    const static float SCALE_COEF;
    /// Total coefficient that expresses the ratio between the current size of screen, compared to the total size.
    ::Ogre::Real m_zoomScale;

    /// Attributes related to the texture
    /// Minimum corner is a vector representing the Ogre coordinates of the Negato's bounding box's inferior corner
    ::Ogre::Vector3 m_minimumCorner;
    /// Maximum corner is a vector representing the Ogre coordinates of the Negato's bounding box's superior corner
    ::Ogre::Vector3 m_maximumCorner;
    /// Represents the Negato2D's total width
    ::Ogre::Real m_totalWidth;
    /// Represents the Negato2D's total height.
    ::Ogre::Real m_totalHeight;

    /// Current number of pixels from the Negato2D that are currently displayed horizontally.
    ::Ogre::Real m_currentWidth;
    /// Current number of pixels from the Negato2D that are currently displayed vertically.
    ::Ogre::Real m_currentHeight;

    /// Attributes related to the RenderWindow
    /// Qt RenderWindow's height.
    ::Ogre::Real m_renderWindowHeight;
    /// Qt RenderWindow's width.
    ::Ogre::Real m_renderWindowWidth;
    /// Qt RenderWindow's ratio.
    ::Ogre::Real m_renderWindowRatio;

    /// Stores the adress of the camera.
    ::Ogre::Camera* m_camera;
    /// Stores the X and Y components of the camera position.
    ::Ogre::Vector3 m_cameraPos;


    /// Gets the camera. makes sure that the member pointing to the Ogre camera is initialized.
    /// This cannot be done in the constructor because we can't know for sure if the Ogre context is launched.
    ::Ogre::Camera* getCamera();
    /// Updates m_totalWidth/Height according to the size of the negato plane mesh
    void updateTotalSize();
    /// Updates RenderWindow's dimensions
    void updateRenderWindowDimensions();

    /// Update camera Position.
    void updateCameraPosition();
    /// Resets camera Position.
    void resetCameraPosition();
    /// Update camera Ratio.
    void updateCameraAngle();

    /// Getters for Ogre World coordinates.
    /// Gets the Ogre coordinate in X function of an X in the QRenderWindow.
    ::Ogre::Real getTextureCoordinate_X( int mouseX );
    /// Gets the Ogre coordinate in Y function of an Y in the QRenderWindow.(Caution: Ogre & Qt's Y-axis are opposites)
    ::Ogre::Real getTextureCoordinate_Y( int mouseY );
};

}
}
#endif // __FWRENDEROGRE_INTERACTOR_NEGATO2DINTERACTOR_HPP__
