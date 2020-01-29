/************************************************************************
 *
 * Copyright (C) 2017-2020 IRCAD France
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

#include "uiMeasurementQt/config.hpp"

#include <fwData/Landmarks.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QSlider>
#include <QTreeWidget>

namespace uiMeasurementQt
{
namespace editor
{

/**
 * @brief This service defines a graphical editor to edit landmarks.
 *
 * @section Slots Slots
 * - \b @deprecated addPickedPoint(::fwDataTools::PickingInfo) : adds or removes picked landmark.
 * - \b pick(::fwDataTools::PickingInfo) : adds or removes picked landmark.
 * - \b addPoint(std::string) : adds a point to editor.
 * - \b modifyPoint(std::string, size_t) : updates the editor when a point has moved.
 * - \b selectPoint(std::string, size_t) : selects a point in the editor.
 * - \b deselectPoint(std::string, size_t) : deselect a point in the editor.
 * - \b removePoint(std::string, size_t) : removes a point from editor.
 * - \b addGroup(std::string) : adds a group to the editor.
 * - \b removeGroup(std::string) : removes a group from the editor.
 * - \b modifyGroup(std::string) : updates a group attributes.
 * - \b renameGroup(std::string, std::string) : renames a group in the editor.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::uiMeasurementQt::editor::SLandmarks" >
           <inout key="landmarks" uid="..." />
           <text>Use 'Ctrl+Left Click' to add new landmarks</text>
           <size>10.0</size>
           <opacity>0.5</opacity>
           <advanced>yes</advanced>
        </service>
       @endcode
 *
 * @subsection In-Out In-Out
 * - \b landmarks [::fwData::Landmarks]: the landmarks structure on which this editor is working.
 *
 * @subsection Configuration Configuration
 * - \b text (optional): text displayed at the top of this editor.
 * - \b size (optional, default="10.0") : default size of created landmarks.
 * - \b opacity (optional, default="1.0") : default opacity of created landmarks.
 * - \b advanced (optional, default="no") : if "yes", use the advanced mode displaying point information
 * and groups with multiple points.
 */
class UIMEASUREMENTQT_CLASS_API SLandmarks final : public QObject,
                                                   public ::fwGui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceMacro(SLandmarks, ::fwGui::editor::IEditor)

    /// Initializes slots.
    UIMEASUREMENTQT_API SLandmarks() noexcept;

    /// Destroys the service.
    UIMEASUREMENTQT_API virtual ~SLandmarks() noexcept;

