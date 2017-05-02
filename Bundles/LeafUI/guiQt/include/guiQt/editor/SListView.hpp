/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_EDITOR_SLISTVIEW_HPP__
#define __GUIQT_EDITOR_SLISTVIEW_HPP__

#include "guiQt/config.hpp"

#include <gui/editor/IEditor.hpp>

#include <QListWidget>
#include <QPointer>

namespace guiQt
{

namespace editor
{

/**
 * @brief   This editor allows to draw a list.
 * The currently selected item can be deleted by pressing on "del" key
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::guiQt::editor::SListView"/>
    </service>
   @endcode
 *
 * @section Signals Signals
 * - \b itemAdded(int): This editor emits the signal "itemAdded" with the index value of added item.
 * - \b itemRemoved(int): This editor emits the signal "itemRemoved" with the index value of removed item.
 *
 * @section Slots Slots
 * - \b insertItem(int, std::string): This slot allows to add an item with its index and text.
 * - \b removeItem(int): This slot allows to remove the element at the index.
 */
class GUIQT_CLASS_API SListView : public QObject,
                                  public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SListView)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    GUIQT_API SListView() throw();

    /// Destructor. Do nothing.
    GUIQT_API virtual ~SListView() throw();

    /**@name Signals API
     * @{
     */

    GUIQT_API static const ::fwCom::Signals::SignalKeyType s_ITEM_ADDED_SIG;
    typedef ::fwCom::Signal<void (int)> ItemAddedSignalType;

    GUIQT_API static const ::fwCom::Signals::SignalKeyType s_ITEM_REMOVED_SIG;
    typedef ::fwCom::Signal<void (int)> ItemRemovedSignalType;

    /** @} */

    /**
     * @name Slots API
     * @{
     */

    GUIQT_API static const ::fwCom::Slots::SlotKeyType s_INSERT_ITEM_SLOT;
    GUIQT_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_ITEM_SLOT;

    ///@}

protected:

    /// used to catch the del key released event
    bool eventFilter(QObject* watched, QEvent* event);

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Installs the layout
    virtual void starting() throw(::fwTools::Failed);

    /// Destroys the layout
    virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing
    virtual void updating() throw(::fwTools::Failed);

    /// Does nothing
    virtual void swapping() throw(::fwTools::Failed);

    /// Configure the service
    virtual void configuring() throw(fwTools::Failed);

    /// Signal when the item will be removed
    ItemRemovedSignalType::sptr m_sigItemRemoved;

    /// Signal when an item is added
    ItemAddedSignalType::sptr m_sigAddedItem;

protected Q_SLOTS:

    /// SLOT : Called to insert an item at index.
    void insertItem( int index, std::string value );

    /// SLOT : Called to remove the item at the index position.
    void removeItem(int index);

private:

    QPointer<QListWidget> m_listWidget; /// listWidget
};

}
}

#endif /*__GUIQT_EDITOR_SLISTVIEW_HPP__*/
