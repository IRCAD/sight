/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include <service/base.hpp>

#include <QShortcut>

namespace sight::module::ui::qt::com
{

/**
 * @brief   This service sends a signal when the associated shortcut is activated.
 * @section XML XML configuration
 * @code{.xml}
   <service uid="..." impl="sight::module::ui::qt::com::signal_shortcut" >
        <config shortcut="..." sid="..." />
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b shortcut: associated shortcut
 * - \b sid/wid (exclusive): id of the service/window associated to the gui container
 *   to which the shortcut will be associated
 *
 * @section Signals Signals
 * - \b activated(): This signal is emitted when the shortcut is received.
 */
class MODULE_UI_QT_CLASS_API signal_shortcut : public QObject,
                                               public service::base
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(signal_shortcut, service::base);

    /// Signal emitted when the shortcut is received.
    using activated_shortcut_signal_t = core::com::signal<void ()>;

    /// Constructor. Do nothing.
    MODULE_UI_QT_API signal_shortcut() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~signal_shortcut() noexcept override;

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    /// This method configures the service
    MODULE_UI_QT_API void configuring() override;

    /**
     * @brief This method enables the eventFilter
     */
    MODULE_UI_QT_API void starting() override;

    /**
     * @brief This method deletes the eventFilter
     */
    MODULE_UI_QT_API void stopping() override;

    /**
     * @brief This method does nothing.
     */
    MODULE_UI_QT_API void updating() override;

private Q_SLOTS:

    void on_activation();

private:

    /// string containing the shortcut to trigger
    std::string m_shortcut;

    /// Service id used to get the widget of the activity to set up a shortcut in
    /// Either this member or m_wid has to be specified
    std::string m_sid;

    /// Window id used to get the widget of the activity to set up a shortcut in
    /// Either this member or m_sid has to be specified
    std::string m_wid;

    /// Qt shortcut object
    QShortcut* m_shortcut_object {nullptr};
};

} // namespace sight::module::ui::qt::com
