/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

namespace sight::module::viz::scene3d_qt::adaptor
{

struct LandmarksOrImageSeriesConstPtr
{
    data::landmarks::csptr landmarks;
    data::image_series::csptr imageSeries;
};

struct LandmarksOrImageSeriesConstLock
{
    data::mt::locked_ptr<const data::landmarks> landmarks;
    data::mt::locked_ptr<const data::image_series> imageSeries;

    operator LandmarksOrImageSeriesConstPtr() const {
        return {.landmarks = landmarks.get_shared(), .imageSeries = imageSeries.get_shared()};
    }
};

struct LandmarksOrImageSeriesPtr
{
    data::landmarks::sptr landmarks;
    data::image_series::sptr imageSeries;

    operator LandmarksOrImageSeriesConstPtr() const {
        return {.landmarks = landmarks, .imageSeries = imageSeries};
    }
};

struct LandmarksOrImageSeriesLock
{
    data::mt::locked_ptr<data::landmarks> landmarks;
    data::mt::locked_ptr<data::image_series> imageSeries;

    operator LandmarksOrImageSeriesPtr() const {
        return {.landmarks = landmarks.get_shared(), .imageSeries = imageSeries.get_shared()};
    }

    operator LandmarksOrImageSeriesConstPtr() const {
        return {.landmarks = landmarks.get_shared(), .imageSeries = imageSeries.get_shared()};
    }
};

struct ImageOrImageSeriesConstPtr
{
    data::image::csptr image;
    data::image_series::csptr imageSeries;
};

struct ImageOrImageSeriesConstLock
{
    data::mt::locked_ptr<const data::image> image;
    data::mt::locked_ptr<const data::image_series> imageSeries;

    operator ImageOrImageSeriesConstPtr() const {
        return {.image = image.get_shared(), .imageSeries = imageSeries.get_shared()};
    }
};

struct ImageOrImageSeriesPtr
{
    data::image::csptr image;
    data::image_series::sptr imageSeries;

    operator ImageOrImageSeriesConstPtr() const {
        return {.image = image, .imageSeries = imageSeries};
    }
};

struct ImageOrImageSeriesLock
{
    data::mt::locked_ptr<const data::image> image;
    data::mt::locked_ptr<data::image_series> imageSeries;

    operator ImageOrImageSeriesPtr() const {
        return {.image = image.get_shared(), .imageSeries = imageSeries.get_shared()};
    }

    operator ImageOrImageSeriesConstPtr() const {
        return {.image = image.get_shared(), .imageSeries = imageSeries.get_shared()};
    }
};

/**
 * @brief This adaptor displays landmarks.
 *
 * @section Slots Slots
 * - \b removeAll(): removes all groups.
 * - \b removeGroup(std::string): removes an entire group.
 * - \b modifyGroup(std::string): removes an entire group and re-create it.
 * - \b renameGroup(std::string, std::string): replaces and old group's name by a new one.
 * - \b setCurrentGroup(std::string): set the current group
 * - \b addPoint(std::string): adds the last point of a landmarks group.
 * - \b removePoint(std::string, std::size_t): removes a point.
 * - \b insertPoint(std::string, std::size_t): inserts a point.
 * - \b selectPoint(std::string, std::size_t) hightlights the selected landmark.
 * - \b deselectPoint(std::string, std::size_t): resets the hightlighting of the selected landmark.
 * - \b initializeImage(): initializes image slices index if there is one.
 * - \b changeSliceType(int, int): updates the image slice type.
 * - \b changeSliceIndex(int, int, int): updates the image slice index to show or hide landmarks.
 * - \b updateVisibility(bool): shows or hides the landmarks.
 * - \b toggleVisibility(): toggles whether the landmarks are shown or not.
 * - \b show(): shows the landmarks.
 * - \b hide(): hides the landmarks.
 * - \b removeLandmarks(): Remove all visible landmarks.
 *                         Which landmarks are removed depends of the "viewDistance" parameters.
 * - \b configureLandmarks(): Configure the new landmarks size, shape and color used when adding landmarks.
 * - \b enableEditMode(): enable edit mode
 * - \b disableEditMode(): disable edit mode
 * - \b toggleEditMode(): enable edit mode if it is enabled, else disable it
 * - \b changeEditMode(bool): enable edit mode if true, else disable it
 *
 * @section Signals Signals
 * - \b sendWorldCoord(double, double, double): sends world coordinates of current selected landmarks (by double click).
 * - \b editModeChanged(bool): sent if the edit mode changed (typically via Escape key press or right mouse click).
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
 * - \b allowRenaming (optional, bool, default=true): true if the user can rename landmarks
 * - \b modify (optional, all/group, default="all"): if "all", all the landmarks can be modified, ignoring the current
        group; if "group", only the landmarks belonging to the current group can be modified
 */
class MODULE_VIZ_SCENE3D_QT_CLASS_API landmarks final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable,
    public sight::viz::scene3d::interactor::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(landmarks, sight::viz::scene3d::adaptor);

