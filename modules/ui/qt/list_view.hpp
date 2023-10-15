/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "modules/ui/qt/config.hpp"

#include <ui/__/editor.hpp>

#include <QListWidget>
#include <QPointer>

namespace sight::module::ui::qt
{

/**
 * @brief   This editor allows to draw a list.
 * The currently selected item can be deleted by pressing on "del" key
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::list_view"/>
    </service>
   @endcode
 *
 * @section Signals Signals
 * - \b itemAdded(int): This editor emits the signal "itemAdded" with the index value of added item.
 * - \b itemRemoved(int): This editor emits the signal "itemRemoved" with the index value of removed item.
 * - \b itemDoubleClicked(int): This editor emits the signal "itemDoubleClicked" with the index value of selected item.
 *
 * @section Slots Slots
 * - \b insertItem(int, std::string): This slot allows to add an item with its index and text.
 * - \b removeItem(int): This slot allows to remove the element at the index.
 */
class MODULE_UI_QT_CLASS_API list_view : public QObject,
                                         public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(list_view, sight::ui::editor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API list_view() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~list_view() noexcept override;

    /**@name Signals API
     * @{
     */

    MODULE_UI_QT_API static const core::com::signals::key_t ITEM_ADDED_SIG;
    typedef core::com::signal<void (int)> ItemAddedSignalType;

    MODULE_UI_QT_API static const core::com::signals::key_t ITEM_REMOVED_SIG;
    typedef core::com::signal<void (int)> ItemRemovedSignalType;

    MODULE_UI_QT_API static const core::com::signals::key_t ITEM_DOUBLE_CLICKED_SIG;
    typedef core::com::signal<void (int)> ItemDoubleClickedSignalType;

    /** @} */

    /**
     * @name Slots API
     * @{
     */

    MODULE_UI_QT_API static const core::com::slots::key_t INSERT_ITEM_SLOT;
    MODULE_UI_QT_API static const core::com::slots::key_t REMOVE_ITEM_SLOT;

    /// SLOT : Called to insert an item at index.
    void insertItem(int index, std::string value);

    /// SLOT : Called to remove the item at the index position.
    void removeItem(int index);

    ///@}

protected:

    /// used to catch the del key released event
    bool eventFilter(QObject* watched, QEvent* event) override;

    /// Installs the layout
    void starting() override;

    /// Destroys the layout
    void stopping() override;

    /// Does nothing
    void updating() override;

    /// Configure the service
    void configuring() override;

private:

    QPointer<QListWidget> m_listWidget; /// listWidget

private Q_SLOTS:

    /// called when an item is double clicked
    void onItemDoubleClicked(QListWidgetItem* item);
};

} //namespace sight::module::ui::qt
