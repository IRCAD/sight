/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/ui/qt/config.hpp"

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
 * - \b setEnabled(bool): This slots allows to enable/disable the button
 * - \b enable(): This slot allows to enable the button
 * - \b disable(): This slots allows to disable the button
 */
class MODULE_UI_QT_CLASS_API selection_menu_button : public QObject,
                                                     public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(selection_menu_button, sight::ui::editor);

    MODULE_UI_QT_API selection_menu_button() noexcept;
    MODULE_UI_QT_API ~selection_menu_button() noexcept override = default;

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
    void onSelection(QAction* action);

private:

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when an item is selected
    typedef core::com::signal<void (int)> SelectedSignalType;
    SelectedSignalType::sptr m_sigSelected;
    /**
     * @}
     */

    /**
     * @name Slots
     * @{
     */
    /// Slot: enable/disable the button
    void setEnabled(bool enabled) override;

    /// Slot: enable the button
    void enable() override;

    /// Slot: disable the button
    void disable() override;
    /**
     * @}
     */

    std::string m_text {">"}; ///< Text displayed on the button
    std::string m_toolTip;    ///< Tool tip displayed on the button
    typedef std::pair<int, std::string> ItemType;
    typedef std::vector<ItemType> ItemContainerType;
    ItemContainerType m_items;

    QPointer<QMenu> m_pDropDownMenu;
    QPointer<QPushButton> m_dropDownButton;
    QPointer<QActionGroup> m_actionGroup;

    int m_selection {0};
};

} // namespace sight::module::ui::qt
