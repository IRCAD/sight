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

#include "data/fiducials_series.hpp"

#include "modules/viz/scene3d_qt/config.hpp"

#include <data/point_list.hpp>

#include <geometry/data/point_list.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/base.hpp>
#include <viz/scene3d/material.hpp>
#include <viz/scene3d/text.hpp>

#include <Ogre.h>
#include <QPushButton>

#include <memory>
#include <optional>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

/**
 * @brief This adaptor displays distances retrieved from the image fields.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::fiducials::ruler" auto_connect="true" >
        <inout key="image" uid="..." />
        <config fontSource="DejaVuSans.ttf" fontSize="32" radius="4.5" priority="2" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::image]: image containing the distance field.
 *
 * @subsection Configuration Configuration:
 * - \b fontSize (optional, int, default=16): font size in points.
 * - \b radius (optional, float, default=4.5): size of the distances spheres.
 * - \b interactive (optional, bool, default=true): enables interactions with distances.
 * - \b priority (optional, int, default=2): priority of the interactor.
 * - \b queryMask (optional, uint32, default=0xFFFFFFFF): mask used to filter out entities when the distance is auto
 *      snapped.
 * - \b distanceQueryFlags (optional, uint32, default=0x40000000): mask apply to distances spheres.
 * - \b color (optional, string, default=""): color applied to the rulers, generated if empty.
 *
 * @section Slots Slots
 * - \b remove_all(): remove all distances.
 * - \b removeDistance(): removes distances contained in the image from the scene manager.
 * - \b updateVisibilityFromField(): updates the visibility of distances from the field status.
 * - \b update_visibility(bool): sets whether distances are shown or not.
 * - \b updateModifiedDistance(data::point_list::sptr): renders distances in other negato2d and negato3d adaptors.
 * - \b toggle_visibility(): toggles whether distances are shown or not.
 * - \b show(): shows distance.
 * - \b hide(): hides distance.
 *
 * @section Signals Signals
 * - \b toolDeactivate(): signal set when the tool is deactivated.
 *
 *
 */
class ruler final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::interactor::base
{
public:

    struct signals
    {
        using key_t = sight::core::com::signals::key_t;
        static inline const key_t DEACTIVATE_DISTANCE_TOOL = "deactivate_distance_tool";

        using void_signal_t = sight::core::com::signal<void ()>;
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(ruler, sight::viz::scene3d::adaptor);

    /// Initialize slots.
    MODULE_VIZ_SCENE3D_QT_API ruler() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_QT_API ~ruler() noexcept override = default;

    struct MODULE_VIZ_SCENE3D_QT_CLASS_API slots final
    {
        using key_t = sight::core::com::slots::key_t;

        inline static const key_t REMOVE_ALL                          = "remove_all";
        inline static const key_t REMOVE_DISTANCES                    = "removeDistances";
        inline static const key_t REMOVE_DISTANCES_FROM_CURRENT_SLICE = "remove_distances_from_current_slice";
        inline static const key_t UPDATE_VISIBILITY_FROM_FIELDS       = "updateVisibilityFromField";
        inline static const key_t ACTIVATE_DISTANCE_TOOL              = "activate_distance_tool";
        inline static const key_t UPDATE_MODIFIED_DISTANCE            = "updateModifiedDistance";
        inline static const key_t RESTRICT_TO_CURRENT_SLICE           = "restrict_to_current_slice";
    };

    /**
     * @brief Retrieves the picked distance and stores the result in m_pickedData.
     * @param _button mouse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    MODULE_VIZ_SCENE3D_QT_API void button_press_event(mouse_button _button, modifier _mod, int _x, int _y) override;

    /**
     * @brief Moves a distance stored in m_pickedData.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    MODULE_VIZ_SCENE3D_QT_API void mouse_move_event(
        mouse_button /*_button*/,
        modifier _mod,
        int _x,
        int _y,
        int /*_dx*/,
        int /*_dy*/
    ) override;

    /// Resets m_pickedData.
    MODULE_VIZ_SCENE3D_QT_API void button_release_event(
        mouse_button _button,
        modifier _mod,
        int _x,
        int _y
    ) override;

    /// catch escape to go out of add distance mode
    MODULE_VIZ_SCENE3D_QT_API void key_press_event(
        int _key,
        modifier /*_mods*/,
        int /*_mouseX*/,
        int /*_mouseY*/
    ) final;

    /// Hides the contextual menu when the mouse wheel is used
    MODULE_VIZ_SCENE3D_QT_API void wheel_event(modifier _mods, double _angle_delta, int _x, int _y) final;

protected:

    /// Configures the service.
    MODULE_VIZ_SCENE3D_QT_API void configuring() override;

    /// Adds the interactor to the layer and creates the material.
    MODULE_VIZ_SCENE3D_QT_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     *
     * Connect data::image::DISTANCE_REMOVED_SIG to REMOVE_DISTANCE_SLOT
     * Connect data::image::DISTANCE_DISPLAYED_SIG to UPDATE_VISIBILITY_SLOT
     * Connect data::image::MODIFIED_SIG to service::slots::UPDATE
     */
    MODULE_VIZ_SCENE3D_QT_API connections_t auto_connections() const override;

    /// Updates materials and all distances.
    MODULE_VIZ_SCENE3D_QT_API void updating() override;

    /// Removes the interactor from the layer and destroys Ogre resources.
    MODULE_VIZ_SCENE3D_QT_API void stopping() override;

    /**
     * @brief Sets distances visibility.
     * @param _visible the visibility status of distances.
     */
    MODULE_VIZ_SCENE3D_QT_API void set_visible(bool _visible) override;

