/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include <data/fiducials_series.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/base.hpp>
#include <viz/scene3d/material.hpp>
#include <viz/scene3d/text.hpp>

#include <QPushButton>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

/**
 * @brief This adaptor displays rulers retrieved from the image series.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d_qt::adaptor::fiducials::ruler" auto_connect="true" >
        <inout key="image" uid="..." />
        <config font_size="16" radius="3" priority="2" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::image]: image containing the ruler fiducials.
 *
 * @subsection Configuration Configuration:
 * - \b font_size (optional, int, default=16): font size in labels.
 * - \b radius (optional, float, default=3): size of the rulers spheres.
 * - \b priority (optional, int, default=2): priority of the interactor.
 * - \b query_mask (optional, uint32, default=0xFFFFFFFF): mask used to filter out entities when the ruler is auto
 *      snapped.
 * - \b query_flags (optional, uint32, default=0x40000000): mask apply to rulers spheres.
 * - \b color (optional, string, default=""): color applied to the rulers, generated if empty.
 * - \b always_display_all (optional, bool, default=false): boolean that impacts how rulers will be displayed.
 * If it's true, all rulers will always be displayed, regardless of the current slice.
 *
 * @section Slots Slots
 * - \b activate_tool(bool): enables or not the ruler tool.
 * - \b remove_all(): removes all rulers.
 * - \b remove_from_current_slice(): removes rulers contained in the current slice.
 *
 * @section Signals Signals
 * - \b tool_deactivated(): signal sent when the ruler tool is deactivated.
 */