    /// Creates the adaptor.
    MODULE_VIZ_SCENE3D_QT_API landmarks() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_QT_API ~landmarks() noexcept final = default;

    struct MODULE_VIZ_SCENE3D_QT_CLASS_API Slots final
    {
        using key_t = sight::core::com::slots::key_t;

        inline static const key_t REMOVE_ALL              = "removeAll";
        inline static const key_t REMOVE_GROUP            = "removeGroup";
        inline static const key_t MODIFY_GROUP            = "modifyGroup";
        inline static const key_t RENAME_GROUP            = "renameGroup";
        inline static const key_t SET_CURRENT_GROUP       = "setCurrentGroup";
        inline static const key_t MODIFY_POINT            = "modifyPoint";
        inline static const key_t ADD_POINT               = "addPoint";
        inline static const key_t REMOVE_POINT            = "removePoint";
        inline static const key_t INSERT_POINT            = "insertPoint";
        inline static const key_t SELECT_POINT            = "selectPoint";
        inline static const key_t DESELECT_POINT          = "deselectPoint";
        inline static const key_t INITIALIZE_IMAGE        = "initializeImage";
        inline static const key_t SLICE_TYPE              = "sliceType";
        inline static const key_t SLICE_INDEX             = "sliceIndex";
        inline static const key_t TOGGLE_ADD_LANDMARKS    = "toggleAddLandmarks";
        inline static const key_t TOGGLE_REMOVE_LANDMARKS = "toggleRemoveLandmarks";
        inline static const key_t REMOVE_LANDMARKS        = "removeLandmarks";
        inline static const key_t CREATE_LANDMARK         = "createLandmark";
        inline static const key_t CONFIGURE_LANDMARKS     = "configureLandmarks";
        inline static const key_t ENABLE_EDIT_MODE        = "enableEditMode";
        inline static const key_t DISABLE_EDIT_MODE       = "disableEditMode";
        inline static const key_t TOGGLE_EDIT_MODE        = "toggleEditMode";
        inline static const key_t CHANGE_EDIT_MODE        = "changeEditMode";
        inline static const key_t ENABLE_MOVE_MODE        = "enableMoveMode";
        inline static const key_t DISABLE_MOVE_MODE       = "disableMoveMode";
        inline static const key_t TOGGLE_MOVE_MODE        = "toggleMoveMode";
        inline static const key_t CHANGE_MOVE_MODE        = "changeMoveMode";
    };

    /// Remove all manual objects group
    MODULE_VIZ_SCENE3D_QT_API void removeAllManualObjects();

    /// SLOT: remove all groups
    MODULE_VIZ_SCENE3D_QT_API void removeAll();

    /// SLOT: removes an entire group.
    /// @param _groupName name of the group to remove.
    MODULE_VIZ_SCENE3D_QT_API void removeGroup(std::string _group_name);

    /// SLOT: removes an entire group and re-create it.
    /// @param _groupName name of the group to update.
    MODULE_VIZ_SCENE3D_QT_API void modifyGroup(std::string _group_name);

