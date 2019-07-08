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

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/interactor/IMovementInteractor.hpp"

#include <fwDataTools/helper/MedicalImage.hpp>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @brief Manages the camera movements (zoom/displacement) over a 2D negato
 */
class FWRENDEROGRE_CLASS_API Negato2DInteractor :
    public ::fwRenderOgre::interactor::IMovementInteractor
{

public:

    /// Constructor
    FWRENDEROGRE_API Negato2DInteractor();

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

    /// Zoom related attributes. Equals the number of zooms that have been done forward.
    int m_zoomCoefficient;

    /// Zoom coefficient for every time you zoom on the picture. Must be between 0 and 1, default is 0.9
    const static float SCALE_COEF;

    /// Total coefficient that expresses the ratio between the current size of screen, compared to the total size.
    ::Ogre::Real m_zoomScale;

    /// Attributes related to the texture
    /// Minimum corner is a vector representing the Ogre coordinates of the Negato's bounding box's inferior corner
    ::Ogre::Vector2 m_minimumCorner;

    /// Maximum corner is a vector representing the Ogre coordinates of the Negato's bounding box's superior corner
    ::Ogre::Vector2 m_maximumCorner;

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

    /// Get the negato orientation based on the camera direction
    ::fwDataTools::helper::MedicalImage::Orientation getOrientation();

    /// Updates m_totalWidth/Height according to the size of the negato plane mesh
    void updateTotalSize();

    /// Updates RenderWindow's dimensions
    void updateRenderWindowDimensions();

    /// Update camera Position.
    void updateCameraPosition();

    /// Update camera Ratio.
    void updateCameraAngle();

    /// Getters for Ogre World coordinates.
    /// Gets the Ogre coordinate in X function of an X in the QRenderWindow.
    ::Ogre::Real getTextureCoordinate_X( int mouseX ) const;

    /// Gets the Ogre coordinate in Y function of an Y in the QRenderWindow.(Caution: Ogre & Qt's Y-axis are opposites)
    ::Ogre::Real getTextureCoordinate_Y( int mouseY ) const;
};

}
}
