/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include <core/macros.hpp>
#include <core/thread/timer.hpp>

#include <data/fiducials_series.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>
#include <data/material.hpp>

#include <QWidget>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/fiducials_configuration.hpp>
#include <viz/scene3d/material/standard.hpp>
#include <viz/scene3d/text.hpp>
#include <viz/scene3d/transformable.hpp>
namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

/**
 * @brief This adaptor displays fiducials with "point" shape. Other shapes will be ignored by this adaptor.
 *
 * @section Slots Slots
 * - \b remove_fiducials(): removes all fiducials.
 * - \b remove_visible_fiducials(): removes all visible fiducials.
 * - \b set_current_group(std::string): set the current group
 * - \b configure_fiducials(): Configure fiducial size, shape and color used when adding fiducials.
 * - \b enable_edit_mode(): enables the edit mode.
 * - \b disable_edit_mode(): disables the edit mode.
 * - \b enable_move_mode(): enables the move mode.
 * - \b disable_move_mode(): disables the move mode.
 * - \b update_visibility(bool): shows or hides the fiducials.
 * - \b toggle_visibility(): toggles whether the fiducials are shown or not.
 * - \b show(): shows the fiducials.
 * - \b hide(): hides the fiducials.
 *
 * @section Signals Signals
 * - \b send_world_coord(double, double, double): sends world coordinates of current selected fiducials (by double
 * click).
 * - \b edit_mode_changed(bool): sent if the edit mode changed (typically via Escape key press or right mouse click).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d_qt::adaptor::fiducials::point">
        <inout key="image" uid="..." />
        <config transform="transformUID" visible="true" priority="2" />
    </service>
   @endcode
 *
 * @subsection Inout Inout:
 * - \b image [sight::data::image_series]: holds the fiducials data.
 *
 * @subsection Configuration Configuration:
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified
 * - \b visible (optional, default=true): the visibility of the fiducials.
 * - \b font_size (optional, unsigned int, default=16): font size in points.
 * - \b label (optional, bool, default=true): display label.
 * - \b orientation (optional, axial/frontal/sagittal, default=axial): orientation of the negato.
 * - \b interactive (optional, bool, default=true): enables interactions with fiducials.
 * - \b priority (optional, int, default=2): priority of the interactor.
 * - \b query_mask (optional, uint32, default=0xFFFFFFFF): mask used to filter out entities when the distance is auto
 *      snapped.
 * - \b fiducials_query_flags (optional, uint32, default=0x40000000): mask applied to fiducials.
 * - \b view_distance (optional, slices_in_range/current_slice/all_slices, default=slices_in_range): on which slices to
 *      display the fiducials.
 * - \b initial_group (optional, string, default="Group_0"): initial name of the current group.
 * - \b initial_color (optional, string, default="#FFFF00FF"): initial color of the current group.
 * - \b initial_size (optional, float, default=32.0): initial size of the current group.
 * - \b initial_shape (optional, sphere/cube, default="sphere): initial shape of the current group.
 * - \b allow_renaming (optional, bool, default=true): true if the user can rename fiducials
 * - \b modify (optional, all/group, default="all"): if "all", all the fiducials can be modified, ignoring the current
        group; if "group", only the fiducials belonging to the current group can be modified.
 */
