/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "modules/viz/scene3d_qt/config.hpp"

#include "viz/scene3d/material_adaptor.hpp"

#include <core/macros.hpp>
#include <core/thread/timer.hpp>

#include <data/fiducials_series.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>
#include <data/landmarks.hpp>
#include <data/material.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/landmarks_configuration.hpp>
#include <viz/scene3d/text.hpp>
#include <viz/scene3d/transformable.hpp>

#include <QPushButton>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

struct landmarks_or_image_series_const_ptr
{
    data::landmarks::csptr landmarks;
    data::image_series::csptr image_series;
};

struct landmarks_or_image_series_const_lock
{
    data::mt::locked_ptr<const data::landmarks> landmarks;
    data::mt::locked_ptr<const data::image_series> image_series;

    operator landmarks_or_image_series_const_ptr() const {
        return {.landmarks = landmarks.get_shared(), .image_series = image_series.get_shared()};
    }
};

struct landmarks_or_image_series_ptr
{
    data::landmarks::sptr landmarks;
    data::image_series::sptr image_series;

    operator landmarks_or_image_series_const_ptr() const {
        return {.landmarks = landmarks, .image_series = image_series};
    }
};

struct landmarks_or_image_series_lock
{
    data::mt::locked_ptr<data::landmarks> landmarks;
    data::mt::locked_ptr<data::image_series> image_series;

    operator landmarks_or_image_series_ptr() const {
        return {.landmarks = landmarks.get_shared(), .image_series = image_series.get_shared()};
    }

    operator landmarks_or_image_series_const_ptr() const {
        return {.landmarks = landmarks.get_shared(), .image_series = image_series.get_shared()};
    }
};

struct image_or_image_series_const_ptr
{
    data::image::csptr image;
    data::image_series::csptr image_series;
};

struct image_or_image_series_const_lock
{
    data::mt::locked_ptr<const data::image> image;
    data::mt::locked_ptr<const data::image_series> image_series;

    operator image_or_image_series_const_ptr() const {
        return {.image = image.get_shared(), .image_series = image_series.get_shared()};
    }
};

struct image_or_image_series_ptr
{
    data::image::csptr image;
    data::image_series::sptr image_series;

    operator image_or_image_series_const_ptr() const {
        return {.image = image, .image_series = image_series};
    }
};

struct image_or_image_series_lock
{
    data::mt::locked_ptr<const data::image> image;
    data::mt::locked_ptr<data::image_series> image_series;

    operator image_or_image_series_ptr() const {
        return {.image = image.get_shared(), .image_series = image_series.get_shared()};
    }

    operator image_or_image_series_const_ptr() const {
        return {.image = image.get_shared(), .image_series = image_series.get_shared()};
    }
};

/**
 * @brief This adaptor displays landmarks.
 *
 * @section Slots Slots
 * - \b remove_all(): removes all groups.
 * - \b remove_group(std::string): removes an entire group.
 * - \b modifyGroup(std::string): removes an entire group and re-create it.
 * - \b rename_group(std::string, std::string): replaces and old group's name by a new one.
 * - \b set_current_group(std::string): set the current group
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
 * - \b remove_landmarks(): Remove all visible landmarks.
 *                         Which landmarks are removed depends of the "viewDistance" parameters.
 * - \b configure_landmarks(): Configure the new landmarks size, shape and color used when adding landmarks.
 * - \b enableEditMode(): enable edit mode
 * - \b disableEditMode(): disable edit mode
 * - \b toggleEditMode(): enable edit mode if it is enabled, else disable it
 * - \b change_edit_mode(bool): enable edit mode if true, else disable it
 *
 * @section Signals Signals
 * - \b send_world_coord(double, double, double): sends world coordinates of current selected landmarks (by double
 * click).
 * - \b edit_mode_changed(bool): sent if the edit mode changed (typically via Escape key press or right mouse click).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d_qt::adaptor::fiducials::point">
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
 * - \b allowRenaming (optional, bool, default=true): true if the user can rename landmarks
 * - \b modify (optional, all/group, default="all"): if "all", all the landmarks can be modified, ignoring the current
        group; if "group", only the landmarks belonging to the current group can be modified
 */