    /// SLOT: replaces an entire group and re-create it.
    /// @param _oldGroupName old group name to update.
    /// @param _newGroupName new group name to replace the old one.
    MODULE_VIZ_SCENE3D_QT_API void renameGroup(std::string _old_group_name, std::string _new_group_name);

    /// SLOT: set the current group name
    /// @param _newCurrentGroupName the new current group name.
    MODULE_VIZ_SCENE3D_QT_API void setCurrentGroup(std::string _new_current_group_name);

    /// SLOT: removes a point group and update it.
    /// @param _groupName name of the group to update.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_QT_API void modifyPoint(std::string _group_name, std::size_t _index);

    /// SLOT: adds the last point of a landmarks group.
    /// _groupName group name of the point to add.
    MODULE_VIZ_SCENE3D_QT_API void addPoint(std::string _group_name);

    /// SLOT: removes a point.
    /// @param _groupName group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_QT_API void removePoint(std::string _group_name, std::size_t _index);

    /// SLOT: inserts a point.
    /// @param _groupName group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_QT_API void insertPoint(std::string _group_name, std::size_t _index);

    /// SLOT: hightlights the selected landmark.
    /// @param _groupName group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_QT_API void selectPoint(std::string _group_name, std::size_t _index);

    /// SLOT: resets the hightlights the selected landmark.
    /// @param _groupName group name of the landmark.
    /// @param _index index of the point relative to the group.
    MODULE_VIZ_SCENE3D_QT_API void deselectPoint(std::string _group_name, std::size_t _index);

    /// SLOT: initializes image slices index if there is one.
    // MODULE_VIZ_SCENE3D_QT_API void initializeImage();

    /// SLOT: updates the image slice type.
    /// @param _from origin of the orientation.
    /// @param _to destination of the orientation.
    MODULE_VIZ_SCENE3D_QT_API void changeSliceType(int _from, int _to);

    /// SLOT: updates the image slice index to show or hide landmarks.
    /// @param _axialIndex new axial slice index.
    /// @param _frontalIndex new frontal slice index.
    /// @param _sagittalIndex new sagittal slice index.
    MODULE_VIZ_SCENE3D_QT_API void changeSliceIndex(int _axial_index, int _frontal_index, int _sagittal_index);

    /// SLOT: Toggle landmarks addition
    /// @param toggle set or unset landmarks addition mode.
    MODULE_VIZ_SCENE3D_QT_API void toggleAddLandmarks(bool _toggle);

    /// SLOT: Toggle landmarks removal
    /// @param toggle set or unset landmarks removal mode.
    MODULE_VIZ_SCENE3D_QT_API void toggleRemoveLandmarks(bool _toggle);

    /// SLOT: Remove all visible landmarks
    MODULE_VIZ_SCENE3D_QT_API void removeLandmarks();

    /// SLOT: Create a point and insert it in the Landmarks data.
    MODULE_VIZ_SCENE3D_QT_API void createLandmark(sight::data::landmarks::point_t _point);

    /// SLOT: Configure the new landmarks size, shape and color used when adding landmarks ind "ADD" mode.

    MODULE_VIZ_SCENE3D_QT_API void configureLandmarks(sight::viz::scene3d::landmarks_configuration _configuration);

    /// SLOT: Enable edit mode.
    void enableEditMode();

    /// SLOT: Disable edit mode.
    void disableEditMode();

    /// SLOT: Enable move mode.
    void enableMoveMode();

    /// SLOT: Disable move mode.
    void disableMoveMode();

    struct MODULE_VIZ_SCENE3D_QT_CLASS_API Signals final
    {
        using key_t = sight::core::com::signals::key_t;

        /// Signal send when double clicked on a landmark, send its world coordinates;
        inline static const key_t SEND_WORLD_COORD = "sendWorldCoord";
        using world_coordinates_signal_t = core::com::signal<void (double, double, double)>;

        inline static const key_t EDIT_MODE_CHANGED = "editModeChanged";
        using EditModeChanged = core::com::signal<void (bool)>;
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
    MODULE_VIZ_SCENE3D_QT_API void setVisible(bool _visible) final;