class point final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable,
    public sight::viz::scene3d::interactor::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(point, sight::viz::scene3d::adaptor);

    /**
     * @brief Construct a new point object
     *
     */
    point() noexcept;

    /**
     * @brief Destroy the point object
     *
     */
    ~point() noexcept final = default;

    struct slots final
    {
        using key_t = sight::core::com::slots::key_t;

        inline static const key_t REMOVE_FIDUCIALS         = "remove_all";
        inline static const key_t REMOVE_VISIBLE_FIDUCIALS = "remove_landmarks";
        inline static const key_t SET_CURRENT_GROUP        = "set_current_group";
        inline static const key_t CONFIGURE_FIDUCIALS      = "configure_landmarks";

        inline static const key_t ENABLE_EDIT_MODE  = "enableEditMode";
        inline static const key_t DISABLE_EDIT_MODE = "disableEditMode";
        inline static const key_t TOGGLE_EDIT_MODE  = "toggleEditMode";
        inline static const key_t CHANGE_EDIT_MODE  = "change_edit_mode";
        inline static const key_t ENABLE_MOVE_MODE  = "enableMoveMode";
        inline static const key_t DISABLE_MOVE_MODE = "disableMoveMode";
        inline static const key_t TOGGLE_MOVE_MODE  = "toggleMoveMode";
        inline static const key_t CHANGE_MOVE_MODE  = "change_move_mode";
    };

    /**
     * @brief SLOT: Remove all fiducials.
     */
    void remove_fiducials();

    /**
     * @brief SLOT: Remove all visible fiducials.
     */
    void remove_visible_fiducials();

    /**
     * @brief SLOT: Set the current group name.
     * @param _group_name the new current group name.
     */
    void set_current_group(std::string _group_name);

    /**
     * @brief SLOT: Configure fiducial size, shape and color used when adding fiducials.
     */
    void configure_fiducials(sight::viz::scene3d::fiducials_configuration _configuration);

    /**
     * @brief SLOT: Enable edit mode.
     */
    void enable_edit_mode();

    /**
     * @brief SLOT: Disable edit mode.
     *
     */
    void disable_edit_mode();

    /**
     * @brief SLOT: Enable move mode.
     */
    void enable_move_mode();

    /**
     * @brief SLOT: Disable move mode.
     */
    void disable_move_mode();

    struct signals final
    {
        using key_t = sight::core::com::signals::key_t;

        /// Signal send when double clicked on a landmark, send its world coordinates;
        inline static const key_t SEND_WORLD_COORD = "send_world_coord";
        using send_world_coordinates_t = core::com::signal<void (double, double, double)>;

        inline static const key_t EDIT_MODE_CHANGED = "edit_mode_changed";
        using edit_mode_changed_t = core::com::signal<void (bool)>;
    };

    /// Fiducial viewing range.
    enum class view_distance : std::uint8_t
    {
        slices_in_range = 0,
        current_slice,
        all_slices
    };

protected:

    /**
     * @brief Configure the adaptor.
     *
     */
    void configuring() final;

    /**
     * @brief Initialize Ogre resources.
     *
     */
    void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     *
     * @return A map of each proposed connection.
     */
    service::connections_t auto_connections() const final;

    /**
     * @brief Redraw all fiducials.
     *
     */
    void updating() final;

    /**
     * @brief Free Ogre resources.
     *
     */
    void stopping() final;

    /**
     * @brief Sets the adaptor visibility.
     *
     * @param _visible If false, it will hide all fiducials. If true, it will show them if possible.
     */
    void set_visible(bool _visible) final;

    /**
     * @brief Retrieves a fiducial or create a new one. Stores the result in m_picked_data.
     *
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void button_press_event(mouse_button _button, modifier _mod, int _x, int _y) final;

    /**
     * @brief Moves a fiducial stored in m_picked_data.
     *
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @param _dx width displacement of the mouse since the last event.
     * @param _dx height displacement of the mouse since the last event.
     */
    void mouse_move_event(mouse_button _button, modifier _mod, int _x, int _y, int _dx, int _dy) final;

    /**
     * @brief Resets m_picked_data.
     *
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void button_release_event(mouse_button _button, modifier _mod, int _x, int _y) final;

    /**
     * @brief Listens to mouse buttons being double pressed.
     *
     * @param _button pressed mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    void button_double_press_event(mouse_button _button, modifier _mods, int _x, int _y) final;

    /**
     * @brief Hides the contextual menu when the mouse wheel is used.
     */
    void wheel_event(modifier _mods, double _angle_delta, int _x, int _y) final;

    /**
     * @brief Exit edit mode if the Escape key is pressed
     *
     * @param _key
     * @param _mods
     * @param _mouse_x
     * @param _mouse_y
     */
    void key_press_event(int _key, modifier _mods, int _mouse_x, int _mouse_y) final;