class MODULE_VIZ_SCENE3D_QT_CLASS_API point final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable,
    public sight::viz::scene3d::interactor::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(point, sight::viz::scene3d::adaptor);

    /// Creates the adaptor.
    MODULE_VIZ_SCENE3D_QT_API point() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_QT_API ~point() noexcept final = default;

    struct MODULE_VIZ_SCENE3D_QT_CLASS_API slots final
    {
        using key_t = sight::core::com::slots::key_t;

        inline static const key_t REMOVE_ALL        = "remove_all";
        inline static const key_t REMOVE_GROUP      = "remove_group";
        inline static const key_t MODIFY_GROUP      = "modifyGroup";
        inline static const key_t RENAME_GROUP      = "rename_group";
        inline static const key_t SET_CURRENT_GROUP = "set_current_group";
        inline static const key_t MODIFY_POINT      = "modifyPoint";
        inline static const key_t ADD_POINT         = "add_point";
        inline static const key_t REMOVE_POINT      = "removePoint";
        inline static const key_t INSERT_POINT      = "insertPoint";
        inline static const key_t SELECT_POINT      = "selectPoint";
        inline static const key_t DESELECT_POINT    = "deselectPoint";
        inline static const key_t INITIALIZE_IMAGE  = "initializeImage";
        inline static const key_t SLICE_TYPE        = "sliceType";
        inline static const key_t SLICE_INDEX       = "sliceIndex";
        // Not implemented
        // inline static const key_t TOGGLE_ADD_LANDMARKS    = "toggle_add_landmarks";
        // inline static const key_t TOGGLE_REMOVE_LANDMARKS = "toggleRemoveLandmarks";
        inline static const key_t REMOVE_LANDMARKS    = "remove_landmarks";
        inline static const key_t CREATE_LANDMARK     = "create_landmark";
        inline static const key_t CONFIGURE_LANDMARKS = "configure_landmarks";
        inline static const key_t ENABLE_EDIT_MODE    = "enableEditMode";
        inline static const key_t DISABLE_EDIT_MODE   = "disableEditMode";
        inline static const key_t TOGGLE_EDIT_MODE    = "toggleEditMode";
        inline static const key_t CHANGE_EDIT_MODE    = "change_edit_mode";
        inline static const key_t ENABLE_MOVE_MODE    = "enableMoveMode";
        inline static const key_t DISABLE_MOVE_MODE   = "disableMoveMode";
        inline static const key_t TOGGLE_MOVE_MODE    = "toggleMoveMode";
        inline static const key_t CHANGE_MOVE_MODE    = "change_move_mode";
    };

    /// Remove all manual objects group
    MODULE_VIZ_SCENE3D_QT_API void remove_all_manual_objects();

    /// SLOT: remove all groups
    MODULE_VIZ_SCENE3D_QT_API void remove_all();

    /// SLOT: removes an entire group.
    /// @param _group_name name of the group to remove.
    MODULE_VIZ_SCENE3D_QT_API void remove_group(std::string _group_name);

    /// SLOT: removes an entire group and re-create it.
    /// @param _group_name name of the group to update.
    MODULE_VIZ_SCENE3D_QT_API void modify_group(std::string _group_name);

    /// SLOT: replaces an entire group and re-create it.
    /// @param _old_group_name old group name to update.
    /// @param _new_group_name new group name to replace the old one.
    MODULE_VIZ_SCENE3D_QT_API void rename_group(std::string _old_group_name, std::string _new_group_name);

    /// SLOT: set the current group name
    /// @param _new_current_group_name the new current group name.
    MODULE_VIZ_SCENE3D_QT_API void set_current_group(std::string _new_current_group_name);

    /// SLOT: removes a point group and update it.
    /// @param _group_name name of the group to update.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_QT_API void modify_point(std::string _group_name, std::size_t _index);

    /// SLOT: adds the last point of a landmarks group.
    /// _groupName group name of the point to add.
    MODULE_VIZ_SCENE3D_QT_API void add_point(std::string _group_name);

    /// SLOT: removes a point.
    /// @param _group_name group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_QT_API void remove_point(std::string _group_name, std::size_t _index);

    /// SLOT: inserts a point.
    /// @param _group_name group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_QT_API void insert_point(std::string _group_name, std::size_t _index);

    /// SLOT: highlights the selected landmark.
    /// @param _group_name group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_QT_API void select_point(std::string _group_name, std::size_t _index);

    /// SLOT: resets the highlights the selected landmark.
    /// @param _group_name group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_QT_API void deselect_point(std::string _group_name, std::size_t _index);

    /// SLOT: initializes image slices index if there is one.
    // MODULE_VIZ_SCENE3D_QT_API void initializeImage();

    /// SLOT: updates the image slice type.
    /// @param _from origin of the orientation.
    /// @param _to destination of the orientation.
    MODULE_VIZ_SCENE3D_QT_API void change_slice_type(int _from, int _to);

    /// SLOT: updates the image slice index to show or hide landmarks.
    /// @param _axial_index new axial slice index.
    /// @param _frontal_index new frontal slice index.
    /// @param _sagittal_index new sagittal slice index.
    MODULE_VIZ_SCENE3D_QT_API void change_slice_index(int _axial_index, int _frontal_index, int _sagittal_index);

    /// SLOT: Toggle landmarks addition
    /// @param _toggle set or unset landmarks addition mode.
    MODULE_VIZ_SCENE3D_QT_API void toggle_add_landmarks(bool _toggle);

    /// SLOT: Toggle landmarks removal
    /// @param _toggle set or unset landmarks removal mode.
    MODULE_VIZ_SCENE3D_QT_API void toggle_remove_landmarks(bool _toggle);

    /// SLOT: Remove all visible landmarks
    MODULE_VIZ_SCENE3D_QT_API void remove_landmarks();

    /// SLOT: Create a point and insert it in the Landmarks data.
    MODULE_VIZ_SCENE3D_QT_API void create_landmark(sight::data::landmarks::point_t _point);

    /// SLOT: Configure the new landmarks size, shape and color used when adding landmarks ind "ADD" mode.

    MODULE_VIZ_SCENE3D_QT_API void configure_landmarks(sight::viz::scene3d::landmarks_configuration _configuration);

    /// SLOT: Enable edit mode.
    void enable_edit_mode();

    /// SLOT: Disable edit mode.
    void disable_edit_mode();

    /// SLOT: Enable move mode.
    void enable_move_mode();

    /// SLOT: Disable move mode.
    void disable_move_mode();

    struct MODULE_VIZ_SCENE3D_QT_CLASS_API signals final
    {
        using key_t = sight::core::com::signals::key_t;

        /// Signal send when double clicked on a landmark, send its world coordinates;
        inline static const key_t SEND_WORLD_COORD = "send_world_coord";
        using world_coordinates_signal_t = core::com::signal<void (double, double, double)>;

        inline static const key_t EDIT_MODE_CHANGED = "edit_mode_changed";
        using edit_mode_changed = core::com::signal<void (bool)>;
    };

