/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Float.hpp>

#include <fwDataTools/helper/TransferFunction.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/Plane.hpp>
#include <fwRenderOgre/TransferFunction.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief   Adaptor to display a 3D negato.
 * *
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
 */
class VISUOGREADAPTOR_CLASS_API SNegato3D : public ::fwRenderOgre::IAdaptor,
                                            public ::fwRenderOgre::ITransformable
{
public:

    typedef ::fwDataTools::helper::MedicalImage::Orientation OrientationMode;

    fwCoreServiceClassDefinitionsMacro( (SNegato3D)(::fwRenderOgre::IAdaptor) )

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

    /// Sets whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera;

    /// Sets the opacity to that of the transfer function.
    bool m_enableAlpha {false};

    /// Ogre texture which will be displayed on the negato
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view)
    std::unique_ptr< ::fwRenderOgre::TransferFunction> m_gpuTF;

    /// Stores the planes on which we will apply our texture
    ::fwRenderOgre::Plane* m_planes[3];

    /// The current selected plane. This one will move in the scene
    ::fwRenderOgre::Plane* m_activePlane;

    /// The scene node allowing to move the entire negato
    ::Ogre::SceneNode* m_negatoSceneNode;

    /// Defines the filtering type for this negato
    ::fwRenderOgre::Plane::FilteringEnumType m_filtering;

    /// Image orientation
    OrientationMode m_orientation;

    /// Axial slice index
    size_t m_axialIndex {0};
    /// Frontal slice index
    size_t m_frontalIndex {0};
    /// Sagittal slice index
    size_t m_sagittalIndex {0};

    ::fwDataTools::helper::TransferFunction m_helperTF;
};

//------------------------------------------------------------------------------
// Inline functions

inline void SNegato3D::setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering )
{
    m_filtering = _filtering;
}

//------------------------------------------------------------------------------

} // visuOgreAdaptor
