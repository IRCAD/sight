/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <data/color.hpp>
#include <data/ptr.hpp>

#include <ui/__/editor.hpp>

#include <QPointer>
#include <QToolButton>

namespace sight::module::ui::qt
{

/**
 * @brief Shows a colored circle representing a status.
 *
 * @note To change the status color, you should call the slots 'change_to_green', 'change_to_orange', 'change_to_red'
 * or 'toggle_green_red', or modify the 'color' data::property for arbitrary colors.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="sight::module::ui::qt::status">
       <name>Status</name>
   </service>
   @endcode
 * - \b name (optional): the label associated to the indicator
 *
 * @section Slots Slots
 * - \b change_to_green(): This slotallows to change the indicator color to green.
 * - \b change_to_red(): This slotallows to change the indicator color to red.
 * - \b change_to_orange(): This slotallows to change the indicator color to orange.
 * - \b toggle_green_red(bool): This slotallows to change the indicator color to green or red.
 *
 * @section Data Data
 * - \b color (sight::data::color, optional): a color property to set arbitrary colors on the circle.
 */
class status : public QObject,
               public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(status, sight::ui::editor);

    /// Constructor. Do nothing.
    status() noexcept;

    /// Destructor. Do nothing.
    ~status() noexcept override = default;

    struct slots
    {
        static inline const core::com::slots::key_t CHANGE_TO_GREEN_SLOT  = "change_to_green";
        static inline const core::com::slots::key_t CHANGE_TO_RED_SLOT    = "change_to_red";
        static inline const core::com::slots::key_t CHANGE_TO_ORANGE_SLOT = "change_to_orange";
        static inline const core::com::slots::key_t TOGGLE_GREEN_RED_SLOT = "toggle_green_red";
    };

    /// Defines the automatic connections between signals and slots of the service.
    service::connections_t auto_connections() const override;

protected:

    void configuring(const config_t& _config) override;
    void starting() override;
    void stopping() override;

    /// Does nothing
    void updating() override;

    /// SLOT: change label color to green
    void change_to_green();

    /// SLOT: change label color to red
    void change_to_red();

    /// SLOT: change label color to orange
    void change_to_orange();

    /// SLOT: change label color (true = green, false = red)
    void toggle_green_red(bool _green);

private:

    /// Update the circle color from the property
    void update_circle_color();

    QPointer<QToolButton> m_status_button;

    /// Label for the status
    std::string m_label;
    std::string m_orientation; ///< Layout orientation

    enum class label_display : std::uint8_t
    {
        under,
        beside
    } m_label_display {label_display::under};

    /// Previous color value to avoid unnecessary updates
    sight::data::color::value_t m_prev_color_value {0.0F, 0.0F, 0.0F, 0.0F};

    /// The color property
    sight::data::property<sight::data::color> m_color {this, "color", sight::data::color(1.0F, 0.0F, 0.0F, 1.0F)};
};

} // namespace sight::module::ui::qt