protected:

    /// Configure the adaptor.
    MODULE_VIZ_SCENE3D_QT_API void configuring() final;

    /// Creates the material adaptor end create existing landmarls.
    MODULE_VIZ_SCENE3D_QT_API void starting() final;

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
    MODULE_VIZ_SCENE3D_QT_API service::connections_t auto_connections() const final;

    /// Deletes landmarks and re-create them.
    MODULE_VIZ_SCENE3D_QT_API void updating() final;

    /// Destroys Ogre's resources.
    MODULE_VIZ_SCENE3D_QT_API void stopping() final;

    /**
     * @brief Sets the landmarks visibility.
     * @param _visible the visibility status of the landmarks.
     */
    MODULE_VIZ_SCENE3D_QT_API void set_visible(bool _visible) final;

    /**
     * @brief Retrieves the picked landmark and stores the result in m_pickedData.
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    MODULE_VIZ_SCENE3D_QT_API void button_press_event(mouse_button _button, modifier _mod, int _x, int _y) final;

    /**
     * @brief Moves a landmark stored in m_pickedData.
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @param _dx width displacement of the mouse since the last event.
     * @param _dx height displacement of the mouse since the last event.
     */
    MODULE_VIZ_SCENE3D_QT_API void mouse_move_event(
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
    MODULE_VIZ_SCENE3D_QT_API void button_release_event(mouse_button _button, modifier _mod, int _x, int _y) final;

    /**
     * @brief Listens to mouse buttons being double pressed.
     * @param _button pressed mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    MODULE_VIZ_SCENE3D_QT_API void button_double_press_event(
        mouse_button _button,
        modifier _mods,
        int _x,
        int _y
    ) final;

    /// Hides the contextual menu when the mouse wheel is used.
    MODULE_VIZ_SCENE3D_QT_API void wheel_event(modifier _mods, double _angle_delta, int _x, int _y) final;

    /// Exit edit mode if the Escape key is pressed
    MODULE_VIZ_SCENE3D_QT_API void key_press_event(int _key, modifier _mods, int _mouse_x, int _mouse_y) final;

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

        Ogre::SceneNode* m_node {nullptr};                           /*!< Contains the node of the landmark */
        Ogre::ManualObject* m_object {nullptr};                      /*!< Contains the manual object that represent the
                                                                        landmark */
        std::string m_group_name;                                    /*!< Defines the group name of the landmark */
        std::size_t m_index {0};                                     /*!< Defines the index of the landmark */
        sight::viz::scene3d::text::sptr m_label;                     /*!< Defines the text label of the landmark (can be
                                                                         nullptr) */
        std::vector<std::shared_ptr<core::com::slot_base> > m_slots; /*!< Contains the slots related to the landmark */
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

    class delete_contextual_menu_when_focus_out : public QObject
    {
    public:

        explicit delete_contextual_menu_when_focus_out(point* _point);
        bool eventFilter(QObject* _o, QEvent* _e) override;
        point* m_point;
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

    const signals::edit_mode_changed::sptr m_edit_mode_changed = new_signal<signals::edit_mode_changed>(
        signals::EDIT_MODE_CHANGED
    );

    /**
     * Creates a manual object which represents the landmark whose information is in parameter.
     * @param _group_name The group name of the landmark
     * @param _index The index of the landmark in its group
     * @param _group_data Graphical hints for the appearance of the landmark (color, size and shape)
     * @param _point_pos The position of the landmark
     * @returns The created manual object
     */
    std::shared_ptr<landmark> create_manual_object(
        const std::string& _group_name,
        std::size_t _index,
        data::landmarks::landmarks_group _group_data,
        data::landmarks::point_t _point_pos
    );

    /**
     * Creates a manual object which represents the landmark whose information is in parameter.
     * @param _group_name The group name of the landmark
     * @param _index The index of the landmark in its group
     * @param _landmarks Landmarks container which provides information about the landmark (graphical hints and
     * position)
     * @returns The created manual object
     */
    std::shared_ptr<landmark> create_manual_object(
        const std::string& _group_name,
        std::size_t _index,
        data::landmarks::csptr _landmarks
    );

    /**
     * Creates a manual object which represents the landmark whose information is in parameter.
     * @param _group_name The group name of the landmark
     * @param _index The index of the landmark in its group
     * @param _lf Landmarks container which provides information about the landmark (graphical hints and position)
     * @returns The created manual object
     */
    std::shared_ptr<landmark> create_manual_object(
        const std::string& _group_name,
        std::size_t _index,
        landmarks_or_image_series_const_ptr _lf
    );

    /**
     * @brief Manages the highlighting of the landmarks at the given index (must be run in a thread).
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
    void update_landmarks_visibility();

    /**
     * @brief Gets the nearest picked position if there is one.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @return The picked world coordinates.
     */
    std::optional<Ogre::Vector3> get_nearest_picked_position(int _x, int _y);

    /**
     * @brief Hides the landmark if it's not on the current image slice index (if one is given).
     * @param _landmark the landmark whose visibility must be updated.
     */
    void update_landmark_visibility(std::shared_ptr<landmark> _landmark);

    /**
     * Hides the landmark if it's not on the current image slice index (if one is given).
     * @param _landmark The landmark to hide
     * @param _group The group information of the landmark to hide
     */
    void update_landmark_visibility(landmark& _landmark, std::optional<data::landmarks::landmarks_group> _group);

    /**
     * @brief Hides the landmark if it's not on the current image slice index (if one is given).
     * @param _landmark the landmark whose visibility must be updated.
     * @param _lock landmarks data in which the landmarks should be updated.
     */
    void update_landmark_visibility(landmark& _landmark, const landmarks_or_image_series_const_lock& _lock);

    bool is_landmark_visible(const data::landmarks::point_t& _point, data::landmarks::size_t _group_size) const;

    std::shared_ptr<landmark> try_pick(int _x, int _y, bool _for_modification = true) const;

    void set_cursor(QCursor _cursor);

    [[nodiscard]] landmarks_or_image_series_lock lock_landmarks();

    [[nodiscard]] landmarks_or_image_series_const_lock const_lock_landmarks() const;

    [[nodiscard]] image_or_image_series_lock lock_image();

    [[nodiscard]] image_or_image_series_const_lock const_lock_image() const;

    /// Contains the root scene node.
    Ogre::SceneNode* m_trans_node {nullptr};

    /// Contains the material data.
    data::material::sptr m_material {nullptr};

    /// Contains the Ogre material adaptor.
    sight::viz::scene3d::material_adaptor::sptr m_material_adaptor {nullptr};

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

    std::set<std::string> m_movable_groups;

    /// Adaptor edit "mode":
    /// - DISPLAY is read-only,
    /// - MOVE allows to modify existing landmarks position
    /// - EDIT allows to add/remove and move landmarks
    enum edit_mode : std::uint8_t
    {
        display = 1 << 0,
        move    = 1 << 1,
        edit    = 1 << 2
    };

    std::uint8_t m_edit_mode {std::uint8_t(edit_mode::display)};

    /// Whether all landmarks can be modified or only landmarks belonging to the current group
    bool m_can_only_modify_current = false;

    /// True if the landmarks can be renamed by the user
    bool m_renaming_allowed = true;

    /// True if we must show the contextual menu. It must be shown if the landmark already existed and the landmark
    /// wasn't moved.
    bool m_must_show_contextual_menu = false;

    QWidget* m_contextual_menu = nullptr;

    /// Auto-delete the event filter in the end
    std::unique_ptr<delete_contextual_menu_when_focus_out> m_event_filter = nullptr;

    static constexpr std::string_view LANDMARKS_INOUT    = "landmarks";
    static constexpr std::string_view IMAGE_SERIES_INOUT = "imageSeries";
    static constexpr std::string_view IMAGE_INPUT        = "image";

    sight::data::ptr<sight::data::landmarks, sight::data::access::inout> m_landmarks {this, LANDMARKS_INOUT, true};
    sight::data::ptr<sight::data::image_series, sight::data::access::inout> m_image_series
    {this, IMAGE_SERIES_INOUT, true};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_image {this, IMAGE_INPUT, true, true};
};

} // namespace sight::module::viz::scene3d_qt::adaptor.
