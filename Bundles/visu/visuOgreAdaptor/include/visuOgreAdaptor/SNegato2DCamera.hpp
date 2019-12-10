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

#include "visuOgreAdaptor/config.hpp"

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/interactor/TrackballInteractor.hpp>

#include <memory>

namespace visuOgreAdaptor
{

/**
 * @brief Lets the user move an orthographic camera to visualize medical images in 2D.
 *
 * The camera can moved along its screen plane and zoomed on a precise scene location.
 * For more convenience this adaptor can make use of an optional input image to reset the camera's orientation
 * when the image's slice type changes.
 * Although this service was designed with negato visualization in mind, it could be used to render regular scenes
 * from a 2D orthographic perspective.
 *
 * @warning may not work as intended when used with another camera adaptor on the same layer.
 *
 * @section Slots Slots
 * - \b resetCamera(): zooms out the camera to see the whole scene.
 * - \b changeOrientation(int,int): sets the camera's orientation to one of the image's axes.
 * - \b moveBack(): moves the camera backwards outside the scene's bounding box.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::visuOgreAdaptor::SNegato2DCamera">
            <config layer="..." priority="0" orientation="sagittal"/>
            <in key="image" uid="..." autoConnect="yes"/>
       </service>
   @endcode
 *
 * @subsection Inputs Inputs
 * - \b image [::fwData::Image](optional): image viewed in negato mode, used for auto connections only.
 * Modification signals can be used to reset the camera's position and orientation. Useless without autoConnect="yes".
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): layer on which the negato camera interactions are added.
 * - \b priority (optional, int, default=0): interaction priority, higher priority interactions are performed first.
 * - \b orientation (optional, sagittal/frontal/axial, default=sagittal): the camera's orientation at start.
 */
class VISUOGREADAPTOR_CLASS_API SNegato2DCamera final : public ::fwRenderOgre::IAdaptor,
                                                        public ::fwRenderOgre::interactor::IInteractor
{

public:

    fwCoreServiceMacro(SNegato2DCamera, ::fwRenderOgre::IAdaptor)

    /// Constructor.
    VISUOGREADAPTOR_API SNegato2DCamera() noexcept;

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SNegato2DCamera() noexcept final;

private:

    using Orientation = ::fwDataTools::helper::MedicalImage::Orientation;

    /// Configures the layer, interaction priority and camera orientation.
    virtual void configuring() final;

    /// Adds negato camera interactions interactions to the layer.
    virtual void starting() final;

    /// Updates the service. Unused here.
    virtual void updating() noexcept final;

    /// Removes negato camera interactions from the layer.
    virtual void stopping() final;

    /// Retuns proposals to connect the images signals to camera actions:
    /// - change the camera's orientation when the image's slice type changes
    /// - reset the camera's position when the image is modified.
    virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const final;

    /// Zooms in the scene at the current cursor position.
    virtual void wheelEvent(Modifier, int delta, int mouseX, int mouseY) final;

    /// Moves the camera along the projection plane.
    virtual void mouseMoveEvent(IInteractor::MouseButton button, Modifier, int, int, int dx, int dy) final;

    /// Verifies if the button is pressed within the camera's viewport and enables mouse movements if that is the case.
    virtual void buttonPressEvent(IInteractor::MouseButton button, Modifier, int, int) final;

    /// Disables mouse movements.
    virtual void buttonReleaseEvent(IInteractor::MouseButton button, Modifier, int, int) final;

    /// Resets the camera when the 'R' key is pressed.
    virtual void keyPressEvent(int key, Modifier) final;

    /// Resets the camera's zoom.
    void resetCamera();

    /// Sets the camera's orientation to one of the image's axes.
    void changeOrientation(int, int _to);

    /// Moves the camera backwards outside the scene's bounding box.
    void moveBack();

    bool m_moveCamera { false };

    /// Current image orientation.
    Orientation m_currentNegatoOrientation {  ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS };

    /// Interaction priority.
    int m_priority { 0 };

};

} // namespace visuOgreAdaptor