    MODULE_VIZ_SCENE3D_QT_API void set_visible(std::string _id, bool _visible);

private:

    /// Stores Ogre resources used to display a distance.
    /// Two spheres each attached to a node, a label to display millimeters,
    /// one line rendered with the depth check and a dashed line rendered without depth check.
    /// The point list is used to update each points when the interactor move a distance sphere,
    /// it is retrieved from the image via a field.
    struct distance_data
    {
        data::point_list::sptr m_point_list;
        Ogre::SceneNode* m_node1 {};
        Ogre::ManualObject* m_sphere1 {};
        Ogre::SceneNode* m_node2 {};
        Ogre::ManualObject* m_sphere2 {};
        Ogre::ManualObject* m_line {};
        Ogre::ManualObject* m_dashed_line {};
        Ogre::SceneNode* m_label_node {};
        sight::viz::scene3d::text::sptr m_label;
    };

    /// Stores picking information.
    struct picked_data
    {
        distance_data* m_data;
        bool m_first;
    };

    /// Map each distances to there related list ID.
    using distance_map = std::map<core::tools::id::type, distance_data>;

    class delete_bin_button_when_focus_out : public QObject
    {
    public:

        explicit delete_bin_button_when_focus_out(ruler* _ruler);
        bool eventFilter(QObject* _o, QEvent* _e) override;
        ruler* m_ruler;
    };

    /// Saves the created distances to image's field
    void update_image_distance_field(data::point_list::sptr _pl);

    /// SLOT: Remove all distances
    void remove_all();

    /// Retrieves distances from the image and remove them from the scene.
    void remove_distances();

    /// SLOT: Remove only distances from the current slice
    void remove_distances_from_current_slice();

    /// Updates distances visibility from the image field.
    void update_visibility_from_field();

    /// Activates the distance tool by changing the cursor and updating a boolean
    void activate_distance_tool(bool _activate);

    /**
     * @brief Gets the nearest picked position if there is one.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @return The picked world coordinates.
     */
    std::optional<Ogre::Vector3> get_nearest_picked_position(int _x, int _y);

    /**
     * @brief Creates a distance and add it into m_distances.
     * @param _pl The point list used to create the distance.
     */
    void create_distance(data::point_list::sptr& _pl);

    /**
     * @brief Updates a distance.
     * @param _data Distance to update.
     * @param _begin New begin position.
     * @param _end New end position
     */
    void update_distance(const distance_data* _data, Ogre::Vector3 _begin, Ogre::Vector3 _end);

    /**
     * @brief Destroys a distance from its ID and remove it from m_distances.
     * @param _id ID of the distance.
     */
    void destroy_distance(core::tools::id::type _id);

    /// Slot: Allows visualizing the creation and/or modification of distances field of an image in multiple
    /// adaptors(Negato2d/Negato3d) by displaying the distances synchronously.
    void update_modified_distance(data::point_list::sptr _pl);

    void set_cursor(QCursor _cursor);

    void remove_distance(data::point_list::sptr _pl);

    void update_from_fiducials();

    void restrict_to_current_slice();

    /// Defines the radius of distances spheres.
    float m_sphere_radius {3.5F};

    /// Defines the font size in points.
    std::size_t m_font_size {12};

    /// Defines whether or not interactions are enabled with distances.
    bool m_interactive {true};

    /// Defines whether to start using the distance creation tool or not.
    bool m_tool_activated {false};

    /// Defines whether the distances are actually visible or not.
    bool m_visible {true};

    /// Defines whether a distance is in creation or not.
    bool m_creation_mode {false};

    /// @brief  indicates if is over a distance
    bool m_is_over_distance {false};

    /// Defines the priority of the interactor.
    int m_priority {2};

    /// Counter to swap color at each new distance
    int m_color_index {0};

    /// Color extracted from the configuration
    std::string m_config_color {""};

    /// Defines the current picked data, reset by buttonReleaseEvent(MouseButton, int, int).
    picked_data m_picked_data {nullptr, true};

    /// Defines the mask used to filter out entities when the distance is auto snapped.
    std::uint32_t m_query_mask {0xFFFFFFFF};

    /// Defines the mask used to filter distances, it optimizes the ray launched to retrieve the picked distance.
    std::uint32_t m_distance_query_flag {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Defines the material name with no depth check for spheres.
    std::string m_sphere_material_name;

    /// Defines the material name with depth check for lines.
    std::string m_line_material_name;

    /// Defines the material name with no depth check for dashed lines.
    std::string m_dashed_line_material_name;

    /// Contains the material with no depth check for spheres.
    sight::viz::scene3d::material::uptr m_sphere_material {nullptr};

    /// Contains the material with depth check for lines.
    sight::viz::scene3d::material::uptr m_line_material {nullptr};

    /// Contains the material with no depth check for dashed lines.
    sight::viz::scene3d::material::uptr m_dashed_line_material {nullptr};

    /// Stores all generated distances.
    distance_map m_distances;
    std::vector<data::point::sptr> m_points;

    // Cached fiducials from the input image
    std::vector<sight::data::fiducials_series::fiducial> m_cached_fiducials;

    QPushButton* m_bin_button = nullptr;

    bool m_is_a_mouse_move = false;

    std::unique_ptr<delete_bin_button_when_focus_out> m_event_filter = nullptr;

    static constexpr std::string_view IMAGE_INOUT = "image";
    sight::data::ptr<sight::data::image, sight::data::access::inout> m_image {this, IMAGE_INOUT, true};
};

} // namespace sight::module::viz::scene3d_qt::adaptor