class ruler final : public sight::viz::scene3d::adaptor,
                    public sight::viz::scene3d::interactor::base
{
public:

    struct signals
    {
        using key_t = sight::core::com::signals::key_t;

        static inline const key_t TOOL_DEACTIVATED = "tool_deactivated";
        using void_signal_t = sight::core::com::signal<void ()>;
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(ruler, sight::viz::scene3d::adaptor);

    /// Creates the adaptor an initialize slots.
    ruler() noexcept;

    /// Destroys the adaptor.
    ~ruler() noexcept final = default;

    struct slots final
    {
        using key_t = sight::core::com::slots::key_t;

        inline static const key_t ACTIVATE_TOOL             = "activate_tool";
        inline static const key_t REMOVE_ALL                = "remove_all";
        inline static const key_t REMOVE_FROM_CURRENT_SLICE = "remove_from_current_slice";
    };

protected:

    sight::service::connections_t auto_connections() const final;

    /// Configures the adaptor.
    void configuring() final;

    /// Creates materials and the interactor.
    void starting() final;

    /// Creates or recreates m_ruler_ogre_sets from the fiducials.
    void updating() final;

    /// Removes the interactor, reset materials and m_ruler_ogre_sets.
    void stopping() final;

private:

    struct private_slots final
    {
        using key_t = sight::core::com::slots::key_t;
        inline static const key_t REMOVE_RULER_OGRE_SET    = "remove_ruler_ogre_set";
        inline static const key_t UPDATE_MODIFIED_RULER    = "update_modified_ruler";
        inline static const key_t DISPLAY_ON_CURRENT_SLICE = "display_on_current_slice";
    };

    class delete_bin_button_when_focus_out : public QObject
    {
    public:

        explicit delete_bin_button_when_focus_out(ruler* _ruler);
        bool eventFilter(QObject* _o, QEvent* _e) override;
    };

    /// Represents ogre elements and the associated fiducial id.
    struct ruler_ogre_set final
    {
        std::optional<std::string> id;
        Ogre::SceneNode* node1 {nullptr};
        Ogre::ManualObject* sphere1 {nullptr};
        Ogre::SceneNode* node2 {nullptr};
        Ogre::ManualObject* sphere2 {nullptr};
        Ogre::ManualObject* line {nullptr};
        Ogre::ManualObject* dashed_line {nullptr};
        Ogre::SceneNode* label_node {nullptr};
        sight::viz::scene3d::text::sptr label;
    };

    /// Stores picking information associated to the ruler.
    struct picked_ruler
    {
        ruler_ogre_set* m_data;
        bool m_first;
    };

    /// Vector of ruler_ogre_sets.
    std::vector<ruler_ogre_set> m_ruler_ogre_sets;

    /// Creates a ruler fiducial and add it to fiducial_sets.
    void create_ruler_fiducial(
        const std::array<float, 4> _color,
        const std::optional<std::string> _id,
        const std::array<double, 3> _begin,
        const std::array<double, 3> _end
    );

    /// Create a ruler_ogre_set and store it in m_ruler_ogre_sets.
    void create_ruler_ogre_set(
        const std::array<float, 4> _color,
        const float _sphere_radius,
        const std::optional<std::string> _id,
        const std::array<double, 3> _begin,
        const std::array<double, 3> _end,
        const bool _visible
    );

    /// SLOT: Activates the ruler tool by changing the cursor and updating a boolean.
    /// @param _activate set the state of ruler tool.
    void activate_tool(const bool _activate);

    /// Changes visibility of rulers.
    void set_visible(bool _visible) override;

    /// SLOT: Removes ogre elements from m_ruler_ogre_sets for the given id ruler.
    void remove_ruler_ogre_set(std::optional<std::string> _id);

    /// Removes fiducial ruler for the given id ruler.
    void remove_ruler_fiducial(std::optional<std::string> _id);

    /// SLOT: Removes all rulers and their associated fiducials data.
    void remove_all();

    /// SLOT: Removes all rulers and their associated fiducials data from the current slice.
    void remove_from_current_slice();

    /// SLOT: Displays ruler associated to the current slice.
    void display_on_current_slice();

    /**
     * @brief Retrieves the picked ruler and stores the result in m_picked_ruler.
     * @param _button mouse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void button_press_event(mouse_button _button, modifier _mod, int _x, int _y) override;

    /**
     * @brief Moves a ruler stored in m_picked_ruler.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void mouse_move_event(
        mouse_button /*_button*/,
        modifier _mod,
        int _x,
        int _y,
        int /*_dx*/,
        int /*_dy*/
    ) override;

    /// Resets m_picked_ruler.
    void button_release_event(
        mouse_button _button,
        modifier _mod,
        int _x,
        int _y
    ) override;

    /// Updates picked ruler on mouse movement.
    void update_picked_ruler(ruler_ogre_set* _ruler_to_update, Ogre::Vector3 _begin, Ogre::Vector3 _end);

    /// SLOT: Allows visualizing the creation and/or modification of rulers in multiple
    /// adaptors(Negato2d/Negato3d) by displaying the rulers synchronously.
    void update_modified_ruler(
        std::optional<std::string> _id,
        std::array<double, 3> _begin,
        std::array<double, 3> _end
    );

    /// Catches escape to go out of the ruler creation mode.
    void key_press_event(
        int _key,
        modifier /*_mods*/,
        int /*_mouseX*/,
        int /*_mouseY*/
    ) final;

    /// Changes the cursor type.
    void set_cursor(QCursor _cursor);

    /// Returns the color config or a generated color.
    Ogre::ColourValue get_default_color();

    /// Returns true if the given ruler is on current slice
    bool is_visible_on_current_slice(std::array<double, 3> _begin, std::array<double, 3> _end);

    using orientation_t = sight::data::helper::medical_image::orientation_t;

    orientation_t m_orientation {
        orientation_t::z_axis
    };

    /// Defines the radius of spheres.
    float m_sphere_radius {3.0F};

    /// Defines the font size in points.
    std::size_t m_font_size {16};

    /// Defines whether or not interactions are enabled with rulers.
    bool m_interactive {false};

    /// Defines whether the rulers are actually visible or not.
    bool m_visible {true};

    /// Defines the priority of the interactor.
    int m_priority {2};

    /// Counter to swap color at each new ruler.
    int m_color_index {0};

    /// Color extracted from the configuration.
    std::string m_color;

    // Defines if all rulers will always be displayed, regardless of the current slice.
    bool m_always_display_all {false};

    /// Defines the mask used to filter out entities when the ruler is auto snapped.
    std::uint32_t m_query_mask {0xFFFFFFFF};

    /// Defines the mask used to filter rulers, it optimizes the ray launched to retrieve the picked ruler.
    std::uint32_t m_query_flag {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Defines the material name with no depth check for spheres.
    std::string m_sphere_material_name;

    /// Defines the material name with depth check for lines.
    std::string m_line_material_name;

    /// Defines the material name with no depth check for dashed lines.
    std::string m_dashed_line_material_name;

    /// Contains the material with no depth check for spheres.
    sight::viz::scene3d::material::uptr m_sphere_material;

    /// Contains the material with depth check for lines.
    sight::viz::scene3d::material::uptr m_line_material;

    /// Contains the material with no depth check for dashed lines.
    sight::viz::scene3d::material::uptr m_dashed_line_material;

    /// Defines whether to start using the ruler creation tool or not.
    bool m_tool_activated {false};

    /// Defines whether a ruler is in creation or not.
    bool m_creation_mode {false};

    /// Indicates if is over a ruler
    bool m_is_over_ruler {false};

    /// Defines the current picked data, reset by buttonReleaseEvent(MouseButton, int, int).
    picked_ruler m_picked_ruler {nullptr, true};

    QPushButton* m_bin_button = nullptr;

    bool m_is_a_mouse_move = false;

    std::unique_ptr<delete_bin_button_when_focus_out> m_event_filter = nullptr;

    static constexpr std::string_view s_IMAGE_INOUT = "image";
    sight::data::ptr<sight::data::image_series, sight::data::access::inout> m_image {this, s_IMAGE_INOUT};
};

} // sight::module::viz::scene3d_qt::adaptor::fiducials
