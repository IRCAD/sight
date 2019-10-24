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
 * - \b newImage() : update the image display to show the new content.
 * - \b sliceType(int, int) : update image slice index .
 * - \b sliceIndex(int, int, int) : update image slice type.
 * - \b updateVisibility() : updates the negato visibility from the image field.
 * - \b setVisibility(bool) : sets the image visibility fields.
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
 */
class VISUOGREADAPTOR_CLASS_API SNegato3D : public ::fwRenderOgre::IAdaptor,
                                            public ::fwRenderOgre::ITransformable,
                                            public ::fwRenderOgre::interactor::IInteractor
{
public:

    typedef ::fwDataTools::helper::MedicalImage::Orientation OrientationMode;

    fwCoreServiceMacro(SNegato3D, ::fwRenderOgre::IAdaptor)

    /// Constructor
    VISUOGREADAPTOR_API SNegato3D() noexcept;
    /// Destructor. Does nothing.
    VISUOGREADAPTOR_API virtual ~SNegato3D() noexcept;
    /// Sets the filtering type
    VISUOGREADAPTOR_API void setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering );

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_NEWIMAGE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SLICETYPE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SLICEINDEX_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_OPACITY_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_VISIBILITY_SLOT;
    ///@}

protected:

    /// Configures the service
    VISUOGREADAPTOR_API virtual void configuring() override;
    /// Starts the service,
    VISUOGREADAPTOR_API virtual void starting() override;
    /// Stops the service, disconnects connections.
    VISUOGREADAPTOR_API virtual void stopping() override;
    /// Requests rendering of the scene.
    VISUOGREADAPTOR_API virtual void updating() override;

    /// Retrieves the current transfer function
    VISUOGREADAPTOR_API void swapping(const KeyType& key) override;

    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Slot: update the displayed transfer function
    VISUOGREADAPTOR_API virtual void updateTF();

private:

    /** Interacts with the negato if it was picked by pressing any mouse button.
     * Interactions will take place while holding down the button. The following actions are available:
     * - Left mouse click: shows a cross widget to select a voxel and retrieve its intensity.
     * - Middle mouse click: move the slice's intersection to where the mouse cursor is.
     * - Right mouse click: adjust the transfer function level and window by moving
     *                      the mouse up/down and left/right respectively.
     */
    virtual void mouseMoveEvent(MouseButton button, int _x, int _y, int _dx, int _dy) final;

    /// Attempts to pick the negato and starts interactions if picking was successful.
    virtual void buttonPressEvent(MouseButton _button, int _x, int _y) final;

    /// Ends all interactions, regardless of the input.
    virtual void buttonReleaseEvent(MouseButton, int, int ) final;

    /** @brief Unused ::fwRenderOgre::interactor::IInteractor API.
     * @{
     */
    virtual void wheelEvent(int, int, int) final;
    virtual void resizeEvent(int, int) final;
    virtual void keyPressEvent(int) final;
    virtual void keyReleaseEvent(int) final;
    virtual void focusInEvent() final;
    virtual void focusOutEvent() final;
    /**@} */

    /// Sets the slice intersection at the (_x, _y) screen position if possible.
    void moveSlices(int _x, int _y);

    /// Picks the intensity value at the (_x, _y) screen position.
    void pickIntensity(int _x, int _y);

    /// Updates the transfer function window and level by adding the input values.
    void updateWindowing(double _windowDelta, double _levelDelta);

    /// Slot: update image buffer
    void newImage();

    /// Slot: update image slice type
    void changeSliceType(int _from, int _to);

    /// Slot: update image slice index
    void changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex);

    /// Makes the planes process their meshes
    void createPlanes(const ::fwData::Image::SpacingType& _spacing, const fwData::Image::OriginType& _origin);

    /// Sets the planes's opacity
    /// Also a slot called when image opacity is modified
    void setPlanesOpacity();

    /// Slot: sets the negato's visibility
    void setVisibility(bool visibility);

    /// Sets the picking flags on all three negato planes.
    void setPlanesQueryFlags(std::uint32_t _flags);

    /// Attemps to pick the negato planes, returns the world space position of the intersection if successful.
    std::optional< ::Ogre::Vector3 > getPickedSlices(int _x, int _y);

    /// Updates the intensity picking widget's position.
    void updatePickingCross(const ::Ogre::Vector3& _pickedPos);

    /// Sets whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera { true };

    /// Sets the opacity to that of the transfer function.
    bool m_enableAlpha {false};

    /// Sets whether or not interaction are enables on the negato.
    bool m_interactive { false };

    /// Ogre texture which will be displayed on the negato
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view)
    std::unique_ptr< ::fwRenderOgre::TransferFunction> m_gpuTF;

    /// Stores the planes on which we will apply our texture
    std::array< ::fwRenderOgre::Plane::sptr, 3> m_planes;

    /// Points to the plane that the user is currently interacting with.
    ::fwRenderOgre::Plane::sptr m_pickedPlane;

    /// Widget used to pick intensities.
    ::Ogre::ManualObject* m_pickingCross { nullptr };

    /// The scene node allowing to move the entire negato
    ::Ogre::SceneNode* m_negatoSceneNode { nullptr };

    /// Defines the filtering type for this negato
    ::fwRenderOgre::Plane::FilteringEnumType m_filtering { ::fwRenderOgre::Plane::FilteringEnumType::NONE };

    /// Helps interfacing with the transfer function input.
    ::fwDataTools::helper::TransferFunction m_helperTF;

    /// Stores the transfer function window value at the time the interaction started.
    double m_initialWindow { 0.f };

    /// Stores the transfer function level value at the time the interaction started.
    double m_initialLevel { 0.f };

    /// Stores the mouse position at the time the windowing interaction started.
    ::Ogre::Vector2i m_initialPos { -1, -1 };

    using PickedVoxelSigType = ::fwCom::Signal< void (std::string) >;
    /// Sent when a voxel is picked using the left mouse button.
    PickedVoxelSigType::sptr m_pickedVoxelSignal;
};

//------------------------------------------------------------------------------
// Inline functions

inline void SNegato3D::setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering )
{
    m_filtering = _filtering;
}

//------------------------------------------------------------------------------

} // visuOgreAdaptor
