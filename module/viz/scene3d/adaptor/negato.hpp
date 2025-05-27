/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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
#include <data/real.hpp>
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
 * @brief This is the base interface for 2D and 3D negato classes.
 */
class negato :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable,
    public sight::viz::scene3d::interactor::base
{
public:

    using axis_t = data::helper::medical_image::axis_t;

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
        static inline const core::com::slots::key_t UPDATE_IMAGE_BUFFER      = "update_image_buffer";
        static inline const core::com::slots::key_t UPDATE_TF                = "update_tf";
        static inline const core::com::slots::key_t SLICE_TYPE               = "slice_type";
        static inline const core::com::slots::key_t SLICE_INDEX              = "slice_index";
        static inline const core::com::slots::key_t UPDATE_SLICES_FROM_WORLD = "update_slices_from_world";
    };

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::MODIFIED of s_IMAGE_INOUT to service::slots::UPDATE
     * Connect data::image::BUFFER_MODIFIED of s_IMAGE_INOUT to service::slots::UPDATE
     * Connect data::image::SLICE_TYPE_MODIFIED of s_IMAGE_INOUT to SLICE_TYPE
     * Connect data::image::SLICE_INDEX_MODIFIED of s_IMAGE_INOUT to SLICE_INDEX
     */
    service::connections_t auto_connections() const override;

    /// Creates the service and initializes slots.
    negato() noexcept;

    /// Destroys the adaptor.
    ~negato() noexcept override = default;

protected:

    /// Configures the service.
    void configuring(const config_t& _config) override;

    /// Instantiates the texture, material, pass and texture unit state.
    /// Sets the connection between attached data and the received slot.
    void starting() final;

    /// Remove all resources, disconnects connections.
    void stopping() final;

    /// Uploads the input image into the texture buffer and recomputes the negato geometry.
    void updating() override;

    /**
     * @brief Picks the intensity value at the (_x, _y) screen position.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void pick_intensity(int _x, int _y);

    /**
     * @brief Updates the transfer function window and level by adding the input values.
     * @param _dw window delta.
     * @param _dl level delta.
     */
    void update_windowing(double _dw, double _dl);

    /// Updates the displayed transfer function.
    void update_tf();

private:

    /// Sets the filtering type.
    void set_filtering(sight::viz::scene3d::plane::filter_t _filtering);

    /** Uploads the input image into the texture buffer and recomputes the negato geometry.
     * @param _new true if the image was reallocated, false if only pixel values changed.
     */
    void update_image(bool _new);

    /**
     * @brief SLOT: updates the image slice type.
     * @param _from origin of the orientation.
     * @param _to destination of the orientation.
     */
    virtual void change_slice_type(int _from, int _to) = 0;

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

protected:

    /// Contains the widget displayed to pick intensities.
    std::unique_ptr<sight::viz::scene3d::picking_cross> m_picking_cross;

    /// Stores the planes and their axes on which we will apply our texture.
    std::vector<std::pair<sight::viz::scene3d::plane::sptr, axis_t> > m_planes;

    /// Contains the plane that the user is currently interacting with.
    sight::viz::scene3d::plane::sptr m_picked_plane {nullptr};

    /// Contains the scene node allowing to move the entire negato.
    Ogre::SceneNode* m_negato_scene_node {nullptr};

    /// Defines if the other slices planes are displayed.
    bool m_slices_cross {false};

    /// Stores the current slice index for each axis.
    std::array<float, 3> m_current_slice_index {0.F, 0.F, 0.F};

    /// Defines the transfer function window value at the time the interaction started.
    double m_initial_window {0.F};

    /// Defines the transfer function level value at the time the interaction started.
    double m_initial_level {0.F};

    /// Defines the mouse position at the time the windowing interaction started.
    Ogre::Vector2i m_initial_pos {-1, -1};

    /// Enables/disables the usage of the transfer function alpha channel.
    bool m_enable_alpha {false};

    /// Enables whether the camera must be auto reset when a mesh is updated or not.
    bool m_auto_reset_camera {false};

    /// Defines the mask used for picking request.
    std::uint32_t m_query_flags {Ogre::SceneManager::ENTITY_TYPE_MASK};

    sight::data::ptr<sight::data::image, sight::data::access::in> m_image {this, "image"};
    sight::data::ptr<sight::data::transfer_function, sight::data::access::inout> m_tf {this, "tf"};

private:

    /// Contains the texture which will be displayed on the negato.
    sight::viz::scene3d::texture::sptr m_3d_ogre_texture;

    /// Contains the optional mask texture which will be applied on top of the negato.
    sight::viz::scene3d::texture::sptr m_mask_texture;

    /// Contains and manages the textures used to store the transfer function (GPU point of view).
    sight::viz::scene3d::transfer_function::uptr m_gpu_tf;

    /// Enables whether or not interactions are enabled on the negato.
    bool m_interactive {true};

    /// Contains the scene node used for image origin and orientation.
    Ogre::SceneNode* m_origin_scene_node {nullptr};

    /// Defines the filtering type for this negato.
    sight::viz::scene3d::plane::filter_t m_filtering {sight::viz::scene3d::plane::filter_t::none};

    /// Defines if the plane border is used or not.
    bool m_border {true};

    /// Sets the order in which interactions take place in the scene.
    int m_priority {1};

    enum class update_flags : std::uint8_t
    {
        IMAGE,
        IMAGE_BUFFER,
        TF
    };

    sight::data::ptr<sight::data::image, sight::data::access::in> m_mask {this, "mask", true};
    sight::data::property<sight::data::string> m_classification {this, "classification", std::string("post")};
    sight::data::property<sight::data::real> m_depth_bias {this, "depth_bias", 0.};
};

//------------------------------------------------------------------------------

inline void negato::set_filtering(sight::viz::scene3d::plane::filter_t _filtering)
{
    m_filtering = _filtering;
}

} // namespace sight::module::viz::scene3d::adaptor.