private:

    /// Configures the service.
    virtual void configuring() override;

    /// Installs the layout.
    virtual void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Landmarks::s_MODIFIED_SIG of s_LANDMARKS_INOUT to
     *::uiMeasurementQt::editor::SLandmarks::s_UPDATE_SLOT
     * Connect ::fwData::Landmarks::s_POINT_ADDED_SIG of s_LANDMARKS_INOUT to
     *::uiMeasurementQt::editor::SLandmarks::s_ADD_POINT_SLOT
     * Connect ::fwData::Landmarks::s_POINT_MODIFIED_SIG of s_LANDMARKS_INOUT to
     *::uiMeasurementQt::editor::SLandmarks::s_MODIFY_POINT_SLOT
     * Connect ::fwData::Landmarks::s_POINT_SELECTED_SIG of s_LANDMARKS_INOUT to
     *::uiMeasurementQt::editor::SLandmarks::s_SELECT_POINT_SLOT
     * Connect ::fwData::Landmarks::s_POINT_DESELECTED_SIG of s_LANDMARKS_INOUT to
     *::uiMeasurementQt::editor::SLandmarks::s_DESELECT_POINT_SLOT
     * Connect ::fwData::Landmarks::s_GROUP_ADDED_SIG of s_LANDMARKS_INOUT to
     *::uiMeasurementQt::editor::SLandmarks::s_ADD_GROUP_SLOT
     * Connect ::fwData::Landmarks::s_GROUP_REMOVED_SIG of s_LANDMARKS_INOUT to
     *::uiMeasurementQt::editor::SLandmarks::s_REMOVE_GROUP_SLOT
     * Connect ::fwData::Landmarks::s_POINT_REMOVED_SIG of s_LANDMARKS_INOUT to
     *::uiMeasurementQt::editor::SLandmarks::s_REMOVE_POINT_SLOT
     * Connect ::fwData::Landmarks::s_GROUP_MODIFIED_SIG of s_LANDMARKS_INOUT to
     *::uiMeasurementQt::editor::SLandmarks::s_MODIFY_GROUP_SLOT
     * Connect ::fwData::Landmarks::s_GROUP_RENAMED_SIG of s_LANDMARKS_INOUT to
     *::uiMeasurementQt::editor::SLandmarks::s_RENAME_GROUP_SLOT
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

    /// Resets the interface content and create connections between widgets and this service.
    virtual void updating() override;

    /// Destroys the layout.
    virtual void stopping() override;

    /// Called when a color button is clicked.
    void onColorButton();

    /**
     * @brief Called when a group name is changed.
     * @param _item The changed item.
     * @param _column The changed column
     *
     * @pre _column must be 0.
     */
    void onGroupNameEdited(QTreeWidgetItem* _item, int _column);

    /**
     * @brief Called when a new group is selected in the editor.
     * @param _current The new selected item.
     * @param _previous The old selected item.
     */
    void onSelectionChanged(QTreeWidgetItem* _current, QTreeWidgetItem* _previous);

    /**
     * @brief Called when a group's point size is modified.
     * @param _newSize New size of the group.
     */
    void onSizeChanged(int _newSize);

    /**
     * @brief Called when a group's opacity is modified.
     * @param _newOpacity New opacity of the group.
     */
    void onOpacityChanged(int _newOpacity);

    /**
     * @brief Called when a group's visibility is turned on or off.
     * @param _visibility The visibility status
     */
    void onVisibilityChanged(int _visibility);

    /**
     * @brief Called when the landmarks' shape is changed for a group.
     * @param _shape The new shape of the group.
     *
     * @pre _shape must be 'Cube' or 'Sphere'.
     */
    void onShapeChanged(const QString& _shape);

    /// Called when the new group button is pressed, adds an empty landmark group in our data.
    void onAddNewGroup();

    /// Called when the remove button is pressed, deletes selected group or point.
    void onRemoveSelection();

    /**
     * @brief SLOT: adds or removes a landmark from picking informations.
     *
     * - CTRL + left mouse click: adds a new landmarks in the current selected group or create a new groupd to add it.
     * - CTRL + right mouse click: removes the landmark at the closest picking position.
     *
     * @deprecated Uses pick(::fwDataTools::PickingInfo _info) instead.
     *
     * @param _pickingInfo Picking informations.
     */
    void addPickedPoint(::fwDataTools::PickingInfo _pickingInfo);

    /**
     * @brief SLOT: adds or removes a landmark from picking informations.
     *
     * - CTRL + left mouse click: adds a new landmarks in the current selected group or create a new groupd to add it.
     * - CTRL + right mouse click: removes the landmark at the closest picking position.
     *
     * @param _info Picking informations.
     */
    void pick(::fwDataTools::PickingInfo _info);

    /**
     * @brief Slot: adds a point to the editor.
     * @param _groupName The group name where the point is added.
     */
    void addPoint(std::string _groupName);

    /**
     * @brief Slot: updates a point coordinates in the editor.
     * @param _groupName The group name of the updated point.
     * @param _index The index of the point to update.
     */
    void modifyPoint(std::string _groupName, size_t _index);

    /**
     * @brief Slot: selects the point's corresponding item in the editor.
     * @param _groupName The group name of the selected point.
     * @param _index The index of the point to select.
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
     * @brief Slot: removes a group from the editor.
     * @param _name The group name to remove.
     */
    void removeGroup(std::string _name);

    /**
     * @brief Slot: removes point from editor
     * @param _groupName The group name of the point the remove.
     * @param _index The index of the point to remove.
     */
    void removePoint(std::string _groupName, size_t _index);

    /**
     * @brief Slot: renames a group in the editor.
     * @param _oldName Old name of the group.
     * @param _newName New name of the group.
     */
    void renameGroup(std::string _oldName, std::string _newName);

    /**
     * @brief Slot: updates a group properties in the editor.
     * @param _name The group name to updates.
     */
    void modifyGroup(std::string _name);

    /**
     * @brief Gets the name of the currently selected group.
     * @param [out] _selection The name of the currently selected group.
     * @return false if no group is selected.
     */
    bool currentSelection(std::string& _selection) const;

    /**
     * @brief Gets tree item representing the group.
     * @param _groupName The name of the item to find.
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
    static QColor convertToQColor(const ::fwData::Landmarks::ColorType& _color);

    /**
     * @brief Draws a colored square on the button.
     * @param button The button where the square will be drawn.
     * @param _color The color of the square.
     */
    static void setColorButtonIcon(QPushButton* button, const QColor& _color);

    /// Contains a tree representing landmarks sorted by their groups.
    QPointer<QTreeWidget> m_treeWidget;

    /// Contains all widgets.
    QPointer<QWidget> m_groupEditorWidget;

    /// Contains the slider used to change the size of the current selected group.
    /// @ref onSizeChanged(int) "onSizeChanged(int)"
    QPointer<QSlider> m_sizeSlider;

    /// Contains the slider used to change the opacity of the current selected group.
    /// @ref onOpacityChanged(int) "onOpacityChanged(int)"
    QPointer<QSlider> m_opacitySlider;

    /// Contains the button used to change the visibility of the current selected group.
    /// @ref onVisibilityChanged(int) "onVisibilityChanged(int)"
    QPointer<QCheckBox> m_visibilityCheckbox;

    /// Contains the combo box used to change the shape of the current selected group.
    /// @ref onShapeChanged(const QString&) "onShapeChanged(const QString&)"
    QPointer<QComboBox> m_shapeSelector;

    /// Contains the button used to adds a new empty group.
    /// @ref onAddNewGroup() "onAddNewGroup()"
    QPointer<QPushButton> m_newGroupButton;

    /// Contains the button used to remove a group or a landmark.
    /// @ref onRemoveSelection() "onRemoveSelection()"
    QPointer<QPushButton> m_removeButton;

    /// Disables or enables advanced mode.
    bool m_advancedMode { false };

    /// Stores the default landmark size.
    float m_defaultLandmarkSize { 10.f };

    /// Stores the default landmark opacity.
    float m_defaultLandmarkOpacity { 1.f };

    /// Stores the text displayed at the top of this editor.
    std::string m_text { "Use 'Ctrl+Left Click' to add new landmarks" };

};
} // namespace editor
} // uiMeasurementQt
