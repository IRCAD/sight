/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/viz/scene3d/config.hpp"

#include <core/com/Signal.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/helper/TransferFunction.hpp>

#include <viz/scene3d/IAdaptor.hpp>
#include <viz/scene3d/Plane.hpp>
#include <viz/scene3d/TransferFunction.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a 2D negato.
 *
 * @section Signals Signals
 * - \b sliceIndexChanged(): emitted when the slice index changed.
 *
 * @section Slots Slots
 * - \b sliceType(int, int): updates image slice index .
 * - \b sliceIndex(int, int, int): updates image slice type.
 * - \b updateVisibility(bool): shows or hides the negato.
 * - \b toggleVisibility(): toggle whether the negato is shown or not.
 * - \b show(): shows the negato.
 * - \b hide(): hides the negato.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="::sight::module::viz::scene3d::adaptor::SNegato2D">
        <inout key="image" uid="..." />
        <inout key="tf" uid="..." optional="yes" />
        <config layer="default" sliceIndex="axial" filtering="none" tfalpha="true" visible="true" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b image [sight::data::Image]: image to display.
 * - \b tf [sight::data::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): id of the layer where this adaptor applies.
 * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato.
 * - \b filtering (optional, none/linear/anisotropic, default=none): texture filter type of the negato.
 * - \b tfalpha (optional, bool, default=false): if true, the alpha channel of the transfer function is used.
 * - \b border (optional, bool, default=true): allows to display the plane border.
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SNegato2D final : public sight::viz::scene3d::IAdaptor
{

public:

    typedef data::helper::MedicalImage::Orientation OrientationMode;

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SNegato2D, ::sight::viz::scene3d::IAdaptor)

    /// Creates the service and initializes slots.
    MODULE_VIZ_SCENE3D_API SNegato2D() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SNegato2D() noexcept override;

protected:

    /// Configures the service.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Instantiates the texture, material, pass and texture unit state.
    /// Sets the connection between attached data and the received slot.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Image::s_MODIFIED_SIG of s_IMAGE_INOUT to s_UPDATE_SLOT
     * Connect data::Image::s_BUFFER_MODIFIED_SIG of s_IMAGE_INOUT to s_UPDATE_SLOT
     * Connect data::Image::s_SLICE_TYPE_MODIFIED_SIG of s_IMAGE_INOUT to s_SLICETYPE_SLOT
     * Connect data::Image::s_SLICE_INDEX_MODIFIED_SIG of s_IMAGE_INOUT to s_SLICEINDEX_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Uploads the input image into the texture buffer and recomputes the negato geometry.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /**
     * @brief Retrieves the current transfer function.
     * @param _key key of the swapped data.
     */
    MODULE_VIZ_SCENE3D_API void swapping(const KeyType& _key) override;

    /// Disconnects the attached data from the received slot.
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /**
     * @brief Sets the negato visibility.
     * @param _visible the visibility status of the negato.
     */
    MODULE_VIZ_SCENE3D_API void setVisible(bool _visible) override;

private:

    /// Updates the displayed transfer function.
    void updateTF();

    /// Sets the filtering type.
    void setFiltering(sight::viz::scene3d::Plane::FilteringEnumType _filtering);

    /// Uploads the input image into the texture buffer and recomputes the negato geometry.
    void newImage();

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
    std::unique_ptr< sight::viz::scene3d::TransferFunction> m_gpuTF;

    /// Contains the plane on which we will apply our texture.
    std::unique_ptr< sight::viz::scene3d::Plane > m_plane { nullptr };

    /// Enables/disables the usage of the transfer function alpha channel.
    bool m_enableAlpha { false };

    /// Contains the scene node allowing to move the entire negato.
    ::Ogre::SceneNode* m_negatoSceneNode { nullptr };

    /// Defines the filtering type for this negato.
    sight::viz::scene3d::Plane::FilteringEnumType m_filtering { sight::viz::scene3d::Plane::FilteringEnumType::NONE };

    /// Stores the current slice index for each axis.
    std::vector<float> m_currentSliceIndex { 0.f, 0.f, 0.f };

    /// Defines the image orientation.
    OrientationMode m_orientation { OrientationMode::Z_AXIS };

    /// Helps interfacing with the transfer function input.
    data::helper::TransferFunction m_helperTF;

    /// Defines if the plane border is used or not.
    bool m_border { true };

    using SliceIndexChangedSignalType = core::com::Signal<void()>;
    SliceIndexChangedSignalType::sptr m_sliceIndexChangedSig;
};

//------------------------------------------------------------------------------

inline void SNegato2D::setFiltering( sight::viz::scene3d::Plane::FilteringEnumType _filtering )
{
    m_filtering = _filtering;
}

} // namespace sight::module::viz::scene3d::adaptor.
