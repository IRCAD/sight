/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
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
 * @brief   This service show a menu button. The user can select one item in the menu and the service sends a message
 * containing the value of the item. This menu is configurable in xml.
 * @class   SSelectionMenuButton
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

    /**
     * @brief Configure the editor.
     *
     * Example of configuration
     * @verbatim
       <service uid="sliceListNegato3DEditor" type="::gui::editor::IEditor" impl="::uiImage::SSelectionMenuButton" autoConnect="yes">
           <text>...</text>
           <toolTip>...</toolTip>
           <items>
               <item text="One" value="1" />
               <item text="Two" value="2" />
               <item text="Six" value="6" />
           </items>
           <selected>2</selected>
       </service>
       @endverbatim

     */
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


