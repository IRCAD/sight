/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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
 * - \b pickPoint(::fwDataTools::PickingInfo) : Add picked landmark.
 * - \b addPoint(std::string) : Add point to editor.
 * - \b removePoint(std::string, size_t) : Remove point from editor.
 * - \b modifyPoint(std::string, size_t) : Update editor when a point has moved.
 * - \b selectPoint(std::string, size_t) : Select point in editor.
 * - \b deselectPoint(std::string, size_t) : Deselect item in editor.
 * - \b addGroup(std::string) : Add group to editor.
 * - \b removeGroup(std::string) : Removes group from editor.
 * - \b modifyGroup(std::string) : Update group attributes.
 * - \b renameGroup(std::string, std::string) : Renames group in editor.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::uiMeasurementQt::editor::SLandmarks" >
           <inout key="landmarks" uid="..." />
           <size>10.0</size>
           <opacity>0.5</opacity>
           <advanced>yes</advanced>
        </service>
       @endcode
 * @subsection In-Out In-Out
 * - \b landmarks [::fwData::Landmarks]: the landmarks structure on which this editor is working.
 *
 * @subsection Configuration Configuration
 * - \b size (optional, default="10.0") : default size of created landmarks
 * - \b opacity (optional, default="1.0") : default opacity of created landmarks
 * - \b advanced (optional, default="no") : if "yes", use the advanced mode displaying point information
 * and groups with multiple points.
 */
class UIMEASUREMENTQT_CLASS_API SLandmarks : public QObject,
                                             public ::fwGui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceMacro(SLandmarks, ::fwGui::editor::IEditor);

    /// Constructor. Do nothing.
    UIMEASUREMENTQT_API SLandmarks() noexcept;

    /// Destructor. Do nothing.
    UIMEASUREMENTQT_API virtual ~SLandmarks() noexcept;

    UIMEASUREMENTQT_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     *
     * This method launches the IEditor::starting method.
     */
    virtual void starting() override;

    /**
     * @brief Destroy the layout.
     *
     * This method launches the IEditor::stopping method.
     */
    virtual void stopping() override;

    /// Do nothing
    virtual void updating() override;

    virtual void configuring() override;

private:

    /// This method is called when a color button is clicked
    void onColorButton();

    /// Called when a group name is changed
    void onGroupNameEdited(QTreeWidgetItem* item, int column);

    /// Called when a new group is selected in the editor.
    void onSelectionChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

    /// Called when a group's point size is modified.
    void onSizeChanged(int newSize);

    /// Called when a group's opacity is modified.
    void onOpacityChanged(int newOpacity);

    /// Called when a group's visibility is turned on or off.
    void onVisibilityChanged(int visibility);

    /// Called when the landmarks' shape is changed for a group.
    void onShapeChanged(const QString& shape);

    /// Called when the new group button is pressed, adds an empty landmark group in our data.
    void onAddNewGroup();

    /// Called when the remove button is pressed, deletes selected group or point.
    void onRemoveSelection();

    /// Slot: Adds a new landmark.
    void addPickedPoint(::fwDataTools::PickingInfo pickingInfo);

    /// Slot: Add point to editor.
    void addPoint(std::string groupName);

    /// Slot: Update point coordinates in editor.
    void modifyPoint(std::string groupName, size_t index);

    /// Slot: select the point's corresponding item in editor.
    void selectPoint(std::string groupName, size_t index);

    /// Slot: deselect the currently selected item.
    void deselectPoint(std::string groupName, size_t index);

    /// Slot: add a landmark group to the editor.
    void addGroup(std::string name);

    /// Slot: remove group from editor
    void removeGroup(std::string name);

    /// Slot: remove point from editor
    void removePoint(std::string groupName, size_t index);

    /// Slot: rename group in editor.
    void renameGroup(std::string oldName, std::string newName);

    /// Slot: update group properties in editor.
    void modifyGroup(std::string name);

    /// Gets the name of the currently selected group, returns false if no group is selected.
    bool currentSelection(std::string& selection) const;

    /// Get tree item representing the group.
    QTreeWidgetItem* getGroupItem(const std::string& groupName) const;

    /// Generate a group name that doesn't exist already.
    std::string generateNewGroupName() const;

    /// Generate a new random color
    std::array<float, 4> generateNewColor();

    /// Converts a landmark color to a QColor.
    static QColor convertToQColor(const ::fwData::Landmarks::ColorType& color);

    /// Draws a colored square on the button.
    static void setColorButtonIcon(QPushButton* button, const QColor& color);

    QPointer<QTreeWidget> m_treeWidget;

    QPointer<QWidget> m_groupEditorWidget;

    QPointer<QSlider> m_sizeSlider;

    QPointer<QSlider> m_opacitySlider;

    QPointer<QCheckBox> m_visibilityCheckbox;

    QPointer<QComboBox> m_shapeSelector;

    QPointer<QPushButton> m_newGroupButton;

    QPointer<QPushButton> m_removeButton;

    /// Used to disable/enable advanced mode.
    bool m_advancedMode;

    /// Used to set the default landmark size
    float m_defaultLandmarkSize;

    /// Used to set the default landmark opacity
    float m_defaultLandmarkOpacity;

};
} // namespace editor
} // uiMeasurementQt
