/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "modules/ui/qt/config.hpp"

#include <data/Landmarks.hpp>
#include <data/Matrix4.hpp>
#include <data/tools/PickingInfo.hpp>

#include <ui/base/IEditor.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QSlider>
#include <QTreeWidget>

namespace sight::module::ui::qt::metrics
{

/**
 * @brief This service defines a graphical editor to edit landmarks.
 *
 * @section Slots Slots
 * - @deprecated \b addPickedPoint(data::tools::PickingInfo): adds or removes picked landmark. call \b
 * pick(data::tools::PickingInfo) instead.
 *
 * - \b pick(data::tools::PickingInfo): adds or removes picked landmark.
 * - \b addPoint(std::string): adds a point to editor.
 * - \b modifyPoint(std::string, size_t): updates the editor when a point has moved.
 * - \b selectPoint(std::string, size_t): selects a point in the editor.
 * - \b deselectPoint(std::string, size_t): deselect a point in the editor.
 * - \b removePoint(std::string, size_t): removes a point from editor.
 * - \b addGroup(std::string): adds a group to the editor.
 * - \b removeGroup(std::string): removes a group from the editor.
 * - \b modifyGroup(std::string): updates a group attributes.
 * - \b renameGroup(std::string, std::string): renames a group in the editor.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::ui::qt::metrics::SLandmarks" >
           <inout key="landmarks" uid="..." />
           <in key="matrix" uid="..." />
           <text>Use 'Ctrl+Left Click' to add new landmarks</text>
           <size>10.0</size>
           <opacity>0.5</opacity>
           <advanced>true</advanced>
        </service>
       @endcode
 *
 * @subsection In In
 * - \b matrix [sight::data::Matrix4] (optional): Matrix used to compute transformation from the picked
 * point to the landmarks
 *
 *  @subsection In-Out In-Out
 * - \b landmarks [sight::data::Landmarks]: the landmarks structure on which this editor is working.
 *
 * @subsection Configuration Configuration
 * - \b text (optional): text displayed at the top of this editor.
 * - \b size (optional, default="10.0"): default size of created landmarks.
 * - \b opacity (optional, default="1.0"): default opacity of created landmarks.
 * - \b advanced (optional, default="false"): if "true", use the advanced mode displaying point information
 *      and groups with multiple points.
 */
class MODULE_UI_QT_CLASS_API SLandmarks final : public QObject,
                                                public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SLandmarks, sight::ui::base::IEditor);

    /// Initializes slots.
    MODULE_UI_QT_API SLandmarks() noexcept;

    /// Destroys the service.
    MODULE_UI_QT_API ~SLandmarks() noexcept override;

    /// Configures the service.
    void configuring() override;

    /// Installs the layout.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Landmarks::s_MODIFIED_SIG of s_LANDMARKS_INOUT to
     * module::ui::qt::metrics::SLandmarks::s_UPDATE_SLOT
     * Connect data::Landmarks::s_POINT_ADDED_SIG of s_LANDMARKS_INOUT to
     * module::ui::qt::metrics::SLandmarks::s_ADD_POINT_SLOT
     * Connect data::Landmarks::s_POINT_MODIFIED_SIG of s_LANDMARKS_INOUT to
     * module::ui::qt::metrics::SLandmarks::s_MODIFY_POINT_SLOT
     * Connect data::Landmarks::s_POINT_SELECTED_SIG of s_LANDMARKS_INOUT to
     * module::ui::qt::metrics::SLandmarks::s_SELECT_POINT_SLOT
     * Connect data::Landmarks::s_POINT_DESELECTED_SIG of s_LANDMARKS_INOUT to
     * module::ui::qt::metrics::SLandmarks::s_DESELECT_POINT_SLOT
     * Connect data::Landmarks::s_GROUP_ADDED_SIG of s_LANDMARKS_INOUT to
     * module::ui::qt::metrics::SLandmarks::s_ADD_GROUP_SLOT
     * Connect data::Landmarks::s_GROUP_REMOVED_SIG of s_LANDMARKS_INOUT to
     * module::ui::qt::metrics::SLandmarks::s_REMOVE_GROUP_SLOT
     * Connect data::Landmarks::s_POINT_REMOVED_SIG of s_LANDMARKS_INOUT to
     * module::ui::qt::metrics::SLandmarks::s_REMOVE_POINT_SLOT
     * Connect data::Landmarks::s_GROUP_MODIFIED_SIG of s_LANDMARKS_INOUT to
     * module::ui::qt::metrics::SLandmarks::s_MODIFY_GROUP_SLOT
     * Connect data::Landmarks::s_GROUP_RENAMED_SIG of s_LANDMARKS_INOUT to
     * module::ui::qt::metrics::SLandmarks::s_RENAME_GROUP_SLOT
     */
    KeyConnectionsMap getAutoConnections() const override;

    /// Resets the interface content and create connections between widgets and this service.
    void updating() override;

    /// Destroys the layout.
    void stopping() override;

    /// Called when a color button is clicked.
    void onColorButton();

    /**
     * @brief Called when a group name is changed.
     * @param _item the changed item.
     * @param _column the changed column
     *
     * @pre _column must be 0.
     */
    void onGroupNameEdited(QTreeWidgetItem* _item, int _column);

    /**
     * @brief Called when a new group is selected in the editor.
     * @param _current the new selected item.
     * @param _previous the old selected item.
     */
    void onSelectionChanged(QTreeWidgetItem* _current, QTreeWidgetItem* _previous);

    /**
     * @brief Called when a group's point size is modified.
     * @param _newSize the new size of the group.
     */
    void onSizeChanged(int _newSize);

    /**
     * @brief Called when a group's opacity is modified.
     * @param _newOpacity the new opacity of the group.
     */
    void onOpacityChanged(int _newOpacity);

    /**
     * @brief Called when a group's visibility is turned on or off.
     * @param _visibility the visibility status
     */
    void onVisibilityChanged(int _visibility);

    /**
     * @brief Called when the landmarks' shape is changed for a group.
     * @param _shape the new shape of the group.
     *
     * @pre _shape must be 'Cube' or 'Sphere'.
     */
    void onShapeChanged(const QString& _shape);

    /// Called when the new group button is pressed, adds an empty landmark group in our data.
    void onAddNewGroup();

    /// Called when the remove button is pressed, deletes selected group or point.
    void onRemoveSelection();

    /**
     * @brief SLOT: adds or removes a landmark from picking information.
     *
     * Interactions will take place while holding down the button. The following actions are available:
     * - CTRL + left mouse click: adds a new landmarks in the current selected group or create a new group to add it.
     * - CTRL + right mouse click: removes the landmark at the closest picking position.
     *
     * @deprecated Uses pick(data::tools::PickingInfo _info) instead.
     *
     * @param _pickingInfo Picking information.
     */
    void addPickedPoint(data::tools::PickingInfo _pickingInfo);

    /**
     * @brief SLOT: adds or removes a landmark from picking information.
     *
     * Interactions will take place while holding down the button. The following actions are available:
     * - CTRL + left mouse click: adds a new landmarks in the current selected group or create a new group to add it.
     * - CTRL + right mouse click: removes the landmark at the closest picking position.
     *
     * @param _info contains picking information.
     */
    void pick(data::tools::PickingInfo _info);

    /**
     * @brief SLOT: adds a point to the editor.
     * @param _groupName the group name where the point is added.
     */
    void addPoint(std::string _groupName);

    /**
     * @brief Slot: updates a point coordinates in the editor.
     * @param _groupName the group name of the updated point.
     * @param _index the index of the point to update.
     */
    void modifyPoint(std::string _groupName, size_t _index);

    /**
     * @brief SLOT: selects the point's corresponding item in the editor.
     * @param _groupName the group name of the selected point.
     * @param _index the index of the point to select.
     */
    void selectPoint(std::string _groupName, size_t _index);

    /// Slot: deselects the currently selected item.
    void deselectPoint(std::string, size_t);

    /**
     * @brief Slot: adds a landmark group to the editor.
     * @param _name Name of the new group.
     */
    void addGroup(std::string _name);

    /**
     * @brief SLOT: removes a group from the editor.
     * @param _name The group name to remove.
     */
    void removeGroup(std::string _name);

    /**
     * @brief SLOT: removes point from editor
     * @param _groupName the group name of the point the remove.
     * @param _index the index of the point to remove.
     */
    void removePoint(std::string _groupName, size_t _index);

    /**
     * @brief SLOT: renames a group in the editor.
     * @param _oldName the old name of the group.
     * @param _newName the new name of the group.
     */
    void renameGroup(std::string _oldName, std::string _newName);

    /**
     * @brief SLOT: updates a group properties in the editor.
     * @param _name The group name to updates.
     */
    void modifyGroup(std::string _name);

    /**
     * @brief Gets the name of the currently selected group.
     * @param [out] _selection the name of the currently selected group.
     * @return false if no group is selected.
     */
    bool currentSelection(std::string& _selection) const;

    /**
     * @brief Gets tree item representing the group.
     * @param _groupName the name of the item to find.
     * @return The item representing _groupName.
     */
    QTreeWidgetItem* getGroupItem(const std::string& _groupName) const;

    /**
     * @brief Generates a group name that doesn't exist already.
     * @return A group name that doesn't exist already.
     */
    std::string generateNewGroupName() const;

    /**
     * @brief Generates a new random color.
     * @return An array of 4 float (between 0.f and 1.f) representing an rgba color.
     */
    std::array<float, 4> generateNewColor();

    /**
     * @brief Converts a landmark color to a QColor.
     * @param _color The landmarks color type.
     * @return A QColor with same colour value than _color.
     */
    static QColor convertToQColor(const data::Landmarks::ColorType& _color);

    /**
     * @brief Draws a colored square on the button.
     * @param button the button where the square will be drawn.
     * @param _color the color of the square.
     */
    static void setColorButtonIcon(QPushButton* button, const QColor& _color);

    /// Contains a tree representing landmarks sorted by their groups.
    QPointer<QTreeWidget> m_treeWidget;

    /// Contains all widgets.
    QPointer<QWidget> m_groupEditorWidget;

    /// Contains the slider used to change the size of the current selected group.
    /// @see onSizeChanged(int)
    QPointer<QSlider> m_sizeSlider;

    /// Contains the slider used to change the opacity of the current selected group.
    /// @see onOpacityChanged(int)
    QPointer<QSlider> m_opacitySlider;

    /// Contains the button used to change the visibility of the current selected group.
    /// @see onVisibilityChanged(int)
    QPointer<QCheckBox> m_visibilityCheckbox;

    /// Contains the combo box used to change the shape of the current selected group.
    /// @see onShapeChanged(const QString&)
    QPointer<QComboBox> m_shapeSelector;

    /// Contains the button used to adds a new empty group.
    /// @see onAddNewGroup()
    QPointer<QPushButton> m_newGroupButton;

    /// Contains the button used to remove a group or a landmark.
    /// @see onRemoveSelection()
    QPointer<QPushButton> m_removeButton;

    /// Enables/disables the advanced mode.
    bool m_advancedMode {false};

    /// Sets the default landmark size.
    float m_defaultLandmarkSize {10.f};

    /// Sets the default landmark opacity.
    float m_defaultLandmarkOpacity {1.f};

    /// Sets the text displayed at the top of this editor.
    std::string m_text {"Use 'Ctrl+Left Click' to add new landmarks"};

    static constexpr std::string_view s_LANDMARKS_INOUT = "landmarks";
    static constexpr std::string_view s_MATRIX_IN       = "matrix";
    data::ptr<data::Matrix4, sight::data::Access::in> m_matrix {this, s_MATRIX_IN, false, true};
    data::ptr<data::Landmarks, sight::data::Access::inout> m_landmarks {this, s_LANDMARKS_INOUT, true};
};

} // sight::module::ui::qt::metrics
