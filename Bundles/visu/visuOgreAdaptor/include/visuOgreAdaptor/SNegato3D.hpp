/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Float.hpp>

#include <fwDataTools/helper/TransferFunction.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/Plane.hpp>
#include <fwRenderOgre/TransferFunction.hpp>

#include <OGRE/OgreManualObject.h>

#include <optional>

namespace visuOgreAdaptor
{

/**
 * @brief   Adaptor to display a 3D negato.
 *
 * @section Signals Signals
 * - \b pickedVoxel(string): sends the coordinates and intensity of the voxel picked by the cross widget.
 *
 * @section Slots Slots
 * - \b newImage(): update the image display to show the new content.
 * - \b sliceType(int, int): update image slice index .
 * - \b sliceIndex(int, int, int): update image slice type.
 * - \b updateOpacity(): sets the planes's opacity.
 * - \b updateVisibility(): updates the negato visibility from the image field.
 * - \b setVisibility(bool): sets the image visibility fields.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::visuOgreAdaptor::SNegato3D">
            <inout key="image" uid="..." />
            <inout key="tf" uid="..." optional="yes" />
            <config layer="default" sliceIndex="axial" filtering="none" tfalpha="true" />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b image [::fwData::Image]: image to display.
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): id of the layer where this adaptor applies.
 * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
 * - \b filtering (optional, none/linear/anisotropic, default=none): texture filter type of the negato
 * - \b tfalpha (optional, true/false, default=false): if true, the alpha channel of the transfer function is used
 * - \b interactive (optional, true/false, default=false): enables interactions on the negato
 * - \b priority (optional, int, default=1): interaction priority of the negato.
 * - \b transform (optional): the name of the Ogre transform node where to attach the negato, as it was specified
 * in the STransform adaptor.
 * - \b queryFlags (optional, default=0x40000000): Mask set to planes for picking request.
 */
class VISUOGREADAPTOR_CLASS_API SNegato3D final : public ::fwRenderOgre::IAdaptor,
                                                  public ::fwRenderOgre::ITransformable,
                                                  public ::fwRenderOgre::interactor::IInteractor
{
public:

    typedef ::fwDataTools::helper::MedicalImage::Orientation OrientationMode;

    fwCoreServiceMacro(SNegato3D, ::fwRenderOgre::IAdaptor)

    /// Creates slots.
    VISUOGREADAPTOR_API SNegato3D() noexcept;

    /// Destroys the service.
    VISUOGREADAPTOR_API virtual ~SNegato3D() noexcept;

    /**
     * @brief Sets the filtering type.
     * @param _filtering the filtering type.
     */
    VISUOGREADAPTOR_API void setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering );

private:

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_IMAGE_INOUT to ::visuOgreAdaptor::SNegato3D::s_NEWIMAGE_SLOT
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_BUFFER_MODIFIED_SIG to ::visuOgreAdaptor::SNegato3D::s_NEWIMAGE_SLOT
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_SLICE_TYPE_MODIFIED_SIG to
     *::visuOgreAdaptor::SNegato3D::s_SLICETYPE_SLOT
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_SLICE_INDEX_MODIFIED_SIG to
     *::visuOgreAdaptor::SNegato3D::s_SLICEINDEX_SLOT
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_VISIBILITY_MODIFIED_SIG to
     *::visuOgreAdaptor::SNegato3D::s_UPDATE_VISIBILITY_SLOT
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_TRANSPARENCY_MODIFIED_SIG to
     *::visuOgreAdaptor::SNegato3D::s_UPDATE_VISIBILITY_SLOT
     */
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Configures the service.
    virtual void configuring() override;

    /// Starts the service.
    virtual void starting() override;

    /// Requests rendering of the scene.
    virtual void updating() override;

    /**
     * @brief Notifies that the TF is swapped.
     * @param _key key of the swapped data.
     */
    void swapping(const KeyType& key) override;

    /// Stops the service, disconnects connections.
    virtual void stopping() override;

    /// Update the displayed transfer function.
    void updateTF();

    /**
     * @brief Interacts with the negato if it was picked by pressing any mouse button.
     *
     * Interactions will take place while holding down the button. The following actions are available:
     * - Left mouse click: shows a cross widget to select a voxel and retrieve its intensity.
     * - Middle mouse click: move the slice's intersection to where the mouse cursor is.
     * - Right mouse click: adjust the transfer function level and window by moving
     *                      the mouse up/down and left/right respectively.
     *
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     * @param _dx the cursor's width displacement since the last event.
     * @param _dy the cursor's height displacement since the last event.
     */
    virtual void mouseMoveEvent(MouseButton _button, Modifier, int _x, int _y, int _dx, int _dy) override;

