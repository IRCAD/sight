/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <data/point_list.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/texture.hpp>
#include <viz/scene3d/transfer_function.hpp>

#include <OGRE/OgreTexture.h>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor renders a video frame from a 2D-image.
 *
 * @section Slots Slots
 * - \b update_visibility(bool): sets whether the video is visible or not.
 * - \b toggle_visibility(): toggle whether the video is visible or not.
 * - \b show(): shows the video.
 * - \b hide(): hides the video.
 * - \b update_tf(): updates the displayed transfer function.
 * - \b updatePL(): updates the displayed point list.
 * - \b set_filtering(bool): toggles between nearest (false) and bilinear (true) filtering.
 * - \b scale(bool): displays the video at its original size (false) or scales the video to the viewport size (true).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::video" auto_connect="true" >
        <in key="image" uid="..." />
        <in key="tf" uid="..." />
        <config textureName="videoFrame" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::image]: frame displayed.
 * - \b tf [sight::data::transfer_function] (optional): a transfer function that can be applied to the video.
 * - \b pointList [sight::data::point_list] (optional): point list to display (coordinates must be in the image space).
 *
 * @subsection Configuration Configuration:
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 * - \b materialTemplate (optional, string, default='Billboard_Default'): the name of the base Ogre material for the
 *      internally created material.
 * - \b textureName (optional, string, default=""): the name of the Ogre texture the mesh will use.
 * - \b filtering (optional, bool, default=false): whether to use or not bilinear filtering for the texture.
 * - \b scaling (optional, bool, default=true): whether to scale the video to the viewport or
 *      keep it at its original resolution.
 * - \b radius (optional, float, default=1.f): billboard radius.
 * - \b displayLabel (optional, bool, default=false): display the label points (default = false).
 * - \b labelColor (optional, hexadecimal, default=0xFFFFFF): color of the label in hexadecimal.
 * - \b color (optional, hexadecimal, default=#FFFFFFFF): color of the texture in hexadecimal.
 * - \b fixedSize (optional, bool, default=false): if true, the billboard will have a fixed size in screen space.
 * - \b queryFlags (optional, uint32, default=0x40000000): Picking flags. Points can be picked by pickers with a
 *      matching mask.
 * - \b fontSource (optional, string, default=DejaVuSans.ttf): true_t font (*.ttf) source file.
 * - \b fontSize (optional, unsigned int, default=16): font size in points.
 */
class video final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(video, sight::viz::scene3d::adaptor);

    /// Creates the adaptor an initialize slots.
    video() noexcept;

    /// Destroys the adaptor.
    ~video() noexcept final = default;

protected:

    /// Configures the adaptor.
    void configuring() final;

    /// Creates the Ogre texture and mapper used to show the video frame.
    void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::MODIFIED_SIG of s_IMAGE_INPUT to service::slots::UPDATE
     * Connect data::image::BUFFER_MODIFIED_SIG of s_IMAGE_INPUT to service::slots::UPDATE
     * Connect data::transfer_function::MODIFIED_SIG of s_TF_INPUT to :UPDATE_TF_SLOT
     * Connect data::transfer_function::POINTS_MODIFIED_SIG of s_TF_INPUT to UPDATE_TF_SLOT
     * Connect data::transfer_function::WINDOWING_MODIFIED_SIG of s_TF_INPUT to UPDATE_TF_SLOT
     */
    service::connections_t auto_connections() const final;

    /// Updates the frame from the current image.
    void updating() final;

    /// Removes the actor from the renderer
    void stopping() final;

    /**
     * @brief Sets the video visibility.
     * @param _visible the visibility status of the video.
     */
    void set_visible(bool _visible) final;

private:

    ///@brief Internal wrapper holding config defines.
    struct config
    {
        static inline const std::string VISIBLE           = CONFIG + "visible";
        static inline const std::string MATERIAL_TEMPLATE = CONFIG + "materialTemplate";
        static inline const std::string TEXTURE_NAME      = CONFIG + "textureName";
        static inline const std::string FILTERING         = CONFIG + "filtering";
        static inline const std::string SCALING           = CONFIG + "scaling";
        static inline const std::string RADIUS            = CONFIG + "radius";
        static inline const std::string DISPLAY_LABEL     = CONFIG + "displayLabel";
        static inline const std::string LABEL_COLOR       = CONFIG + "labelColor";
        static inline const std::string COLOR             = CONFIG + "color";
        static inline const std::string FIXED_SIZE        = CONFIG + "fixedSize";
        static inline const std::string QUERY             = CONFIG + "queryFlags";
        static inline const std::string FONT_SOURCE       = CONFIG + "fontSource";
        static inline const std::string FONT_SIZE         = CONFIG + "fontSize";
    };

    /// SLOTS: updates the displayed transfer function.
    void update_tf();

    /// SLOTS: updates 2D coordinates of the pointlist from image space (bottom left) to video space (center center).
    void update_pl();

    /// SLOTS: whether to use or not bilinear filtering
    void set_filtering(bool /*filtering*/);

    /// SLOTS: whether to scale or not the video to occupy the whole viewport
    void scale(bool /*value*/);

    /// Updates the current texture filtering option
    void update_texture_filtering();

    /// Cleans scene entities and associated mesh.
    void clear_entity();

    /// Contains the Ogre texture used to store the image.
    sight::viz::scene3d::texture::sptr m_texture;

    /// Contains the Ogre material used to display the video plane.
    Ogre::MaterialPtr m_material;

    /// Contains the Ogre mesh used to display the video plane.
    Ogre::MeshPtr m_mesh;

    /// Contains the node where the plane is attached.
    Ogre::SceneNode* m_scene_node {nullptr};

    /// Contains the Ogre entity used to display the video plane.
    Ogre::Entity* m_entity {nullptr};

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view).
    std::unique_ptr<sight::viz::scene3d::transfer_function> m_gpu_tf;

    /// Defines if the texture is initialized.
    bool m_is_texture_init {false};

    /// Defines the previous image size.
    std::size_t m_previous_width {0};

    /// Defines the previous image spacing.
    std::size_t m_previous_height {0};

    /// Defines the previous viewport width.
    int m_previous_viewport_width {0};

    /// Defines the previous viewport height.
    int m_previous_viewport_height {0};

    /// Defines the previous image type.
    core::type m_previous_type;

    /// Contains the pointlist in the video space coordinates.
    data::point_list::sptr m_point_list {nullptr};

    /// Contains the adaptor used to display the 2D pointlist.
    sight::viz::scene3d::adaptor::sptr m_point_list_adaptor {nullptr};

    /// Defines material's name used by the point list.
    std::string m_material_template_name;

    /// Defines the attached texture adaptor UID used by the point list.
    std::string m_texture_name;

    /// Defines whether of not the video texture will use bilinear filtering
    bool m_filtering {false};

    /// Defines whether the video is scaled to the viewport or displayed at its original size.
    bool m_scaling {true};

    /// Whether an update to the current plane is required due to a scaling change or not
    bool m_force_plane_update {false};

    /// Defines the billboards radius.
    std::string m_radius;

    /// Defines if label numbers are displayed.
    std::string m_display_label;

    /// Defines the RGB color for the label point color.
    std::string m_label_color;

    /// Defines the color of points.
    std::string m_color;

    /// Defines if billboard will have a fixed size in screen space.
    std::string m_fixed_size;

    /// Defines the mask for picking requests in the point list.
    std::string m_query_flags;

    /// Defines the true_t font source file.
    std::string m_font_source;

    /// Defines the font size in points.
    std::string m_font_size;

    static constexpr std::string_view IMAGE_INPUT = "image";
    static constexpr std::string_view TF_INPUT    = "tf";
    static constexpr std::string_view PL_INPUT    = "pointList";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_image {this, IMAGE_INPUT, true};
    sight::data::ptr<sight::data::transfer_function, sight::data::access::in> m_tf {this, TF_INPUT, true, true};
    sight::data::ptr<sight::data::point_list, sight::data::access::in> m_pl {this, PL_INPUT, true, true};
};

} // namespace sight::module::viz::scene3d::adaptor.
