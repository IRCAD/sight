/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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
#include <fwDataTools/helper/TransferFunction.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/interactor/TrackballInteractor.hpp>

#include <memory>

namespace visuOgreAdaptor
{

/**
 * @brief Lets the user move an orthographic camera to visualize medical images in 2D.
 *
 * The camera can be moved along its screen plane and zoom on a precise scene location.
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
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image](mandatory): image viewed in negato mode, used for auto connections only.
 * Modification signals can be used to reset the camera's position and orientation. Useless without autoConnect="yes".
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
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

    /// Creates the service and initializes slots.
    VISUOGREADAPTOR_API SNegato2DCamera() noexcept;

    /// Destroyes the service.
    VISUOGREADAPTOR_API virtual ~SNegato2DCamera() noexcept final;

private:

    using Orientation = ::fwDataTools::helper::MedicalImage::Orientation;

    /// Configures the layer, interaction priority and camera orientation.
    virtual void configuring() final;

    /// Adds negato camera interactions to the layer.
    virtual void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_IMAGE_INPUT to ::visuOgreAdaptor::STransform::s_RESET_CAMERA_SLOT
     * Connect ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG of s_IMAGE_INPUT to
     * ::visuOgreAdaptor::STransform::s_CHANGE_ORIENTATION_SLOT
     * Connect ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG of s_IMAGE_INPUT to
     * ::visuOgreAdaptor::STransform::s_MOVE_BACK_SLOT
     */
    virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const final;

    /// Does nothing.
    virtual void updating() noexcept final;

    /**
     * @brief Retrieves the current transfer function.
     * @param _key key of the swapped data.
     */
    virtual void swapping(const KeyType& _key) override;

    /// Removes negato camera interactions from the layer.
    virtual void stopping() final;

    /**
     * @brief Zooms in the scene at the current cursor position.
     * @param _delta distance that the wheel is rotated.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    virtual void wheelEvent(Modifier, int _delta, int _x, int _y) final;

    /**
     * @brief Interacts with the negato if it was picked by pressing any mouse button.
     *
     * Interactions will take place while holding down the button. The following actions are available:
     * - Middle mouse click: moves the camera along the projection plane.
     * - Right mouse click: adjust the transfer function level and window by moving
     *                      the mouse up/down and left/right respectively.
     *
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    virtual void mouseMoveEvent(IInteractor::MouseButton _button, Modifier, int _x, int _y, int _dx, int _dy) final;

    /**
     * @brief Verifies if the button is pressed within the camera's viewport and enables mouse movements if that is the
     * case.
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    virtual void buttonPressEvent(IInteractor::MouseButton _button, Modifier, int _x, int _y) final;

    /**
     * @brief Disables mouse movements.
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    virtual void buttonReleaseEvent(IInteractor::MouseButton _button, Modifier, int _x, int _y) final;

    /**
     * @brief Resets the camera when the 'R' key is pressed.
     * @param _key key button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    virtual void keyPressEvent(int _key, Modifier, int _x, int _y) final;

    /// SLOT: resets the camera's zoom.
    void resetCamera();

    /**
     * @brief SLOT: sets the camera's orientation to one of the image's axes.
     * @param _from origin of the orientation.
     * @param _to destination of the orientation.
     */
    void changeOrientation(int _from, int _to);

    /// SLOT: moves the camera backwards outside the scene's bounding box.
    void moveBack();

    /// Updates the transfer function window and level by adding the input values.
    void updateWindowing(double _windowDelta, double _levelDelta);

    /// Defines the current interaction status.
    bool m_isInteracting { false };

    /// Defines the image current orientation.
    Orientation m_currentNegatoOrientation {  ::fwDataTools::helper::MedicalImage::Orientation::Z_AXIS };

    /// Sets the interaction priority.
    int m_priority { 0 };

    /// Helps interfacing with the transfer function input.
    ::fwDataTools::helper::TransferFunction m_helperTF;

    /// Sets the transfer function window value at the time the interaction started.
    double m_initialWindow { 0.f };

    /// Sets the transfer function level value at the time the interaction started.
    double m_initialLevel { 0.f };

    /// Sets the mouse position at the time the windowing interaction started.
    ::Ogre::Vector2i m_initialPos { -1, -1 };

};

} // namespace visuOgreAdaptor