    /**
     * @brief Attempts to pick the negato and starts interactions if picking was successful.
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    virtual void buttonPressEvent(MouseButton _button, Modifier, int _x, int _y) override;

    /// Ends all interactions, regardless of the input.
    virtual void buttonReleaseEvent(MouseButton, Modifier, int, int) override;

    /**
     * @brief Sets the slice intersection at the (_x, _y) screen position if possible.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void moveSlices(int _x, int _y);

    /**
     * @brief Picks the intensity value at the (_x, _y) screen position.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void pickIntensity(int _x, int _y);

    /**
     * @brief Updates the transfer function window and level by adding the input values.
     * @param _dw window delta.
     * @param _dl level delta.
     */
    void updateWindowing(double _dw, double _dl);

    /// SLOT: updates the image buffer.
    void newImage();

    /// SLOT: updates the image slice type.
    void changeSliceType(int, int);

    /**
     * @brief SLOT: updates the image slice index.
     * @param _axialIndex new axial slice index.
     * @param _frontalIndex new frontal slice index.
     * @param _sagittalIndex new sagittal slice index.
     */
    void changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex);

    /**
     * @brief Makes the planes process their meshes.
     * @param _spacing spacing of the input image.
     * @param _origin origin of the input image.
     */
    void createPlanes(const ::Ogre::Vector3& _spacing, const ::Ogre::Vector3& _origin);

    /// SLOT: sets the planes's opacity.
    void setPlanesOpacity();

    /**
     * @brief SLOT: sets the negato's visibility.
     * @param _visibility visibility status of the negato.
     */
    void setVisibility(bool _visibility);

    /// Sets the picking flags on all three negato planes.
    void setPlanesQueryFlags(std::uint32_t _flags);

    /// Attemps to pick the negato planes, returns the world space position of the intersection if successful.
    std::optional< ::Ogre::Vector3 > getPickedSlices(int _x, int _y);

    /// Updates the intensity picking widget's position.
    void updatePickingCross(const ::Ogre::Vector3& _pickedPos, const ::Ogre::Vector3& _imgOrigin);

    /// Sets whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera { true };

    /// Sets the opacity to that of the transfer function.
    bool m_enableAlpha { false };

    /// Sets whether or not interactions are enabled on the negato.
    bool m_interactive { false };

    /// Defines the order in which interactions take place in the scene.
    int m_interactionPriority { 1 };

    /// Contains the ogre texture which will be displayed on the negato.
    ::Ogre::TexturePtr m_3DOgreTexture { nullptr };

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view).
    std::unique_ptr< ::fwRenderOgre::TransferFunction> m_gpuTF { nullptr };

    /// Stores the planes on which we will apply our texture.
    std::array< ::fwRenderOgre::Plane::sptr, 3> m_planes;

    /// Contains the plane that the user is currently interacting with.
    ::fwRenderOgre::Plane::sptr m_pickedPlane { nullptr };

    /// Contains the widget displayed to pick intensities.
    ::Ogre::ManualObject* m_pickingCross { nullptr };

    /// Contains the scene node allowing to move the entire negato.
    ::Ogre::SceneNode* m_negatoSceneNode { nullptr };

    /// Defines the filtering type for this negato.
    ::fwRenderOgre::Plane::FilteringEnumType m_filtering { ::fwRenderOgre::Plane::FilteringEnumType::NONE };

    /// Helps interfacing with the transfer function input.
    ::fwDataTools::helper::TransferFunction m_helperTF;

    /// Stores the transfer function window value at the time the interaction started.
    double m_initialWindow { 0.f };

    /// Stores the transfer function level value at the time the interaction started.
    double m_initialLevel { 0.f };

    /// Stores the mouse position at the time the windowing interaction started.
    ::Ogre::Vector2i m_initialPos { -1, -1 };

    /// Store the mask used for picking request.
    std::uint32_t m_queryFlags {::Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Stores the signal sent when a voxel is picked using the left mouse button.
    using PickedVoxelSigType = ::fwCom::Signal< void (std::string) >;
    PickedVoxelSigType::sptr m_pickedVoxelSignal { nullptr };

};

//------------------------------------------------------------------------------
// Inline functions

inline void SNegato3D::setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering )
{
    m_filtering = _filtering;
}

//------------------------------------------------------------------------------

} // visuOgreAdaptor
