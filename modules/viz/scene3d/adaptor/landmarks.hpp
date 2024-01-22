/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/material.hpp"
#include "modules/viz/scene3d/config.hpp"

#include <core/macros.hpp>
#include <core/thread/timer.hpp>

#include <data/helper/medical_image.hpp>
#include <data/landmarks.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/text.hpp>
#include <viz/scene3d/transformable.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays landmarks.
 * @deprecated Use sight::module::viz::scene3d_qt::adaptor::landmarks instead.
 *
 * @section Slots Slots
 * - \b remove_all(): removes all groups.
 * - \b remove_group(std::string): removes an entire group.
 * - \b modifyGroup(std::string): removes an entire group and re-create it.
 * - \b rename_group(std::string, std::string): replaces and old group's name by a new one.
 * - \b add_point(std::string): adds the last point of a landmarks group.
 * - \b removePoint(std::string, std::size_t): removes a point.
 * - \b insertPoint(std::string, std::size_t): inserts a point.
 * - \b selectPoint(std::string, std::size_t) hightlights the selected landmark.
 * - \b deselectPoint(std::string, std::size_t): resets the hightlighting of the selected landmark.
 * - \b initializeImage(): initializes image slices index if there is one.
 * - \b changeSliceType(int, int): updates the image slice type.
 * - \b changeSliceIndex(int, int, int): updates the image slice index to show or hide landmarks.
 * - \b update_visibility(bool): shows or hides the landmarks.
 * - \b toggle_visibility(): toggles whether the landmarks are shown or not.
 * - \b show(): shows the landmarks.
 * - \b hide(): hides the landmarks.
 * - \b toggle_add_landmarks(bool): Allow to add landmarks by clicking on the scene.
 * - \b toggleRemoveLandmarks(bool): Allow to remove landmarks by clicking on the scene.
 * - \b remove_landmarks(): Remove all visible landmarks.
 *                         Which landmarks are removed depends of the "viewDistance" parameters.
 * - \b configure_landmarks(): Configure the new landmarks size, shape and color used when adding landmarks.
 *
 * @section Signals Signals
 * - \b send_world_coord(double, double, double): sends world coordinates of current selected landmarks (by double
 * click).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::landmarks">
        <inout key="landmarks" uid="..." />
        <in key="image" uid="..." />
        <config transform="transformUID" visible="true" priority="2" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b landmarks [sight::data::landmarks]: landmarks to display.
 *
 * @subsection Input Input:
 * - \b image [sight::data::image] (optional): if the image is used, each landmark will be displayed only if the
 *      image slice is on it.
 *
 * @subsection Configuration Configuration:
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified
 * - \b fontSize (optional, unsigned int, default=16): font size in points.
 * - \b label (optional, bool, default=true): display label.
 * - \b orientation (optional, axial/frontal/sagittal, default=axial): orientation of the negato.
 * - \b visible (optional, default=true): the visibility of the landmarks.
 * - \b interactive (optional, bool, default=true): enables interactions with landmarks.
 * - \b priority (optional, int, default=2): priority of the interactor.
 * - \b queryMask (optional, uint32, default=0xFFFFFFFF): mask used to filter out entities when the distance is auto
 *      snapped.
 * - \b landmarksQueryFlags (optional, uint32, default=0x40000000): mask applied to landmarks.
 * - \b viewDistance (optional, slicesInRange/currentSlice/allSlices, default=slicesInRange): on which slices to display
 *      the landmarks.
 * - \b initialGroup (optional, string, default="Landmarks"): initial name of the current group.
 * - \b initialColor (optional, string, default="#FFFF00FF"): initial color of the current group.
 * - \b initialSize (optional, float, default=32.0): initial size of the current group.
 * - \b initialShape (optional, sphere/cube, default="sphere): initial shape of the current group.
 *
 */
class MODULE_VIZ_SCENE3D_CLASS_API landmarks final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable,
    public sight::viz::scene3d::interactor::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(landmarks, sight::viz::scene3d::adaptor);

    /// Creates the adaptor.
    MODULE_VIZ_SCENE3D_API landmarks() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~landmarks() noexcept final = default;

    struct MODULE_VIZ_SCENE3D_CLASS_API slots final
    {
        using key_t = sight::core::com::slots::key_t;

        inline static const key_t REMOVE_ALL              = "remove_all";
        inline static const key_t REMOVE_GROUP            = "remove_group";
        inline static const key_t MODIFY_GROUP            = "modifyGroup";
        inline static const key_t RENAME_GROUP            = "rename_group";
        inline static const key_t MODIFY_POINT            = "modifyPoint";
        inline static const key_t ADD_POINT               = "add_point";
        inline static const key_t REMOVE_POINT            = "removePoint";
        inline static const key_t INSERT_POINT            = "insertPoint";
        inline static const key_t SELECT_POINT            = "selectPoint";
        inline static const key_t DESELECT_POINT          = "deselectPoint";
        inline static const key_t INITIALIZE_IMAGE        = "initializeImage";
        inline static const key_t SLICE_TYPE              = "sliceType";
        inline static const key_t SLICE_INDEX             = "sliceIndex";
        inline static const key_t TOGGLE_ADD_LANDMARKS    = "toggle_add_landmarks";
        inline static const key_t TOGGLE_REMOVE_LANDMARKS = "toggleRemoveLandmarks";
        inline static const key_t REMOVE_LANDMARKS        = "remove_landmarks";
        inline static const key_t CREATE_LANDMARK         = "create_landmark";
        inline static const key_t CONFIGURE_LANDMARKS     = "configure_landmarks";
    };

    /// SLOT: removes all groups.
    MODULE_VIZ_SCENE3D_API void remove_all();

    /// SLOT: removes an entire group.
    /// @param _group_name name of the group to remove.
    MODULE_VIZ_SCENE3D_API void remove_group(std::string _group_name);

    /// SLOT: removes an entire group and re-create it.
    /// @param _group_name name of the group to update.
    MODULE_VIZ_SCENE3D_API void modify_group(std::string _group_name);

    /// SLOT: replaces an entire group and re-create it.
    /// @param _old_group_name old group name to update.
    /// @param _new_group_name new group name to replace the old one.
    MODULE_VIZ_SCENE3D_API void rename_group(std::string _old_group_name, std::string _new_group_name);

    /// SLOT: removes a point group and update it.
    /// @param _group_name name of the group to update.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_API void modify_point(std::string _group_name, std::size_t _index);

    /// SLOT: adds the last point of a landmarks group.
    /// @param _group_name  group name of the point to add.
    MODULE_VIZ_SCENE3D_API void add_point(std::string _group_name);

    /// SLOT: removes a point.
    /// @param _group_name group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_API void remove_point(std::string _group_name, std::size_t _index);

    /// SLOT: inserts a point.
    /// @param _group_name group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_API void insert_point(std::string _group_name, std::size_t _index);

    /// SLOT: hightlights the selected landmark.
    /// @param _group_name group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_API void select_point(std::string _group_name, std::size_t _index);

    /// SLOT: resets the hightlights the selected landmark.
    /// @param _group_name group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_API void deselect_point(std::string _group_name, std::size_t _index);

    /// SLOT: initializes image slices index if there is one.
    MODULE_VIZ_SCENE3D_API void initialize_image();

    /// SLOT: updates the image slice type.
    /// @param _from origin of the orientation.
    /// @param _to destination of the orientation.
    MODULE_VIZ_SCENE3D_API void change_slice_type(int _from, int _to);

    /// SLOT: updates the image slice index to show or hide landmarks.
    /// @param _axial_index new axial slice index.
    /// @param _frontal_index new frontal slice index.
    /// @param _sagittal_index new sagittal slice index.
    MODULE_VIZ_SCENE3D_API void change_slice_index(int _axial_index, int _frontal_index, int _sagittal_index);

    /// SLOT: Toggle landmarks addition
    /// @param _toggle set or unset landmarks addition mode.
    MODULE_VIZ_SCENE3D_API void toggle_add_landmarks(bool _toggle);

    /// SLOT: Toggle landmarks removal
    /// @param _toggle set or unset landmarks removal mode.
    MODULE_VIZ_SCENE3D_API void toggle_remove_landmarks(bool _toggle);

    /// SLOT: Remove all visible landmarks
    MODULE_VIZ_SCENE3D_API void remove_landmarks();

    /// SLOT: Create a point and insert it in the Landmarks data.
    MODULE_VIZ_SCENE3D_API void create_landmark(sight::data::landmarks::point_t _point);

    /// SLOT: Configure the new landmarks size, shape and color used when adding landmarks ind "ADD" mode.
    /// Parameter with `std::nullopt`, means "no change".
    /// @param _group the group name of the landmarks to configure.
    /// @param _color the color of the landmarks.
    /// @param _size the size of the landmarks.
    /// @param _shape the shape of the landmarks.
    /// @param _group_max the maximum number of landmark in the group. Value < 0 means "no limit".
    /// @param _visible_max the maximum number of visible landmark. Value < 0 means "no limit".
    /// @param _total_max the maximum number of total landmark. Value < 0 means "no limit".
    MODULE_VIZ_SCENE3D_API void configure_landmarks(
        std::optional<std::string> _group,
        std::optional<sight::data::landmarks::color_t> _color,
        std::optional<sight::data::landmarks::size_t> _size,
        std::optional<sight::data::landmarks::shape> _shape,
        std::optional<int> _group_max,
        std::optional<int> _visible_max,
        std::optional<int> _total_max
    );

    struct MODULE_VIZ_SCENE3D_CLASS_API signals final
    {
        using key_t = sight::core::com::signals::key_t;

        /// Signal send when double clicked on a landmark, send its world coordinates;
        inline static const key_t SEND_WORLD_COORD = "send_world_coord";

        using world_coordinates_signal_t = core::com::signal<void (double, double, double)>;
    };

protected:

    /// Configure the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() final;

    /// Creates the material adaptor end create existing landmarls.
    MODULE_VIZ_SCENE3D_API void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::landmarks::MODIFIED_SIG of s_LANDMARKS_INOUT to UPDATE_SLOT_SLOT
     * Connect data::landmarks::GROUP_REMOVED_SIG of s_LANDMARKS_INOUT to REMOVE_GROUP_SLOT
     * Connect data::landmarks::GROUP_MODIFIED_SIG of s_LANDMARKS_INOUT to MODIFY_GROUP_SLOT
     * Connect data::landmarks::POINT_ADDED_SIG of s_LANDMARKS_INOUT to ADD_POINT_SLOT
     * Connect data::landmarks::POINT_REMOVED_SIG of s_LANDMARKS_INOUT to REMOVE_GROUP_SLOT
     * Connect data::landmarks::POINT_INSERTED_SIG of s_LANDMARKS_INOUT to INSERT_POINT_SLOT
     * Connect data::landmarks::POINT_SELECTED_SIG of s_LANDMARKS_INOUT to SELECT_POINT_SLOT
     * Connect data::landmarks::POINT_DESELECTED_SIG of s_LANDMARKS_INOUT to DESELECT_POINT_SLOT
     * Connect data::image::SLICE_TYPE_MODIFIED_SIG of s_IMAGE_INPUT to SLICE_TYPE_SLOT
     * Connect data::image::SLICE_INDEX_MODIFIED_SIG of s_IMAGE_INPUT to SLICE_INDEX_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::connections_t auto_connections() const final;

    /// Deletes landmarks and re-create them.
    MODULE_VIZ_SCENE3D_API void updating() final;

    /// Destroys Ogre's resources.
    MODULE_VIZ_SCENE3D_API void stopping() final;

    /**
     * @brief Sets the landmarks visibility.
     * @param _visible the visibility status of the landmarks.
     */
    MODULE_VIZ_SCENE3D_API void set_visible(bool _visible) final;

    /**
     * @brief Retrieves the picked landmark and stores the result in m_pickedData.
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    MODULE_VIZ_SCENE3D_API void button_press_event(mouse_button _button, modifier _mod, int _x, int _y) final;

    /**
     * @brief Moves a landmark stored in m_pickedData.
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @param _dx width displacement of the mouse since the last event.
     * @param _dx height displacement of the mouse since the last event.
     */
    MODULE_VIZ_SCENE3D_API void mouse_move_event(
        mouse_button _button,
        modifier _mod,
        int _x,
        int _y,
        int _dx,
        int _dy
    ) final;

    /**
     * @brief Resets m_pickedData.
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    MODULE_VIZ_SCENE3D_API void button_release_event(mouse_button _button, modifier _mod, int _x, int _y) final;

    /**
     * @brief Listens to mouse buttons being double pressed.
     * @param _button pressed mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    MODULE_VIZ_SCENE3D_API void button_double_press_event(mouse_button _button, modifier _mods, int _x, int _y) final;

private:

    /// Stores data used to display one landmark.
    struct landmark final
    {
        landmark(
            Ogre::SceneNode* _node,
            Ogre::ManualObject* _object,
            std::string _group_name,
            std::size_t _index,
            sight::viz::scene3d::text::sptr _label
        ) :
            m_node(_node),
            m_object(_object),
            m_group_name(_group_name),
            m_index(_index),
            m_label(_label)
        {
        }

        Ogre::SceneNode* m_node {nullptr};       /*!< Contains the node of the landmark */
        Ogre::ManualObject* m_object {nullptr};  /*!< Contains the manual object that represent the landmark */
        std::string m_group_name;                /*!< Defines the group name of the landmark */
        std::size_t m_index {0};                 /*!< Defines the index of the landmark */
        sight::viz::scene3d::text::sptr m_label; /*!< Defines the text label of the landmark (can be nullptr) */
    };

    /// Stores data used to hightlight the selected landmark.
    struct selected_landmark final
    {
        selected_landmark(core::thread::timer::sptr _timer, std::shared_ptr<landmark> _landmark) :
            m_timer(_timer),
            m_landmark(_landmark)
        {
        }

        core::thread::timer::sptr m_timer;
        std::shared_ptr<landmark> m_landmark;
        bool m_show {false};
    };

    using orientation_mode = data::helper::medical_image::orientation_t;

    /// Show the landmark at the given index.
    enum class view_distance : std::uint8_t
    {
        slices_in_range = 0,
        current_slice,
        all_slices
    };

    /**
     * @brief Gets the normalized camera direction vector.
     * @param _cam camera from which to extract the direction vector.
     * @return A vector representing the camera direction
     */
    static Ogre::Vector3 get_cam_direction(const Ogre::Camera* _cam);

    /// Signal send when double clicked on a landmark, send its world coordinates;
    const signals::world_coordinates_signal_t::sptr m_send_world_coord {
        new_signal<signals::world_coordinates_signal_t>(signals::SEND_WORLD_COORD)
    };

    /**
     * @brief inserts a point.
     * @param _group_name group name of the landmark.
     * @param _index index of the point relative to the group.
     * @param _landmarks landmarks data in which the point will be inserted.
     */
    std::shared_ptr<landmark> insert_my_point(
        std::string _group_name,
        std::size_t _index,
        const data::landmarks::csptr& _landmarks
    );

    /**
     * @brief Manages the highting of the landmarks at the given index (must be run in a thread).
     * @param _selected_landmark which landmarks to manage.
     */
    void hightlight(std::shared_ptr<selected_landmark> _selected_landmark);

    /// Used to create a landmark and pick it. Called by the mouse event (pick) and the slot (no pick).
    void create_and_pick_landmark(const sight::data::landmarks::point_t& _point, bool _pick = true);

    /// Check if the maximum number of landmarks is reached.
    bool is_max_landmarks_reached();

    /**
     * @brief Hides landmarks that are not on the current image slice index (if one is given).
     * @see hideLandmark().
     */
    void hide_landmarks();

    /**
     * @brief Gets the nearest picked position if there is one.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @return The picked world coordinates.
     */
    std::optional<Ogre::Vector3> get_nearest_picked_position(int _x, int _y);

    /**
     * @brief Hides the landmark if it's not on the current image slice index (if one is given).
     * @param _landmark the landmark to hide.
     */
    void hide_landmark(std::shared_ptr<landmark> _landmark);

    /**
     * @brief Hides the landmark if it's not on the current image slice index (if one is given).
     * @param _landmark the landmark to hide.
     * @param _landmarks landmarks data in which the landmarks should be hidden.
     */
    void hide_my_landmark(landmark& _landmark, const data::landmarks& _landmarks);

    bool is_landmark_visible(const data::landmarks::point_t& _point, data::landmarks::size_t _group_size) const;

    /// Contains the root scene node.
    Ogre::SceneNode* m_trans_node {nullptr};

    /// Contains the material data.
    data::material::sptr m_material {nullptr};

    /// Contains the Ogre material adaptor.
    module::viz::scene3d::adaptor::material::sptr m_material_adaptor {nullptr};

    /// Stores each landmarks points.
    std::vector<std::shared_ptr<landmark> > m_manual_objects;

    /// Enables labels.
    bool m_enable_labels {true};

    /// Defines the label font size in points.
    std::size_t m_font_size {12};

    /// Stores informations about the selected landmark.
    std::list<std::shared_ptr<selected_landmark> > m_selected_landmarks;

    /// Define a mutex to synchronized methods working with selected landmark.
    std::mutex m_selected_mutex;

    /// Defines the image orientation.
    orientation_mode m_orientation {orientation_mode::z_axis};

    /// Defines the view distance of the landmarks.
    view_distance m_view_distance {view_distance::slices_in_range};

    /// Stores the current position index for each axis.
    std::array<float, 3> m_current_slice_pos {0.F, 0.F, 0.F};

    /// Defines whether or not interactions are enabled with distances.
    bool m_interactive {true};

    /// Defines the priority of the interactor.
    int m_priority {2};

    /// Defines the current picked data, reset by buttonReleaseEvent(MouseButton, int, int).
    std::shared_ptr<landmark> m_picked_data {nullptr};

    /// Defines the mask used to filter out entities when the distance is auto snapped.
    std::uint32_t m_query_mask {0xFFFFFFFF};

    /// Defines the mask used to filter landmarks, it optimizes the ray launched to retrieve the picked distance.
    std::uint32_t m_landmarks_query_flag {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Landmark addition / removal toggle
    enum class landmarks_mode : uint8_t
    {
        none = 0,
        add,
        remove
    } m_landmarks_mode {landmarks_mode::none};

    /// Initial group configuration
    /// @{
    std::string m_current_group {"Landmarks"};
    sight::data::landmarks::color_t m_current_color {1.0F, 1.0F, 0.0F, 1.0F};
    sight::data::landmarks::size_t m_current_size {32.0F};
    sight::data::landmarks::shape m_current_shape {sight::data::landmarks::shape::sphere};
    /// @}

    /// Allow to limit the number of landmarks
    /// @{
    std::map<std::string, std::size_t> m_group_max;
    std::optional<size_t> m_visible_max {std::nullopt};
    std::optional<size_t> m_total_max {std::nullopt};
    /// @}

    static constexpr std::string_view LANDMARKS_INOUT = "landmarks";
    static constexpr std::string_view IMAGE_INPUT     = "image";

    sight::data::ptr<sight::data::landmarks, sight::data::access::inout> m_landmarks {this, LANDMARKS_INOUT, true};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_image {this, IMAGE_INPUT, true, true};
};

} // namespace sight::module::viz::scene3d::adaptor.
