/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include <data/string.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/base.hpp>
#include <viz/scene3d/picking_cross.hpp>
#include <viz/scene3d/plane.hpp>
#include <viz/scene3d/texture.hpp>
#include <viz/scene3d/transfer_function.hpp>
#include <viz/scene3d/transformable.hpp>

#include <OGRE/OgreManualObject.h>

#include <optional>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a 3D negato.
 *
 * @section Signals Signals
 * - \b picked_voxel(string): sends the coordinates and intensity of the voxel picked by the cross widget.
 *
 * @section Slots Slots
 * - \b new_image(): update the image display to show the new content.
 * - \b sliceType(int, int): update image slice index .
 * - \b sliceIndex(int, int, int): update image slice type.
 * - \b set_transparency(double): sets the global transparency of the three image planes.
 * - \b update_visibility(bool): sets whether the negato is shown or not.
 * - \b toggle_visibility(): toggle whether the negato is shown or not.
 * - \b show(): shows the negato.
 * - \b hide(): hides the negato.
 * - \b update_slices_from_world(double, double, double): updates image slices indexes according to a 3d world point
 * or landmark.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::negato3d">
        <in key="image" uid="..." />
        <in key="mask" uid="..." />
        <inout key="tf" uid="..." />
        <config sliceIndex="axial" filtering="none" tfAlpha="true" />
        <properties classification="pre" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::image]: image to display.
 * - \b mask [sight::data::image] (optional): mask to apply onto the image. Values < 0.5 are considered masked.
 *
 * @subsection In-Out In-Out:
 * - \b tf [sight::data::transfer_function]: the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration:
 * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato.
 * - \b filtering (optional, none/linear/anisotropic, default=none): texture filter type of the negato.
 * - \b tfAlpha (optional, bool, default=false): if true, the alpha channel of the transfer function is used.
 * - \b interactive (optional, bool, default=false): enables interactions on the negato.
 * - \b priority (optional, int, default=1): interaction priority of the negato.
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the negato, as it
 *      was specified in the transform adaptor.
 * - \b queryFlags (optional, uint32, default=0x40000000): Mask set to planes for picking request.
 * - \b border (optional, bool, default=true): allows to display plane borders.
 *
 * @subsection Properties Properties:
 * - \b classification (optional, pre/post, default=pre): classification of voxels. "pre" means the filtering is applied
 * after the sampling of the transfer function, and "post" after. When using labelled images, it is highly recommended
 * to use "pre", otherwise it is likely that class of objects can be confounded.
 * - \b visible (optional, bool, default=true): set the initial visibility of the 3D negato.
 */
