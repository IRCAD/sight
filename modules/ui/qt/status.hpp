/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include <QPointer>
#include <QToolButton>

namespace sight::module::ui::qt
{

/**
 * @brief Shows a colored circle (red, orange, green) representing a status.
 *
 * @note To change the status color, you should call the slots 'changeToGreen', * 'changeToOrange', 'changeToRed'
 * or 'toggleGreenRed'.
 *
 * @brief Configures the status tooltip
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="sight::module::ui::qt::status">
       <layout>horizontal|vertical</layout>
       <labels display="under|beside">
           <name>SCP Server</name>
           <name>TCP Server</name>
       </labels>
       <red>Stopped</red>
       <green>Tracking</green>
       <orange>Started</orange>
   </service>
   @endcode
 * - \b count (optional, '1' by default): the number of status
 * - \b layout(optional, 'horizontal' by default): orientation of the layout
 * - \b labels (optional): the description associated to the indicators when count > 1
 *   - \b display (optional, 'under' by default): location of the label display
 *   - \b name (optional): the description associated to each indicator
 * - \b name (optional): the description associated to the indicator when count = 1 or is missing
 * - \b red (optional): the description associated to the red status
 * - \b green (optional): the description associated to the green status
 * - \b orange (optional): the description associated to the orange status
 *
 * @section Slots Slots
 * - \b change_to_green(): This slot allows to change the indicator color to green. If there is more than one status, it
 * changes them all.
 * - \b change_to_red(): This slot allows to change the indicator color to red. If there is more than one status, it
 * changes them all.
 * - \b change_to_orange(): This slot allows to change the indicator color to orange. If there is more than one status,
 * it
 * changes them all.
 * - \b toggle_green_red(bool): This slot allows to change the indicator color to green or red. If there is more than
 * one,
 * it toggle them all.
 * - \b change_nth_to_green(int): This slot allows to change the indicator color to green for the ith status.
 * - \b change_nth_to_red(int): This slot allows to change the indicator color to red for the ith status.
 * - \b change_nth_to_orange(int): This slot allows to change the indicator color to orange for the ith status.
 * - \b toggle_nth_green_red(int,bool): This slot allows to change the indicator color to green or red for the ith
 * status.
 */
class MODULE_UI_QT_CLASS_API status : public QObject,
                                      public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(status, sight::ui::editor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API status() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~status() noexcept override = default;

    struct slots
    {
        static inline const core::com::slots::key_t CHANGE_TO_GREEN_SLOT      = "change_to_green";
        static inline const core::com::slots::key_t CHANGE_TO_RED_SLOT        = "change_to_red";
        static inline const core::com::slots::key_t CHANGE_TO_ORANGE_SLOT     = "change_to_orange";
        static inline const core::com::slots::key_t TOGGLE_GREEN_RED_SLOT     = "toggle_green_red";
        static inline const core::com::slots::key_t CHANGE_NTH_TO_GREEN_SLOT  = "change_nth_to_green";
        static inline const core::com::slots::key_t CHANGE_NTH_TO_RED_SLOT    = "change_nth_to_red";
        static inline const core::com::slots::key_t CHANGE_NTH_TO_ORANGE_SLOT = "change_nth_to_orange";
        static inline const core::com::slots::key_t TOGGLE_NTH_GREEN_RED_SLOT = "toggle_nth_green_red";
    };

protected:

    void configuring(const config_t& _config) override;
    void starting() override;
    void stopping() override;

    /// Does nothing
    void updating() override;

    /// SLOT : change label color to green
    void change_to_green();

    /// SLOT : change label color to red
    void change_to_red();

    /// SLOT : change label color to orange
    void change_to_orange();

    /// SLOT : change label color (true = green, false = red)
    void toggle_green_red(bool _green);

    /// SLOT : change nth label color to green
    void change_nth_to_green(int _index);

    /// SLOT : change nth label color to red
    void change_nth_to_red(int _index);

    /// SLOT : change nth label color to orange
    void change_nth_to_orange(int _index);

    /// SLOT : change nth label color (true = green, false = red)
    void toggle_nth_green_red(int _index, bool _green);

private:

    QVector<QPointer<QToolButton> > m_label_status;

    std::string m_green_tooltip;  ///< Tooltip for green status
    std::string m_red_tooltip;    ///< Tooltip for red status
    std::string m_orange_tooltip; ///< Tooltip for orange status
    std::string m_orientation;    ///< Layout orientation

    enum class label_display
    {
        UNDER,
        BESIDE
    } m_label_display {label_display::UNDER};
};

} // namespace sight::module::ui::qt
