/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include <viz/scene3d/IAdaptor.hpp>
#include <viz/scene3d/interactor/IInteractor.hpp>
#include <viz/scene3d/ITransformable.hpp>
#include <viz/scene3d/PickingCross.hpp>
#include <viz/scene3d/Plane.hpp>
#include <viz/scene3d/Texture.hpp>
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
 * - \b updateSlicesFromWorld(double, double, double): updates image slices indexes according to a 3d world point
 * or landmark.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::SNegato2D">
        <in key="image" uid="..." />
        <in key="tf" uid="..." />
        <config layer="default" sliceIndex="axial" filtering="none" tfAlpha="true" visible="true" />
   </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::Image]: image to display.
 * - \b tf [sight::data::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): id of the layer where this adaptor applies.
 * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato.
 * - \b filtering (optional, none/linear/anisotropic, default=none): texture filter type of the negato.
 * - \b tfAlpha (optional, bool, default=false): if true, the alpha channel of the transfer function is used.
 * - \b border (optional, bool, default=true): allows to display the plane border.
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the negato, as it
 *      was specified in the STransform adaptor.
 * * - \b interactive (optional, bool, default=false): enables interactions on the negato.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SNegato2D final :
    public sight::viz::scene3d::IAdaptor,
    public sight::viz::scene3d::ITransformable,
    public sight::viz::scene3d::interactor::IInteractor
{
public:

    using OrientationMode = data::helper::MedicalImage::orientation_t;

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SNegato2D, sight::viz::scene3d::IAdaptor);

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

    /// Disconnects the attached data from the received slot.
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /**
     * @brief Sets the negato visibility.
     * @param _visible the visibility status of the negato.
     */
    MODULE_VIZ_SCENE3D_API void setVisible(bool _visible) override;

private:

    /**
     * @brief Interacts with the negato if it was picked by pressing any mouse button.
     *
     * Interactions will take place while holding down the button. The following actions are available:
     * - Left mouse click: shows a cross widget to select a voxel and retrieve its intensity.
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     * @param _dx the cursor's width displacement since the last event.
     * @param _dy the cursor's height displacement since the last event.
     */
    void mouseMoveEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y, int _dx, int _dy) override;

    /**
     * @brief Attempts to pick the negato and starts interactions if picking was successful.
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y) override;

    /// Ends all interactions, regardless of the input.
    void buttonReleaseEvent(MouseButton /*_button*/, Modifier /*_mods*/, int /*_x*/, int /*_y*/) override;

    /// Updates the displayed transfer function.
    void updateTF();

    /// Sets the filtering type.
    void setFiltering(sight::viz::scene3d::Plane::filter_t _filtering);

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

    /**
     * @brief SLOT: Update slices index to match x,y,z world coordinates
     * @param _x world coordinates in double.
     * @param _y world coordinates in double.
     * @param _z world coordinates in double.
     */
    void updateSlicesFromWorld(double _x, double _y, double _z);

    /// Updates image slice index for the current fragment program.
    void updateShaderSliceIndexParameter();

    /**
     * @brief Picks the intensity value at the (_x, _y) screen position.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void pickIntensity(int _x, int _y);

    /// Contains the texture which will be displayed on the negato.
    sight::viz::scene3d::Texture::sptr m_3DOgreTexture;

    /// Contains and manages the textures used to store the transfer function (GPU point of view).
    sight::viz::scene3d::TransferFunction::uptr m_gpuTF;

    /// Contains the plane on which we will apply our texture.
    std::unique_ptr<sight::viz::scene3d::Plane> m_plane {nullptr};

    /// Contains the widget displayed to pick intensities.
    std::unique_ptr<sight::viz::scene3d::PickingCross> m_pickingCross;

    /// Enables/disables the usage of the transfer function alpha channel.
    bool m_enableAlpha {false};

    /// Enables whether or not interactions are enabled on the negato.
    bool m_interactive {true};

    /// Contains the scene node allowing to move the entire negato.
    Ogre::SceneNode* m_negatoSceneNode {nullptr};

    /// Defines the filtering type for this negato.
    sight::viz::scene3d::Plane::filter_t m_filtering {sight::viz::scene3d::Plane::filter_t::NONE};

    /// Stores the current slice index for each axis.
    std::vector<float> m_currentSliceIndex {0.F, 0.F, 0.F};

    /// Defines the image orientation.
    OrientationMode m_orientation {OrientationMode::Z_AXIS};

    /// Defines if the plane border is used or not.
    bool m_border {true};

    /// True if the plane is being picked
    bool m_picked {false};

    using SliceIndexChangedSignalType = core::com::Signal<void ()>;
    SliceIndexChangedSignalType::sptr m_sliceIndexChangedSig;

    /// Defines the signal sent when a voxel is picked using the left mouse button.
    using PickedVoxelSigType = core::com::Signal<void (std::string)>;
    PickedVoxelSigType::sptr m_pickedVoxelSignal {nullptr};

    static constexpr std::string_view s_IMAGE_IN = "image";
    static constexpr std::string_view s_TF_IN    = "tf";

    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_image {this, s_IMAGE_IN, true};
    sight::data::ptr<sight::data::TransferFunction, sight::data::Access::in> m_tf {this, s_TF_IN, true};
};

//------------------------------------------------------------------------------

inline void SNegato2D::setFiltering(sight::viz::scene3d::Plane::filter_t _filtering)
{
    m_filtering = _filtering;
}

} // namespace sight::module::viz::scene3d::adaptor.