    /**
     * @brief Retrieves the picked landmark and stores the result in m_pickedData.
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    MODULE_VIZ_SCENE3D_QT_API void buttonPressEvent(MouseButton _button, Modifier _mod, int _x, int _y) final;

    /**
     * @brief Moves a landmark stored in m_pickedData.
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @param _dx width displacement of the mouse since the last event.
     * @param _dx height displacement of the mouse since the last event.
     */
    MODULE_VIZ_SCENE3D_QT_API void mouseMoveEvent(
        MouseButton _button,
        Modifier _mod,
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
    MODULE_VIZ_SCENE3D_QT_API void buttonReleaseEvent(MouseButton _button, Modifier _mod, int _x, int _y) final;

    /**
     * @brief Listens to mouse buttons being double pressed.
     * @param _button pressed mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    MODULE_VIZ_SCENE3D_QT_API void buttonDoublePressEvent(MouseButton _button, Modifier _mods, int _x, int _y) final;

    /// Hides the contextual menu when the mouse wheel is used.
    MODULE_VIZ_SCENE3D_QT_API void wheelEvent(Modifier _mods, double _angle_delta, int _x, int _y) final;

    /// Exit edit mode if the Escape key is pressed
    MODULE_VIZ_SCENE3D_QT_API void keyPressEvent(int _key, Modifier _mods, int _mouse_x, int _mouse_y) final;

private:

    /// Stores data used to display one landmark.
    struct Landmark final
    {
        Landmark(
            Ogre::SceneNode* _node,
            Ogre::ManualObject* _object,
            std::string _group_name,
            std::size_t _index,
            sight::viz::scene3d::text::sptr _label
        ) :
            m_node(_node),
            m_object(_object),
            m_groupName(_group_name),
            m_index(_index),
            m_label(_label)
        {
        }

        Ogre::SceneNode* m_node {nullptr};                           /*!< Contains the node of the landmark */
        Ogre::ManualObject* m_object {nullptr};                      /*!< Contains the manual object that represent the
                                                                        landmark */
        std::string m_groupName;                                     /*!< Defines the group name of the landmark */
        std::size_t m_index {0};                                     /*!< Defines the index of the landmark */
        sight::viz::scene3d::text::sptr m_label;                     /*!< Defines the text label of the landmark (can be
                                                                         nullptr) */
        std::vector<std::shared_ptr<core::com::slot_base> > m_slots; /*!< Contains the slots related to the landmark */
    };

    /// Stores data used to hightlight the selected landmark.
    struct SelectedLandmark final
    {
        SelectedLandmark(core::thread::timer::sptr _timer, std::shared_ptr<Landmark> _landmark) :
            m_timer(_timer),
            m_landmark(_landmark)
        {
        }

        core::thread::timer::sptr m_timer;
        std::shared_ptr<Landmark> m_landmark;
        bool m_show {false};
    };

    typedef data::helper::medical_image::orientation_t OrientationMode;

    /// Show the landmark at the given index.
    enum class ViewDistance : std::uint8_t
    {
        SLICES_IN_RANGE = 0,
        CURRENT_SLICE,
        ALL_SLICES
    };

    class DeleteContextualMenuWhenFocusOut : public QObject
    {
    public:

        explicit DeleteContextualMenuWhenFocusOut(landmarks* _s_landmarks);
        bool eventFilter(QObject* _o, QEvent* _e) override;
        landmarks* m_sLandmarks;
    };

    /**
     * @brief Gets the normalized camera direction vector.
     * @param _cam camera from which to extract the direction vector.
     * @return A vector representing the camera direction
     */
    static Ogre::Vector3 getCamDirection(const Ogre::Camera* _cam);

    /// Signal send when double clicked on a landmark, send its world coordinates;
    const Signals::world_coordinates_signal_t::sptr M_SEND_WORLD_COORD {
        new_signal<Signals::world_coordinates_signal_t>(Signals::SEND_WORLD_COORD)
    };

