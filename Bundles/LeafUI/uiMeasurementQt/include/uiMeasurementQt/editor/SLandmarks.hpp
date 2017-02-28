/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENTQT_EDITOR_SLANDMARKS_HPP__
#define __UIMEASUREMENTQT_EDITOR_SLANDMARKS_HPP__

#include "uiMeasurementQt/config.hpp"

#include <fwData/Landmarks.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <gui/editor/IEditor.hpp>

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
           <advanced>yes</advanced>
        </service>
       @endcode
 * @subsection In-Out In-Out
 * - \b landmarks [::fwData::Landmarks]: the landmarks structure on which this editor is working.
 *
 * @subsection Configuration Configuration
 * - \b advanced (optional, default="no") : if "yes", use the advanced mode displaying point information
 * and groups with multiple points.
 */
class UIMEASUREMENTQT_CLASS_API SLandmarks : public QObject,
                                             public ::gui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SLandmarks)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIMEASUREMENTQT_API SLandmarks() throw();

    /// Destructor. Do nothing.
    UIMEASUREMENTQT_API virtual ~SLandmarks() throw();

    UIMEASUREMENTQT_API virtual KeyConnectionsMap getAutoConnections() const;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     *
     * This method launches the IEditor::starting method.
     */
    virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief Destroy the layout.
     *
     * This method launches the IEditor::stopping method.
     */
    virtual void stopping() throw(::fwTools::Failed);

    /// Do nothing
    virtual void updating() throw(::fwTools::Failed);

    virtual void configuring() throw(fwTools::Failed);

private:

    /// This method is called when a color button is clicked
    void onColorButton();

    /// Called when a group name is changed
    void onGroupNameEdited(QTreeWidgetItem* item, int column);

    /// Called when a new group is selected in the editor.
    void onSelectionChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

    /// Called when a group's point size is modified.
    void onSizeChanged(int newSize);

    /// Called when a group's transparency is modified.
    void onTransparencyChanged(int newTransparency);

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
    static std::array<float, 4> generateNewColor();

    /// Converts a landmark color to a QColor.
    static QColor convertToQColor(const ::fwData::Landmarks::ColorType& color);

    /// Draws a colored square on the button.
    static void setColorButtonIcon(QPushButton* button, const QColor& color);

    QPointer<QTreeWidget> m_treeWidget;

    QPointer<QWidget> m_groupEditorWidget;

    QPointer<QSlider> m_sizeSlider;

    QPointer<QSlider> m_transparencySlider;

    QPointer<QCheckBox> m_visibilityCheckbox;

    QPointer<QComboBox> m_shapeSelector;

    QPointer<QPushButton> m_newGroupButton;

    QPointer<QPushButton> m_removeButton;

    /// Used to disable/enable advanced mode.
    bool m_advancedMode;

};
} // namespace editor
} // uiMeasurementQt

#endif /*__UIMEASUREMENTQT_EDITOR_SLANDMARKS_HPP__*/

