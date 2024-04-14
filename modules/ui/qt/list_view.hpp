/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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
class list_view : public QObject,
                  public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(list_view, sight::ui::editor);

    /// Constructor. Do nothing.
    list_view() noexcept;

    /// Destructor. Do nothing.
    ~list_view() noexcept override;

    /**@name Signals API
     * @{
     */

    static const core::com::signals::key_t ITEM_ADDED_SIG;
    using item_added_signal_t = core::com::signal<void (int)>;

    static const core::com::signals::key_t ITEM_REMOVED_SIG;
    using item_removed_signal_t = core::com::signal<void (int)>;

    static const core::com::signals::key_t ITEM_DOUBLE_CLICKED_SIG;
    using item_double_clicked_signal_t = core::com::signal<void (int)>;

    /** @} */

    /**
     * @name Slots API
     * @{
     */

    static const core::com::slots::key_t INSERT_ITEM_SLOT;
    static const core::com::slots::key_t REMOVE_ITEM_SLOT;

    /// SLOT : Called to insert an item at index.
    void insert_item(int _index, std::string _value);

    /// SLOT : Called to remove the item at the index position.
    void remove_item(int _index);

    ///@}

protected:

    /// used to catch the del key released event
    bool eventFilter(QObject* _watched, QEvent* _event) override;

    /// Installs the layout
    void starting() override;

    /// Destroys the layout
    void stopping() override;

    /// Does nothing
    void updating() override;

    /// Configure the service
    void configuring() override;

private:

    QPointer<QListWidget> m_list_widget; /// listWidget

private Q_SLOTS:

    /// called when an item is double clicked
    void on_item_double_clicked(QListWidgetItem* _item);
};

} //namespace sight::module::ui::qt