    const Signals::EditModeChanged::sptr M_EDIT_MODE_CHANGED = new_signal<Signals::EditModeChanged>(
        Signals::EDIT_MODE_CHANGED
    );

    /**
     * Creates a manual object which represents the landmark whose information is in parameter.
     * @param groupName The group name of the landmark
     * @param index The index of the landmark in its group
     * @param groupData Graphical hints for the appearance of the landmark (color, size and shape)
     * @param pointPos The position of the landmark
     * @returns The created manual object
     */
    std::shared_ptr<Landmark> createManualObject(
        const std::string& _group_name,
        std::size_t _index,
        data::landmarks::LandmarksGroup _group_data,
        data::landmarks::point_t _point_pos
    );

    /**
     * Creates a manual object which represents the landmark whose information is in parameter.
     * @param groupName The group name of the landmark
     * @param index The index of the landmark in its group
     * @param landmarks Landmarks container which provides information about the landmark (graphical hints and position)
     * @returns The created manual object
     */
    std::shared_ptr<Landmark> createManualObject(
        const std::string& _group_name,
        std::size_t _index,
        data::landmarks::csptr _landmarks
    );

    /**
     * Creates a manual object which represents the landmark whose information is in parameter.
     * @param groupName The group name of the landmark
     * @param index The index of the landmark in its group
     * @param lock Landmarks container which provides information about the landmark (graphical hints and position)
     * @returns The created manual object
     */
    std::shared_ptr<Landmark> createManualObject(
        const std::string& _group_name,
        std::size_t _index,
        LandmarksOrImageSeriesConstPtr _lf
    );

    /**
     * @brief Manages the highting of the landmarks at the given index (must be run in a thread).
     * @param _selectedLandmark which landmarks to manage.
     */
    void hightlight(std::shared_ptr<SelectedLandmark> _selected_landmark);

    /// Used to create a landmark and pick it. Called by the mouse event (pick) and the slot (no pick).
    void createAndPickLandmark(const sight::data::landmarks::point_t& _point, bool _pick = true);

    /// Check if the maximum number of landmarks is reached.
    bool isMaxLandmarksReached();

    /**
     * @brief Hides landmarks that are not on the current image slice index (if one is given).
     * @see hideLandmark().
     */
    void updateLandmarksVisibility();

    /**
     * @brief Gets the nearest picked position if there is one.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @return The picked world coordinates.
     */
    std::optional<Ogre::Vector3> getNearestPickedPosition(int _x, int _y);

    /**
     * @brief Hides the landmark if it's not on the current image slice index (if one is given).
     * @param _landmark the landmark whose visibility must be updated.
     */
    void updateLandmarkVisibility(std::shared_ptr<Landmark> _landmark);

    /**
     * Hides the landmark if it's not on the current image slice index (if one is given).
     * @param landmark The landmark to hide
     * @param group The group information of the landmark to hide
     */
    void updateLandmarkVisibility(Landmark& _landmark, std::optional<data::landmarks::LandmarksGroup> _group);

    /**
     * @brief Hides the landmark if it's not on the current image slice index (if one is given).
     * @param landmark the landmark whose visibility must be updated.
     * @param lock landmarks data in which the landmarks should be updated.
     */
    void updateLandmarkVisibility(Landmark& _landmark, const LandmarksOrImageSeriesConstLock& _lock);

    bool isLandmarkVisible(const data::landmarks::point_t& _point, data::landmarks::size_t _group_size) const;

    std::shared_ptr<Landmark> tryPick(int _x, int _y, bool _for_modification = true) const;

    void setCursor(QCursor _cursor);

    [[nodiscard]] LandmarksOrImageSeriesLock lockLandmarks();

    [[nodiscard]] LandmarksOrImageSeriesConstLock constLockLandmarks() const;

    [[nodiscard]] ImageOrImageSeriesLock lockImage();

    [[nodiscard]] ImageOrImageSeriesConstLock constLockImage() const;

    /// Contains the root scene node.
    Ogre::SceneNode* m_transNode {nullptr};

