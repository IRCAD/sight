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

#include <fwCom/Signal.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>
#include <fwDataTools/helper/TransferFunction.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Plane.hpp>
#include <fwRenderOgre/TransferFunction.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief Adaptor to display a 2D negato.
 *
 * @section Signals Signals
 * - \b sliceIndexChanged(): emitted when the slice index changed.
 *
 * @section Slots Slots
 * - \b sliceType(int, int): updates image slice index .
 * - \b sliceIndex(int, int, int): updates image slice type.
 * - @deprecated \b newImage(): updates the image display to show the new content. call \b update() instead.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::visuOgreAdaptor::SNegato2D">
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
 * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato.
 * - \b filtering (optional, none/linear/anisotropic, default=none): texture filter type of the negato.
 * - \b tfalpha (optional, true/false, default=false): if true, the alpha channel of the transfer function is used.
 */
class VISUOGREADAPTOR_CLASS_API SNegato2D final : public ::fwRenderOgre::IAdaptor
{
public:

    typedef ::fwDataTools::helper::MedicalImage::Orientation OrientationMode;

    fwCoreServiceMacro(SNegato2D, ::fwRenderOgre::IAdaptor)

    /// Creates the service and initializes slots.
    VISUOGREADAPTOR_API SNegato2D() noexcept;

    /// Destroys the service.
    VISUOGREADAPTOR_API virtual ~SNegato2D() noexcept;

    /// Sets the filtering type.
    VISUOGREADAPTOR_API void setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering );

private:

    /// Configures the service.
    virtual void configuring() override;

    /// Instantiates the texture, material, pass and texture unit state.
    /// Sets the connection between attached data and the received slot.
    virtual void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_IMAGE_INOUT to ::visuOgreAdaptor::STransform::s_UPDATE_SLOT
     * Connect ::fwData::Image::s_BUFFER_MODIFIED_SIG of s_IMAGE_INOUT to ::visuOgreAdaptor::STransform::s_UPDATE_SLOT
     * Connect ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG of s_IMAGE_INOUT to
     * ::visuOgreAdaptor::STransform::s_SLICETYPE_SLOT
     * Connect ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG of s_IMAGE_INOUT to
     * ::visuOgreAdaptor::STransform::s_SLICEINDEX_SLOT
     */
    virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Uploads the input image into the texture buffer and recomputes the negato geometry.
    virtual void updating() override;

    /**
     * @brief Retrieves the current transfer function.
     * @param _key key of the swapped data.
     */
    virtual void swapping(const KeyType& _key) override;

    /// Disconnects the attached data from the received slot.
    virtual void stopping() override;

    /// Updates the displayed transfer function.
    virtual void updateTF();

    /// Uploads the input image into the texture buffer and recomputes the negato geometry.
    void newImage();

    /// SLOT: updates the image buffer, @deprecated call @ref update() instead.
    void newImageDeprecatedSlot();

    /**
     * @brief SLOT: updates the image slice type.
     * @param _from origin of the orientation.
     * @param _to destination of the orientation.
     */
    void changeSliceType(int _from, int _to);

    /**
     * @brief SLOT: updates the image slice index.
     * @param _axialIndex new axial slice index.
     * @param _frontalIndex new frontal slice index.
     * @param _sagittalIndex new sagittal slice index.
     */
    void changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex);

    /// Updates image slice index for the current fragment program.
    void updateShaderSliceIndexParameter();

    /**
     * @brief Initializes the planar mesh on which the negato is displayed.
     * @param _spacing spacing of the input image.
     */
    void createPlane(const ::Ogre::Vector3& _spacing);

    /// Contains the Ogre texture which will be displayed on the negato.
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view).
    std::unique_ptr< ::fwRenderOgre::TransferFunction> m_gpuTF;

    /// Contains the plane on which we will apply our texture.
    std::unique_ptr< ::fwRenderOgre::Plane > m_plane { nullptr };

    /// Enables/disables the usage of the transfer function alpha channel.
    bool m_enableAlpha { false };

    /// Contains the scene node allowing to move the entire negato.
    ::Ogre::SceneNode* m_negatoSceneNode { nullptr };

    /// Sets the filtering type for this negato.
    ::fwRenderOgre::Plane::FilteringEnumType m_filtering { ::fwRenderOgre::Plane::FilteringEnumType::NONE };

    /// Stores the current slice index for each axis.
    std::vector<float> m_currentSliceIndex { 0.f, 0.f, 0.f };

    /// Sets the image orientation.
    OrientationMode m_orientation { OrientationMode::Z_AXIS };

    /// Helps interfacing with the transfer function input.
    ::fwDataTools::helper::TransferFunction m_helperTF;

    using SliceIndexChangedSignalType = ::fwCom::Signal<void()>;
    SliceIndexChangedSignalType::sptr m_sliceIndexChangedSig;
};

//------------------------------------------------------------------------------
// Inline functions

inline void SNegato2D::setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering )
{
    m_filtering = _filtering;
}

//------------------------------------------------------------------------------
} // visuOgreAdaptor