private:

    /// Stores data used to display one landmark.
    struct ogre_fiducial final
    {
        ogre_fiducial(
            Ogre::SceneNode* _node,
            Ogre::ManualObject* _manual_object,
            std::string _group_name,
            std::size_t _index,
            sight::viz::scene3d::text::sptr _label
        ) :
            m_node(_node),
            m_manual_object(_manual_object),
            m_group_name(_group_name),
            m_index(_index),
            m_label(_label)
        {
        }

        /// Contains the node of the landmark
        Ogre::SceneNode* m_node {nullptr};

        /// Contains the manual object that represent the landmark
        Ogre::ManualObject* m_manual_object {nullptr};

        /// Defines the group name of the landmark
        std::string m_group_name;

        /// Defines the index of the landmark
        std::size_t m_index {0};

        /// Defines the text label of the landmark (can be nullptr)
        sight::viz::scene3d::text::sptr m_label;

        //// Contains the slots related to the landmark
        std::vector<std::shared_ptr<core::com::slot_base> > m_slots;
    };

    /// Stores data used to highlight the selected landmark.
    struct selected_ogre_fiducial final
    {
        selected_ogre_fiducial(core::thread::timer::sptr _timer, std::shared_ptr<ogre_fiducial> _ogre_fiducial) :
            m_timer(_timer),
            m_ogre_fiducial(_ogre_fiducial)
        {
        }

        core::thread::timer::sptr m_timer;
        std::shared_ptr<ogre_fiducial> m_ogre_fiducial;
        bool m_show {false};
    };

    /**
     * @brief SLOT: Removes all ogre_fiducials of a group.
     *
     * @param _group_name name of the group to remove.
     */
    void remove_group(std::string _group_name);

    /**
     * @brief SLOT: Updates all ogre_fiducials of a group.
     *
     * @param _group_name name of the group to update.
     */
    void modify_group(std::string _group_name);

    /**
     * @brief SLOT: Updates all ogre_fiducials of a group.
     *
     * @param _old_group_name old group name.
     * @param _new_group_name new group name.
     */
    void rename_group(std::string _old_group_name, std::string _new_group_name);

    /**
     * @brief SLOT: Creates ogre_fiducial from the last fiducial of a group.
     *
     * @param _group_name group name.
     */
    void add_point(std::string _group_name);

    /**
     * @brief SLOT: Creates ogre_fiducial from a specific fiducial of a group.
     *
     * @param _group_name group name.
     * @param _index "shape" index of the fiducial in a fiducial_set relative to the group.
     */
    void insert_point(std::string _group_name, std::size_t _index);

    /**
     * @brief SLOT: Updates ogre_fiducial from a specific fiducial of a group.
     *
     * @param _group_name group name.
     * @param _index "shape" index of the fiducial in a fiducial_set relative to the group.
     */
    void modify_point(std::string _group_name, std::size_t _index);

    /**
     * @brief SLOT: Removes a specific ogre_fiducial.
     *
     * @param _group_name group name.
     * @param _index "shape" index of the fiducial in a fiducial_set relative to the group.
     */
    void remove_point(std::string _group_name, std::size_t _index);

    /**
     * @brief SLOT: Highlights the selected fiducial.
     *
     * @param _group_name group name
     * @param _index "shape" index of the fiducial in a fiducial_set relative to the group.
     */
    void select_point(std::string _group_name, std::size_t _index);

    /**
     * @brief SLOT: Resets highlights the selected fiducial.
     *
     * @param _group_name group name
     * @param _index "shape" index of the fiducial in a fiducial_set relative to the group.
     */
    void deselect_point(std::string _group_name, std::size_t _index);

    /**
     * @brief Changes the slice type. Update fiducials visibility.
     *
     * @param _from The current slice type.
     * @param _to The new slice type.
     */
    void change_slice_type(int _from, int _to);

    /**
     * @brief Changes the slice index. Update fiducials visibility.
     *
     * @param _axial_index The new axial slice index.
     * @param _frontal_index The new frontal slice index.
     * @param _sagittal_index The new sagittal slice index.
     */
    void change_slice_index(int _axial_index, int _frontal_index, int _sagittal_index);

    /**
     * @brief Gets the normalized camera direction vector.
     *
     * @param _cam camera from which to extract the direction vector.
     * @return A vector representing the camera direction
     */
    static Ogre::Vector3 get_cam_direction(const Ogre::Camera* _cam);

    /**
     * @brief Remove all ogre_fiducials. Free Ogre resources.
     */
    void remove_ogre_fiducials();

    /**
     * @brief Emit the removed signals and destroy the associated ogre_fiducials.
     *
     * @param _removed_results The removed fiducials information.
     * @param _removed_fiducial_sets The removed fiducial set names.
     * @param _image_series The locked image series.
     * @param _remove_ogre_fiducials If true, removes also the ogre fiducials.
     */
    void emit_removed_signals(
        const std::vector<data::fiducials_series::query_result>& _removed_results,
        const std::set<std::string>& _removed_fiducial_sets,
        const data::image_series& _image_series,
        bool _remove_ogre_fiducials = false
    );

    /**
     * @brief Create an ogre_fiducial. Allocate ogre resources.
     *
     * @param _group_name The name of the group
     * @param _index The index of the fiducial
     * @param _private_shape The private shape of the fiducial
     * @param _color The color of the fiducial
     * @param _size The size of the fiducial
     * @param _visible The visibility of the fiducial
     * @param _position The position of the fiducial
     * @return std::shared_ptr<ogre_fiducial> The created ogre fiducial
     */
    std::shared_ptr<ogre_fiducial> create_ogre_fiducial(
        const std::string& _group_name,
        std::size_t _index,
        data::fiducials_series::private_shape _private_shape,
        std::array<float, 4> _color,
        float _size,
        bool _visible,
        std::vector<double> _position
    );

    /**
     * @brief Wrapper function to create an ogre_fiducial from a query_result.
     *
     * @param _query The fiducial information
     * @param _image_series The Locked image series
     * @return std::shared_ptr<ogre_fiducial> The created ogre fiducial
     */
    std::shared_ptr<ogre_fiducial> create_ogre_fiducial(
        const data::fiducials_series::query_result& _query,
        const data::image_series& _image_series
    );

    /**
     * @brief Destroy ogre_fiducials from m_ogre_fiducials and free ogre resources.
     *
     * @param _group_name The group name of the fiducial
     * @param _index The index of the fiducial within the group
     * @return std::size_t The number of erased fiducials
     */
    std::size_t destroy_ogre_fiducials(
        const std::string& _group_name,
        const std::optional<std::size_t>& _index = std::nullopt,
        const bool _rebuild_indexes              = false
    );

    /**
     * @brief Manages the highlighting of the fiducial at the given index.
     *
     * @param _selected_ogre_fiducial which _selected_fiducial to manage.
     */
    void highlight(std::shared_ptr<selected_ogre_fiducial> _selected_ogre_fiducial);

    /**
     * @brief Create a fiducial and pick it. Called by the mouse event (pick) and the slot (no pick).
     *
     * @param _point The position of the fiducial
     * @param _pick If true, the fiducial is picked
     */
    void create_and_pick_fiducial(const std::vector<double>& _point, bool _pick = true);

    /**
     * @brief Check the number of fiducials.
     *
     * @return true if the maximum number of fiducials is reached.
     */
    bool is_max_fiducials_reached();

    /**
     * @brief Wrapper function to check the visibility of a fiducial.
     *
     * @param _fiducial_position The position of the fiducial
     * @param _fiducial_size The size of the fiducial
     * @param _image The locked image series
     * @return true if the fiducial is visible
     */
    bool check_fiducial_visibility(
        const std::vector<double>& _fiducial_position,
        float _fiducial_size,
        const data::image_series& _image
    ) const;

    /**
     * @brief Wrapper function to check the visibility of a fiducial.
     *
     * @param _query The fiducial information
     * @param _image_series The locked image series
     * @param _base_visibility The base visibility of the fiducial (group/adaptor)
     * @return true if the fiducial is visible
     */
    bool check_fiducial_visibility(
        const data::fiducials_series::query_result& _query,
        const data::image_series& _image_series,
        const std::optional<bool>& _base_visibility = std::nullopt
    ) const;

    /**
     * @brief Launch a ray to pick a ogre_fiducial.
     *
     * @param _x The screen x coordinate
     * @param _y The screen y coordinate
     * @param _for_modification True if the fiducial is picked for modification
     * @return std::shared_ptr<ogre_fiducial>
     */
    std::shared_ptr<ogre_fiducial> try_pick(int _x, int _y, bool _for_modification = true) const;

    /**
     * @brief Set the cursor shape.
     *
     * @param _cursor The new cursor shape, std::nullopt restore the previous shape
     */
    void set_cursor(const std::optional<Qt::CursorShape>& _cursor = std::nullopt);

    /// Contains the root scene node.
    Ogre::SceneNode* m_transform_node {nullptr};

    /// Contains the Ogre material adaptor.
    sight::viz::scene3d::material::standard::uptr m_material {nullptr};

    /// Stores each Ogre fiducial points.
    std::set<std::shared_ptr<ogre_fiducial> > m_ogre_fiducials;

    /// Enables labels.
    bool m_enable_labels {true};

    /// Defines the label font size in points.
    std::size_t m_font_size {12};

    /// Stores informations about the selected landmark.
    std::list<std::shared_ptr<selected_ogre_fiducial> > m_selected_ogre_fiducials;

    /// Define a mutex to synchronized methods working with selected landmark.
    std::mutex m_selected_mutex;

    /// Defines the image orientation.
    using axis_t = data::helper::medical_image::axis_t;
    axis_t m_axis {axis_t::z_axis};

    /// Defines the view distance of the fiducials.
    view_distance m_view_distance {view_distance::slices_in_range};

    /// Defines whether or not interactions are enabled with distances.
    bool m_interactive {true};

    /// Defines the priority of the interactor.
    int m_priority {2};

    /// Defines the current picked data, reset by buttonReleaseEvent(MouseButton, int, int).
    std::shared_ptr<ogre_fiducial> m_picked_data {nullptr};

    /// Defines the mask used to filter out entities.
    std::uint32_t m_query_mask {0xFFFFFFFF};

    /// Defines the mask used to filter fiducials, it optimizes the ray launched to retrieve the picked fiducial.
    std::uint32_t m_fiducials_query_flag {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Initial group configuration
    /// @{
    static constexpr std::string_view INITIAL_GROUP_NAME = "Group_0";
    std::string m_current_group {INITIAL_GROUP_NAME};
    std::string m_initial_group {INITIAL_GROUP_NAME};
    std::array<float, 4> m_current_color {1.0F, 1.0F, 0.0F, 1.0F};
    float m_current_size {32.0F};
    sight::data::fiducials_series::private_shape m_current_shape {sight::data::fiducials_series::private_shape::sphere};
    /// @}

    /// Allow to limit the number of fiducials
    /// @{
    std::map<std::string, std::size_t> m_group_max;
    std::optional<size_t> m_slice_max {std::nullopt};
    std::optional<size_t> m_total_max {std::nullopt};
    /// @}

    std::set<std::string> m_movable_groups;

    /// Adaptor edit "mode":
    /// - DISPLAY is read-only,
    /// - MOVE allows to modify existing fiducials position
    /// - EDIT allows to add/remove and move fiducials
    enum edit_mode : std::uint8_t
    {
        display = 1 << 0,
        move    = 1 << 1,
        edit    = 1 << 2
    };

    std::uint8_t m_edit_mode {std::uint8_t(edit_mode::display)};

    /// Whether all fiducials can be modified or only fiducials belonging to the current group
    bool m_can_only_modify_current {false};

    /// True if the fiducials can be renamed by the user
    bool m_renaming_allowed {true};

    /// True if we must show the contextual menu. It must be shown if the fiducial already existed and wasn't moved.
    bool m_must_show_contextual_menu {false};

    QWidget* m_contextual_menu {nullptr};

    /// Auto-delete the event filter in the end
    std::unique_ptr<QObject> m_event_filter {nullptr};

    sight::data::ptr<sight::data::image_series, sight::data::access::inout> m_image_series {this, "imageSeries"};
};

} // namespace sight::module::viz::scene3d_qt::adaptor.
