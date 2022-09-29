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

#include <viz/scene3d/IAdaptor.hpp>
#include <viz/scene3d/interactor/IInteractor.hpp>
#include <viz/scene3d/ITransformable.hpp>
#include <viz/scene3d/PickingCross.hpp>
#include <viz/scene3d/Plane.hpp>
#include <viz/scene3d/Texture.hpp>
#include <viz/scene3d/TransferFunction.hpp>

#include <OGRE/OgreManualObject.h>

#include <optional>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a 3D negato.
 *
 * @section Signals Signals
 * - \b pickedVoxel(string): sends the coordinates and intensity of the voxel picked by the cross widget.
 *
 * @section Slots Slots
 * - \b newImage(): update the image display to show the new content.
 * - \b sliceType(int, int): update image slice index .
 * - \b sliceIndex(int, int, int): update image slice type.
 * - \b setTransparency(double): sets the global transparency of the three image planes.
 * - \b updateVisibility(bool): sets whether the negato is shown or not.
 * - \b toggleVisibility(): toggle whether the negato is shown or not.
 * - \b show(): shows the negato.
 * - \b hide(): hides the negato.
 * - \b updateSlicesFromWorld(double, double, double): updates image slices indexes according to a 3d world point
 * or landmark.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::SNegato3D">
        <in key="image" uid="..." />
        <inout key="tf" uid="..." />
        <config layer="default" sliceIndex="axial" filtering="none" tfAlpha="true" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::Image]: image to display.
 *
 * @subsection In-Out In-Out:
 * - \b tf [sight::data::TransferFunction]: the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): id of the layer where this adaptor applies.
 * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato.
 * - \b filtering (optional, none/linear/anisotropic, default=none): texture filter type of the negato.
 * - \b tfAlpha (optional, bool, default=false): if true, the alpha channel of the transfer function is used.
 * - \b interactive (optional, bool, default=false): enables interactions on the negato.
 * - \b priority (optional, int, default=1): interaction priority of the negato.
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the negato, as it
 *      was specified in the STransform adaptor.
 * - \b queryFlags (optional, uint32, default=0x40000000): Mask set to planes for picking request.
 * - \b border (optional, bool, default=true): allows to display plane borders.
 * - \b visible (optional, bool, default=true): set the initial visibility of the 3D negato.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SNegato3D final :
    public sight::viz::scene3d::IAdaptor,
    public sight::viz::scene3d::ITransformable,
    public sight::viz::scene3d::interactor::IInteractor
{
public:

    typedef data::helper::MedicalImage::orientation_t OrientationMode;

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SNegato3D, sight::viz::scene3d::IAdaptor);

    /// Creates slots.
    MODULE_VIZ_SCENE3D_API SNegato3D() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SNegato3D() noexcept override;

protected:

    /// Configures the service.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Starts the service.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Image::s_MODIFIED_SIG of s_IMAGE_INOUT to s_NEWIMAGE_SLOT
     * Connect data::Image::s_MODIFIED_SIG of s_BUFFER_MODIFIED_SIG to s_NEWIMAGE_SLOT
     * Connect data::Image::s_MODIFIED_SIG of s_SLICE_TYPE_MODIFIED_SIG to s_SLICETYPE_SLOT
     * Connect data::Image::s_MODIFIED_SIG of s_SLICE_INDEX_MODIFIED_SIG to s_SLICEINDEX_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Requests rendering of the scene.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Stops the service, disconnects connections.
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /**
     * @brief Sets the negato visibility.
     * @param _visible the visibility status of the negato.
     */
    MODULE_VIZ_SCENE3D_API void setVisible(bool _visible) override;

private:

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
     * @brief Update slices index to match x,y,z world coordinates
     * @param _x world coordinates in double.
     * @param _y world coordinates in double.
     * @param _z world coordinates in double.
     */
    void updateSlicesFromWorld(double _x, double _y, double _z);

    /**
     * @brief Updates the transfer function window and level by adding the input values.
     * @param _dw window delta.
     * @param _dl level delta.
     */
    void updateWindowing(double _dw, double _dl);

    /// SLOT: updates the image buffer.
    void newImage();

    /// SLOT: updates the image slice type.
    void changeSliceType(int /*unused*/, int /*unused*/);

    /**
     * @brief SLOT: updates the image slice index.
     * @param _axialIndex new axial slice index.
     * @param _frontalIndex new frontal slice index.
     * @param _sagittalIndex new sagittal slice index.
     */
    void changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex);

    /// SLOT: sets the planes's opacity.
    void setTransparency(double _transparency);

    /// Sets the picking flags on all three negato planes.
    void setPlanesQueryFlags(std::uint32_t _flags);

    /// Attemps to pick the negato planes, returns the world space position of the intersection if successful.
    std::optional<Ogre::Vector3> getPickedSlices(int _x, int _y);

    /// Enables whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera {true};

    /// Enables the opacity to that of the transfer function.
    bool m_enableAlpha {false};

    /// Enables whether or not interactions are enabled on the negato.
    bool m_interactive {false};

    /// Sets the order in which interactions take place in the scene.
    int m_priority {1};

    /// Contains the ogre texture which will be displayed on the negato.
    sight::viz::scene3d::Texture::sptr m_3DOgreTexture;

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view).
    sight::viz::scene3d::TransferFunction::uptr m_gpuTF {nullptr};

    /// Stores the planes on which we will apply our texture.
    std::array<sight::viz::scene3d::Plane::sptr, 3> m_planes;

    /// Contains the plane that the user is currently interacting with.
    sight::viz::scene3d::Plane::sptr m_pickedPlane {nullptr};

    /// Contains the widget displayed to pick intensities.
    std::unique_ptr<sight::viz::scene3d::PickingCross> m_pickingCross;

    /// Contains the scene node allowing to move the entire negato.
    Ogre::SceneNode* m_negatoSceneNode {nullptr};

    /// Defines the filtering type for this negato.
    sight::viz::scene3d::Plane::filter_t m_filtering {sight::viz::scene3d::Plane::filter_t::NONE};

    /// Defines the transfer function window value at the time the interaction started.
    double m_initialWindow {0.F};

    /// Defines the transfer function level value at the time the interaction started.
    double m_initialLevel {0.F};

    /// Defines the mouse position at the time the windowing interaction started.
    Ogre::Vector2i m_initialPos {-1, -1};

    /// Defines the mask used for picking request.
    std::uint32_t m_queryFlags {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Defines if the plane border is used or not.
    bool m_border {true};

    /// Defines the signal sent when a voxel is picked using the left mouse button.
    using PickedVoxelSigType = core::com::Signal<void (std::string)>;
    PickedVoxelSigType::sptr m_pickedVoxelSignal {nullptr};

    static constexpr std::string_view s_IMAGE_IN = "image";
    data::ptr<data::Image, data::Access::in> m_image {this, s_IMAGE_IN, true};

    static constexpr std::string_view s_TF_INOUT = "tf";
    data::ptr<data::TransferFunction, data::Access::inout> m_tf {this, s_TF_INOUT, true};
};

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