    /// Contains the material data.
    data::material::sptr m_material {nullptr};

    /// Contains the Ogre material adaptor.
    sight::viz::scene3d::material_adaptor::sptr m_materialAdaptor {nullptr};

    /// Stores each landmarks points.
    std::vector<std::shared_ptr<Landmark> > m_manualObjects;

    /// Enables labels.
    bool m_enableLabels {true};

    /// Defines the label font size in points.
    std::size_t m_fontSize {12};

    /// Stores informations about the selected landmark.
    std::list<std::shared_ptr<SelectedLandmark> > m_selectedLandmarks;

    /// Define a mutex to synchronized methods working with selected landmark.
    std::mutex m_selectedMutex;

    /// Defines the image orientation.
    OrientationMode m_orientation {OrientationMode::Z_AXIS};

    /// Defines the view distance of the landmarks.
    ViewDistance m_viewDistance {ViewDistance::SLICES_IN_RANGE};

    /// Defines whether or not interactions are enabled with distances.
    bool m_interactive {true};

    /// Defines the priority of the interactor.
    int m_priority {2};

    /// Defines the current picked data, reset by buttonReleaseEvent(MouseButton, int, int).
    std::shared_ptr<Landmark> m_pickedData {nullptr};

    /// Defines the mask used to filter out entities when the distance is auto snapped.
    std::uint32_t m_queryMask {0xFFFFFFFF};

    /// Defines the mask used to filter landmarks, it optimizes the ray launched to retrieve the picked distance.
    std::uint32_t m_landmarksQueryFlag {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Initial group configuration
    /// @{
    std::string m_currentGroup {"Landmarks"};
    sight::data::landmarks::color_t m_currentColor {1.0F, 1.0F, 0.0F, 1.0F};
    sight::data::landmarks::size_t m_currentSize {32.0F};
    sight::data::landmarks::Shape m_currentShape {sight::data::landmarks::Shape::SPHERE};
    /// @}

    /// Allow to limit the number of landmarks
    /// @{
    std::map<std::string, std::size_t> m_groupMax;
    std::optional<size_t> m_visibleMax {std::nullopt};
    std::optional<size_t> m_totalMax {std::nullopt};
    /// @}

    std::set<std::string> m_movableGroups;

    /// Adaptor edit "mode":
    /// - DISPLAY is read-only,
    /// - MOVE allows to modify existing landmarks position
    /// - EDIT allows to add/remove and move landmarks
    enum EditMode : std::uint8_t
    {
        DISPLAY = 1 << 0,
        MOVE    = 1 << 1,
        EDIT    = 1 << 2
    };

    std::uint8_t m_editMode {std::uint8_t(EditMode::DISPLAY)};

    /// Whether all landmarks can be modified or only landmarks belonging to the current group
    bool m_canOnlyModifyCurrent = false;

    /// True if the landmarks can be renamed by the user
    bool m_renamingAllowed = true;

    /// True if we must show the contextual menu. It must be shown if the landmark already existed and the landmark
    /// wasn't moved.
    bool m_mustShowContextualMenu = false;

    QWidget* m_contextualMenu = nullptr;

    /// Auto-delete the event filter in the end
    std::unique_ptr<DeleteContextualMenuWhenFocusOut> m_eventFilter = nullptr;

    static constexpr std::string_view s_LANDMARKS_INOUT    = "landmarks";
    static constexpr std::string_view s_IMAGE_SERIES_INOUT = "imageSeries";
    static constexpr std::string_view s_IMAGE_INPUT        = "image";

    sight::data::ptr<sight::data::landmarks, sight::data::Access::inout> m_landmarks {this, s_LANDMARKS_INOUT, true};
    sight::data::ptr<sight::data::image_series, sight::data::Access::inout> m_imageSeries
    {this, s_IMAGE_SERIES_INOUT, true};
    sight::data::ptr<sight::data::image, sight::data::Access::in> m_image {this, s_IMAGE_INPUT, true, true};
};

} // namespace sight::module::viz::scene3d_qt::adaptor.
