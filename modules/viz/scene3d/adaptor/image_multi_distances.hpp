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

#include <data/point_list.hpp>

#include <geometry/data/point_list.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/base.hpp>
#include <viz/scene3d/material.hpp>
#include <viz/scene3d/text.hpp>

#include <Ogre.h>

#include <memory>
#include <optional>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays distances retrieved from the image fields.
 * @deprecated Use sight::module::viz::scene3d_qt::adaptor::image_multi_distances instead.

 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::image_multi_distances" auto_connect="true" >
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
 *
 * @section Slots Slots
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
class image_multi_distances final :
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
    SIGHT_DECLARE_SERVICE(image_multi_distances, sight::viz::scene3d::adaptor);

    /// Initialize slots.
    image_multi_distances() noexcept;

    /// Destroys the adaptor.
    ~image_multi_distances() noexcept override = default;

    /**
     * @brief Retrieves the picked distance and stores the result in m_pickedData.
     * @param _button mouse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void button_press_event(mouse_button _button, modifier _mod, int _x, int _y) override;

    /**
     * @brief Moves a distance stored in m_pickedData.
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

    /// Resets m_pickedData.
    void button_release_event(
        mouse_button _button,
        modifier _mod,
        int _x,
        int _y
    ) override;

    /// catch escape to go out of add distance mode
    void key_press_event(int _key, modifier /*_mods*/, int /*_mouseX*/, int /*_mouseY*/) final;

    /// catch the mouse leaving the widget
    void leave_event() final;

    /// catch the mouse entering the widget
    void enter_event() final;

protected:

    /// Configures the service.
    void configuring() override;

    /// Adds the interactor to the layer and creates the material.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     *
     * Connect data::image::DISTANCE_REMOVED_SIG to REMOVE_DISTANCE_SLOT
     * Connect data::image::DISTANCE_DISPLAYED_SIG to UPDATE_VISIBILITY_SLOT
     * Connect data::image::MODIFIED_SIG to service::slots::UPDATE
     */
    connections_t auto_connections() const override;

    /// Updates materials and all distances.
    void updating() override;

    /// Removes the interactor from the layer and destroys Ogre resources.
    void stopping() override;

    /**
     * @brief Sets distances visibility.
     * @param _visible the visibility status of distances.
     */
    void set_visible(bool _visible) override;

private:

    /// Stores Ogre resources used to display a distance.
    /// Two spheres each attached to a node, a label to display millimeters,
    /// one line rendered with the depth check and a dashed line rendered without depth check.
    /// The point list is used to update each points when the interactor move a distance sphere,
    /// it's retrieve from the image via a field.
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
    using distance_map = std::map<core::id::type, distance_data>;

    /**
     * @brief Generates a dashed line in a Ogre::ManualObject.
     * @param _object Object where generate the dashed line.
     * @param _begin Begin position of the line.
     * @param _end End position of the line.
     * @param _thickness Thickness of dash.
     */
    static void generate_dashed_line(
        Ogre::ManualObject* _object,
        const Ogre::Vector3& _begin,
        const Ogre::Vector3& _end,
        float _thickness
    );

    /// Saves the created distances to image's field
    void update_image_distance_field(data::point_list::sptr _pl);

    /// Retrieves distances from the image and remove them from the scene.
    void remove_distances();

    /// Updates distances visibility from the image field.
    void update_visibility_from_field();

    /// Activates the distance tool by changing the cursor and updating a boolean
    void activate_distance_tool(bool _activate);

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
    void destroy_distance(core::id::type _id);

    /// Slot: Allows visualizing the creation and/or modification of distances field of an image in multiple
    /// adaptors(Negato2d/Negato3d) by displaying the distances synchronously.
    void update_modified_distance(data::point_list::sptr _pl);

    /// Defines the radius of distances spheres.
    float m_distance_sphere_radius {3.5F};

    /// Defines the font size in points.
    std::size_t m_font_size {12};

    /// Defines whether or not interactions are enabled with distances.
    bool m_interactive {true};

    /// Defines whether to start using the distance creation tool or not.
    bool m_tool_activated {false};

    /// Defines whether a distance is in creation or not.
    bool m_creation_mode {false};

    /// @brief  indicates if is over a distance
    bool m_is_over_distance {false};

    /// Defines the priority of the interactor.
    int m_priority {2};

    /// Counter to swap color at each new distance
    int m_color_index {0};

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

    static constexpr std::string_view IMAGE_INOUT = "image";
    sight::data::ptr<sight::data::image, sight::data::access::inout> m_image {this, IMAGE_INOUT, true};
};

} // namespace sight::module::viz::scene3d::adaptor.