class negato3d final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable,
    public sight::viz::scene3d::interactor::base
{
public:

    using axis_t = data::helper::medical_image::axis_t;

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(negato3d, sight::viz::scene3d::adaptor);

    struct signals
    {
        using picked_voxel_t = core::com::signal<void (std::string)>;
        static inline const core::com::signals::key_t PICKED_VOXEL = "picked_voxel";
    };

    struct slots
    {
        static inline const core::com::slots::key_t UPDATE_IMAGE             = "update_image";
        static inline const core::com::slots::key_t UPDATE_TF                = "update_tf";
        static inline const core::com::slots::key_t SLICE_TYPE               = "slice_type";
        static inline const core::com::slots::key_t SLICE_INDEX              = "slice_index";
        static inline const core::com::slots::key_t UPDATE_SLICES_FROM_WORLD = "update_slices_from_world";
        static inline const core::com::slots::key_t SET_TRANSPARENCY         = "set_transparency";
    };

    /// Creates slots.
    negato3d() noexcept;

    /// Destroys the adaptor.
    ~negato3d() noexcept override = default;

protected:

    /// Configures the service.
    void configuring() override;

    /// Starts the service.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::MODIFIED_SIG of s_IMAGE_INOUT to NEWIMAGE_SLOT
     * Connect data::image::MODIFIED_SIG of BUFFER_MODIFIED_SIG to NEWIMAGE_SLOT
     * Connect data::image::MODIFIED_SIG of SLICE_TYPE_MODIFIED_SIG to SLICETYPE_SLOT
     * Connect data::image::MODIFIED_SIG of SLICE_INDEX_MODIFIED_SIG to SLICEINDEX_SLOT
     */
    service::connections_t auto_connections() const override;

    /// Requests rendering of the scene.
    void updating() override;

    /// Stops the service, disconnects connections.
    void stopping() override;

    /**
     * @brief Sets the negato visibility.
     * @param _visible the visibility status of the negato.
     */
    void set_visible(bool _visible) override;

private:

    /// Update the displayed transfer function.
    void update_tf();

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
    void mouse_move_event(mouse_button _button, modifier /*_mods*/, int _x, int _y, int _dx, int _dy) override;

    /**
     * @brief Attempts to pick the negato and starts interactions if picking was successful.
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y) override;

    /// Ends all interactions, regardless of the input.
    void button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/) override;

    /**
     * @brief Sets the slice intersection at the (_x, _y) screen position if possible.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void move_slices(int _x, int _y);

    /**
     * @brief Picks the intensity value at the (_x, _y) screen position.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void pick_intensity(int _x, int _y);

    /**
     * @brief Update slices index to match x,y,z world coordinates
     * @param _x world coordinates in double.
     * @param _y world coordinates in double.
     * @param _z world coordinates in double.
     */
    void update_slices_from_world(double _x, double _y, double _z);

    /**
     * @brief Updates the transfer function window and level by adding the input values.
     * @param _dw window delta.
     * @param _dl level delta.
     */
    void update_windowing(double _dw, double _dl);

    /// SLOT: updates the image buffer.
    void new_image();

    /// SLOT: updates the image slice type.
    void change_slice_type(int /*unused*/, int /*unused*/);

    /**
     * @brief SLOT: updates the image slice index.
     * @param _axial_index new axial slice index.
     * @param _frontal_index new frontal slice index.
     * @param _sagittal_index new sagittal slice index.
     */
    void change_slice_index(int _axial_index, int _frontal_index, int _sagittal_index);

    /// SLOT: sets the planes's opacity.
    void set_transparency(double _transparency);

    /// Sets the picking flags on all three negato planes.
    void set_planes_query_flags(std::uint32_t _flags);

    /// Attemps to pick the negato planes, returns the world space position of the intersection if successful.
    std::optional<Ogre::Vector3> get_picked_slices(int _x, int _y);

    /// Enables whether the camera must be auto reset when a mesh is updated or not.
    bool m_auto_reset_camera {true};

    /// Enables the opacity to that of the transfer function.
    bool m_enable_alpha {false};

    /// Enables whether or not interactions are enabled on the negato.
    bool m_interactive {false};

    /// Sets the order in which interactions take place in the scene.
    int m_priority {1};

    /// Contains the ogre texture which will be displayed on the negato.
    sight::viz::scene3d::texture::sptr m_3d_ogre_texture;

    /// Contains the optional mask texture which will be applied on top of the negato.
    sight::viz::scene3d::texture::sptr m_mask_texture;

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view).
    sight::viz::scene3d::transfer_function::uptr m_gpu_tf {nullptr};

    /// Stores the planes on which we will apply our texture.
    std::array<sight::viz::scene3d::plane::sptr, 3> m_planes;

    /// Contains the plane that the user is currently interacting with.
    sight::viz::scene3d::plane::sptr m_picked_plane {nullptr};

    /// Contains the widget displayed to pick intensities.
    std::unique_ptr<sight::viz::scene3d::picking_cross> m_picking_cross;

    /// Contains the scene node allowing to move the entire negato.
    Ogre::SceneNode* m_negato_scene_node {nullptr};

    /// Contains the scene node used for image origin and orientation.
    Ogre::SceneNode* m_origin_scene_node {nullptr};

    /// Defines the filtering type for this negato.
    sight::viz::scene3d::plane::filter_t m_filtering {sight::viz::scene3d::plane::filter_t::none};

    /// Defines the transfer function window value at the time the interaction started.
    double m_initial_window {0.F};

    /// Defines the transfer function level value at the time the interaction started.
    double m_initial_level {0.F};

    /// Defines the mouse position at the time the windowing interaction started.
    Ogre::Vector2i m_initial_pos {-1, -1};

    /// Defines the mask used for picking request.
    std::uint32_t m_query_flags {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Defines if the plane border is used or not.
    bool m_border {true};

    enum class update_flags : std::uint8_t
    {
        IMAGE,
        TF
    };

    data::ptr<data::image, data::access::in> m_image {this, "image"};
    data::ptr<sight::data::image, sight::data::access::in> m_mask {this, "mask", true};
    data::ptr<data::transfer_function, data::access::inout> m_tf {this, "tf"};

    sight::data::property<sight::data::string> m_classification {this, "classification", std::string("post")};
};

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
