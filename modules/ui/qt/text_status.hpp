/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <data/string.hpp>

#include <ui/__/editor.hpp>

#include <QPointer>

class QLabel;

namespace sight::module::ui::qt
{

/**
 * @brief This service is used to displays and update values (int, double or string) in a QLabel.
 * Values are set using slots or using a data::string input.
 *
 * @section Slots Slots
 * - \b setIntParameter(int): display the value in the QLabel.
 * - \b setDoubleParameter(double): display the value in the QLabel.
 * - \b setBoolParameter(int): display the value in the QLabel.
 * - \b setStringParameter(int): display the value in the QLabel.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::text_status">
        <in key="string" uid="..." />
        <label>my label</label>
        <color>#FF0000</color>
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b string(data::string, optional): string data to display.
 *
 * @subsection Configuration Configuration
 * - \b label (optional, default="") : text to show before size of the vector
 * - \b color (optional, default="red") : needed color of the displayed label in a CSS style as names (ex: red),
 * rgb/rgba (ex: rgb(0,255,137,0.3)) or hexadecimal (ex: #355C66).
 */
class MODULE_UI_QT_CLASS_API text_status final : public QObject,
                                                 public sight::ui::editor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(text_status, sight::ui::editor);

    /// Initializes slots and member.
    MODULE_UI_QT_API text_status();

    /// Destroys the service.
    MODULE_UI_QT_API ~text_status() override;

private:

    /// Configures the service.
    void configuring() override;

    /// Installs the layout and gets the input data if it exists and displays it.
    void starting() override;

    /// Gets the input data if it exists and displays it.
    void updating() override;

    /// Destroys the layout.
    void stopping() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::object::MODIFIED_SIG of s_STRING_INPUT to service::slots::UPDATE
     */
    connections_t auto_connections() const override;

    /// Sets the interger to display.
    void setIntParameter(int _val);

    /// Sets the double to display.
    void setDoubleParameter(double _val);

    /// Sets the boolean to display.
    void setBoolParameter(bool _val);

    /// Sets the string to display.
    void setStringParameter(std::string _val);

    /// Stores the label.
    QPointer<QLabel> m_labelValue;

    /// Stores the static text to be displayed.
    QPointer<QLabel> m_labelStaticText;

    static constexpr std::string_view s_STRING_INPUT = "string";
    data::ptr<data::string, sight::data::Access::in> m_string {this, s_STRING_INPUT, true, true};
};

} // namespace sight::module::ui::qt
