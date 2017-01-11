/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_EDITOR_SSELECTIONMENUBUTTON_HPP__
#define __GUIQT_EDITOR_SSELECTIONMENUBUTTON_HPP__

#include "guiQt/config.hpp"

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>
#include <QAction>

#include <QObject>
#include <QPointer>

class QMenu;
class QActionGroup;
class QPushButton;


namespace guiQt
{

namespace editor
{

/**
 * @brief   This service show a menu button. The user can select one item in the menu.
 *
 * When the user select an item, the signal \c selected(int selection) is emitted: it sends the value of the selected
 * item.
 *
 * @note This service doesn’t modify its associated object, so it can work on any type of object.
 *
 * @section XML Example of configuration
 * @code{.xml}
    <service uid="sliceListNegato3DEditor" type="::gui::editor::IEditor" impl="::uiImage::SSelectionMenuButton">
        <text>...</text>
        <toolTip>...</toolTip>
        <items>
            <item text="One" value="1" />
            <item text="Two" value="2" />
            <item text="Six" value="6" />
        </items>
        <selected>2</selected>
    </service>
   @endcode
 *
 * - \b text (optional, default “>”): text displayed on the button
 * - \b toolTip (optional): button tool tip
 * - \b items: list of the menu items
 * - \b item: one item
 *    - \b text: the text displayed in the menu
 *    - \b value: the value emitted when the item is selected
 * - \b selected: the value of the item selected by default
 *
 * @section Signal Signal
 * - \b selected(int): This signal is emitted when the user select an item. Sends the item value.
 *
 * @section Slots Slots
 * - \b setEnabled(bool): This slots allows to enable/disable the button
 * - \b enable(): This slot allows to enable the button
 * - \b disable(): This slots allows to disable the button
 */
class GUIQT_CLASS_API SSelectionMenuButton : public QObject,
                                             public ::gui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (SSelectionMenuButton)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    GUIQT_API SSelectionMenuButton() throw();

    /// Destructor. Do nothing.
    GUIQT_API virtual ~SSelectionMenuButton() throw();

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     */
    virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping() throw(::fwTools::Failed);

    /// Do nothing
    virtual void updating() throw(::fwTools::Failed);

    /// Do nothing
    virtual void swapping() throw(::fwTools::Failed);

    /// Configure the editor.
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );

protected Q_SLOTS:
    /// This method is called when the popup menu is clicked. Notify the selection changed.
    void onSelection(QAction* action);

private:

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when an item is selected
    typedef ::fwCom::Signal< void (int) > SelectedSignalType;
    SelectedSignalType::sptr m_sigSelected;
    /**
     * @}
     */

    /**
     * @name Slots
     * @{
     */
    /// Slot: enable/disable the button
    void setEnabled(bool enabled);

    /// Slot: enable the button
    void enable();

    /// Slot: disable the button
    void disable();
    /**
     * @}
     */

    std::string m_text; ///< Text displayed on the button
    std::string m_toolTip; ///< Tool tip displayed on the button
    typedef std::pair <int, std::string> ItemType;
    typedef std::vector<ItemType> ItemContainerType;
    ItemContainerType m_items;

    QPointer < QMenu > m_pDropDownMenu;
    QPointer < QPushButton > m_dropDownButton;
    QPointer < QActionGroup > m_actionGroup;

    int m_selection;

};

} // namespace editor
} // namespace guiQt

#endif /*__GUIQT_EDITOR_SSELECTIONMENUBUTTON_HPP__*/
