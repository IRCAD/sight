/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include <data/boolean.hpp>

#include <service/base.hpp>

#include <QShortcut>

namespace sight::module::ui::qt::com
{

/**
 * @brief   This service sends a signal when the associated shortcut is activated.
 * @section XML XML configuration
 * @code{.xml}
   <service uid="..." impl="sight::module::ui::qt::com::signal_shortcut" >
        <properties enabled="false/true/${property_uid}"/>
        <config shortcut="..." sid="..." />
   </service>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b shortcut: associated shortcut
 * - \b sid/wid (exclusive): id of the service/window associated to the gui container
 *   to which the shortcut will be associated
 *
 * @section Signals Signals
 * - \b activated(): This signal is emitted when the shortcut is received.
 * - \b is_enabled(bool): This signal is emitted when the service change the enables state (current state is sent as a
 * boolean parameter)
 * - \b enabled(): This signal is emitted when the service is enabled.
 * - \b disabled(): This signal is emitted when the service is disabled.
 *
 * @section Slots Slots
 * - \b set_enabled(bool): Sets whether the service emits "activated" when the key-sequence is triggerd
 * - \b set_disabled(bool): Opposite of set_enabled(bool).
 * - \b enable(): Make the service active.
 * - \b disable(): Make the service not inactive.
 */
class signal_shortcut : public QObject,
                        public service::base
{
Q_OBJECT

public:

    struct signals
    {
        using bool_t = core::com::signal<void (bool)>;
        using void_t = core::com::signal<void ()>;

        static inline const core::com::signals::key_t IS_ENABLED = "is_enabled";
        static inline const core::com::signals::key_t ENABLED    = "enabled";
        static inline const core::com::signals::key_t DISABLED   = "disabled";
        static inline const core::com::signals::key_t ACTIVATED  = "activated";
    };

    struct slots
    {
        static inline const core::com::slots::key_t SET_ENABLED   = "set_enabled";
        static inline const core::com::slots::key_t SET_DISABLED  = "set_disabled";
        static inline const core::com::slots::key_t ENABLE        = "enable";
        static inline const core::com::slots::key_t DISABLE       = "disable";
        static inline const core::com::slots::key_t APPLY_ENABLED = "apply_enabled";
    };

    SIGHT_DECLARE_SERVICE(signal_shortcut, service::base);

    /// Constructor. Do nothing.
    signal_shortcut() noexcept;

    /// Destructor. Do nothing.
    ~signal_shortcut() noexcept override;

    /// Enables or disables the shortcut service.
    void set_enabled(bool _enabled);

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    /// This method configures the service
    void configuring() override;

    /**
     * @brief This method enables the eventFilter
     */
    void starting() override;

    /**
     * @brief This method deletes the eventFilter
     */
    void stopping() override;

    /**
     * @brief This method does nothing.
     */
    void updating() override;

    /// Connects the properties
    service::connections_t auto_connections() const override;

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

    /// Enabled property
    /// True: the "activated" signal is triggered when key sequence is detected.
    /// False: the key sequence is ignored.
    sight::data::property<sight::data::boolean> m_enabled {this, "enabled", true};
};

} // namespace sight::module::ui::qt::com
