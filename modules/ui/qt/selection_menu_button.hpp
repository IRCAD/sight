/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/tools/failed.hpp>

#include <ui/__/editor.hpp>

#include <QAction>
#include <QObject>
#include <QPointer>

class QMenu;
class QActionGroup;
class QPushButton;

namespace sight::module::ui::qt
{

/**
 * @brief   This service show a menu button. The user can select one item in the menu.
 *
 * When the user select an item, the signal \c selected(int selection) is emitted: it sends the value of the selected
 * item.
 *
 * @note This service doesn't modify its associated object, so it can work on any type of object.
 *
 * @section XML Example of configuration
 * @code{.xml}
    <service uid="sliceListNegato3DEditor" type="sight::module::ui::qt::image::selection_menu_button">
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
 * - \b text (optional, default ">"): text displayed on the button
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
 * - \b set_enabled(bool): This slots allows to enable/disable the button
 * - \b enable(): This slot allows to enable the button
 * - \b disable(): This slots allows to disable the button
 */
class selection_menu_button : public QObject,
                              public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(selection_menu_button, sight::ui::editor);

    selection_menu_button() noexcept;
    ~selection_menu_button() noexcept override = default;

protected:

    /**
     * @brief Install the layout.
     */
    void starting() override;

    /**
     * @brief Destroy the layout.
     */
    void stopping() override;

    /// Do nothing
    void updating() override;

    /// Configure the editor.
    void configuring() override;

    /// Overrides
    void info(std::ostream& _sstream) override;

protected Q_SLOTS:

    /// This method is called when the popup menu is clicked. Notify the selection changed.
    void on_selection(QAction* _action);

private:

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when an item is selected
    using selected_signal_t = core::com::signal<void (int)>;
    selected_signal_t::sptr m_sig_selected;
    /**
     * @}
     */

    /**
     * @name Slots
     * @{
     */
    /// Slot: enable/disable the button
    void set_enabled(bool _enabled) override;

    /// Slot: enable the button
    void enable() override;

    /// Slot: disable the button
    void disable() override;
    /**
     * @}
     */

    std::string m_text {">"}; ///< Text displayed on the button
    std::string m_tool_tip;   ///< Tool tip displayed on the button
    using item_t           = std::pair<int, std::string>;
    using item_container_t = std::vector<item_t>;
    item_container_t m_items;

    QPointer<QMenu> m_p_drop_down_menu;
    QPointer<QPushButton> m_drop_down_button;
    QPointer<QActionGroup> m_action_group;

    int m_selection {0};
};

} // namespace sight::module::ui::qt
