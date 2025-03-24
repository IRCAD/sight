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

#include <data/helper/medical_image.hpp>
#include <data/string.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/base.hpp>
#include <viz/scene3d/picking_cross.hpp>
#include <viz/scene3d/plane.hpp>
#include <viz/scene3d/texture.hpp>
#include <viz/scene3d/transfer_function.hpp>
#include <viz/scene3d/transformable.hpp>

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
 * - \b update_visibility(bool): shows or hides the negato.
 * - \b toggle_visibility(): toggle whether the negato is shown or not.
 * - \b show(): shows the negato.
 * - \b hide(): hides the negato.
 * - \b update_slices_from_world(double, double, double): updates image slices indexes according to a 3d world point
 * or landmark.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::negato2d">
        <in key="image" uid="..." />
        <in key="mask" uid="..." />
        <in key="tf" uid="..." />
        <config sliceIndex="axial" filtering="none" tf_alpha="true" />
        <properties classification="pre" visible="true" />
   </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::image]: image to display.
 * - \b mask [sight::data::image] (optional): mask to apply onto the image. Values < 0.5 are considered masked.
 * - \b tf [sight::data::transfer_function] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration:
 * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato.
 * - \b filtering (optional, none/linear/anisotropic, default=none): texture filter type of the negato.
 * - \b tf_alpha (optional, bool, default=false): if true, the alpha channel of the transfer function is used.
 * - \b border (optional, bool, default=true): displays a border around the plane.
 * - \b slicesCross (optional, bool, default=true): display the two other slices location as two lines.
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the negato, as it
 *      was specified in the transform adaptor.
 * - \b interactive (optional, bool, default=false): enables interactions on the negato.
 *
 * @subsection Properties Properties:
 * - \b classification (optional, pre/post, default=pre): classification of voxels. "pre" means the filtering is applied
 * after the sampling of the transfer function, and "post" after. When using labelled images, it is highly recommended
 * to use "pre", otherwise it is likely that class of objects can be confounded.
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class negato2d final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable,
    public sight::viz::scene3d::interactor::base
{
public:

    using axis_t = data::helper::medical_image::axis_t;

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(negato2d, sight::viz::scene3d::adaptor);

    struct signals
    {
        using slice_index_changed_t = core::com::signal<void ()>;
        using picked_voxel_t        = core::com::signal<void (std::string)>;
        static inline const core::com::signals::key_t SLICE_INDEX_CHANGED = "slice_index_changed";
        static inline const core::com::signals::key_t PICKED_VOXEL        = "picked_voxel";
    };

    struct slots
    {
        static inline const core::com::slots::key_t UPDATE_IMAGE             = "update_image";
        static inline const core::com::slots::key_t UPDATE_TF                = "update_tf";
        static inline const core::com::slots::key_t SLICE_TYPE               = "slice_type";
        static inline const core::com::slots::key_t SLICE_INDEX              = "slice_index";
        static inline const core::com::slots::key_t UPDATE_SLICES_FROM_WORLD = "update_slices_from_world";
    };

    /// Creates the service and initializes slots.
    negato2d() noexcept;

    /// Destroys the adaptor.
    ~negato2d() noexcept final = default;

protected:

    /// Configures the service.
    void configuring() final;

    /// Instantiates the texture, material, pass and texture unit state.
    /// Sets the connection between attached data and the received slot.
    void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::MODIFIED of s_IMAGE_INOUT to service::slots::UPDATE
     * Connect data::image::BUFFER_MODIFIED of s_IMAGE_INOUT to service::slots::UPDATE
     * Connect data::image::SLICE_TYPE_MODIFIED of s_IMAGE_INOUT to SLICE_TYPE
     * Connect data::image::SLICE_INDEX_MODIFIED of s_IMAGE_INOUT to SLICE_INDEX
     */
    service::connections_t auto_connections() const final;

    /// Uploads the input image into the texture buffer and recomputes the negato geometry.
    void updating() final;

    /// Disconnects the attached data from the received slot.
    void stopping() final;

    /**
     * @brief Sets the negato visibility.
     * @param _visible the visibility status of the negato.
     */
    void set_visible(bool _visible) final;

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
    void mouse_move_event(mouse_button _button, modifier /*_mods*/, int _x, int _y, int _dx, int _dy) final;

    /**
     * @brief Attempts to pick the negato and starts interactions if picking was successful.
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y) final;

    /// Ends all interactions, regardless of the input.
    void button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/) final;

    /// Updates the displayed transfer function.
    void update_tf();

    /// Sets the filtering type.
    void set_filtering(sight::viz::scene3d::plane::filter_t _filtering);

    /// Uploads the input image into the texture buffer and recomputes the negato geometry.
    void new_image();

    /**
     * @brief SLOT: updates the image slice type.
     * @param _from origin of the orientation.
     * @param _to destination of the orientation.
     */
    void change_slice_type(int _from, int _to);

    /**
     * @brief SLOT: updates the image slice index.
     * @param _axial_index new axial slice index.
     * @param _frontal_index new frontal slice index.
     * @param _sagittal_index new sagittal slice index.
     */
    void change_slice_index(int _axial_index, int _frontal_index, int _sagittal_index);

    /**
     * @brief SLOT: Update slices index to match x,y,z world coordinates
     * @param _x world coordinates in double.
     * @param _y world coordinates in double.
     * @param _z world coordinates in double.
     */
    void update_slices_from_world(double _x, double _y, double _z);

    /// Updates image slice index for the current fragment program.
    void update_shader_slice_index_parameter();

    /**
     * @brief Picks the intensity value at the (_x, _y) screen position.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void pick_intensity(int _x, int _y);

    /// Contains the texture which will be displayed on the negato.
    sight::viz::scene3d::texture::sptr m_3d_ogre_texture;

    /// Contains the optional mask texture which will be applied on top of the negato.
    sight::viz::scene3d::texture::sptr m_mask_texture;

    /// Contains and manages the textures used to store the transfer function (GPU point of view).
    sight::viz::scene3d::transfer_function::uptr m_gpu_tf;

    /// Contains the plane on which we will apply our texture.
    std::unique_ptr<sight::viz::scene3d::plane> m_plane {nullptr};

    /// Contains the widget displayed to pick intensities.
    std::unique_ptr<sight::viz::scene3d::picking_cross> m_picking_cross;

    /// Enables/disables the usage of the transfer function alpha channel.
    bool m_enable_alpha {false};

    /// Enables whether or not interactions are enabled on the negato.
    bool m_interactive {true};

    /// Contains the scene node allowing to move the entire negato.
    Ogre::SceneNode* m_negato_scene_node {nullptr};

    /// Contains the scene node used for image origin and orientation.
    Ogre::SceneNode* m_origin_scene_node {nullptr};

    /// Defines the filtering type for this negato.
    sight::viz::scene3d::plane::filter_t m_filtering {sight::viz::scene3d::plane::filter_t::none};

    /// Stores the current slice index for each axis.
    std::array<float, 3> m_current_slice_index {0.F, 0.F, 0.F};

    /// Defines the image orientation.
    axis_t m_axis {axis_t::z_axis};

    /// Defines if the plane border is used or not.
    bool m_border {true};

    /// Defines if the other slices planes are displayed.
    bool m_slices_cross {true};

    /// True if the plane is being picked
    bool m_picked {false};

    enum class update_flags : std::uint8_t
    {
        IMAGE,
        TF
    };

    sight::data::ptr<sight::data::image, sight::data::access::in> m_image {this, "image"};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_mask {this, "mask", true};
    sight::data::ptr<sight::data::transfer_function, sight::data::access::in> m_tf {this, "tf"};

    sight::data::property<sight::data::string> m_classification {this, "classification", std::string("post")};
};

//------------------------------------------------------------------------------

inline void negato2d::set_filtering(sight::viz::scene3d::plane::filter_t _filtering)
{
    m_filtering = _filtering;
}

} // namespace sight::module::viz::scene3d::adaptor.
