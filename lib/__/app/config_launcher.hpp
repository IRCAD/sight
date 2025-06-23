/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <sight/app/config.hpp>

#include "app/helper/config_launcher.hpp"

#include "service/controller.hpp"

#include <data/string.hpp>

namespace sight::app
{

/**
 * @brief   This service starts/stops a template configuration.
 *
 * The parameters of the template configuration <param name="..." /> are filled according to the
 * <inout> and <parameter> tags. Using <inout> is especially useful to wait for deferred objects, but it is strongly
 * recommended to use it to pass any object.
 * Note that <in> is not supported. This would have no meaning, because we can't ensure the object won't be modified
 * in the configuration. <out> is also not supported because if we assume that the target configuration produces the
 * object, thus we would not get a valid id for the matching parameter.
 *
 * @section Signals Signals
 * - \b launched(): Sent when the configuration has been launched.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::app::config_launcher" >
            <properties config="..." />
            <inout group="object">
                <key name="object1" uid="..." />
                <key name="object2" uid="..." />
                ...
            </inout>
            <parameter replace="channel" by="changeValueChannel" />
            <parameter replace="service" by="serviceUid" />
        </service>
   @endcode
 * @subsection Properties Properties:
 * - \b config [sight::data::string]: identifier of the configuration to launch.
 * @subsection In-Out In-Out:
 * - \b data [sight::data::object]: \b key specifies the name of the parameter in the target configuration and \b uid
 * identifies the objects whose uid are passed as value of the parameter.
 * @subsection Configuration Configuration:
 * - \b parameter: \b replace specifies the name of the parameter in the target configuration and \b by the value of
 * this parameter.
 * The parameter CLOSE_CONFIG can be used inside the configuration to stop it.
 */
class SIGHT_APP_CLASS_API config_launcher : public service::controller
{
public:

    struct signals
    {
        using launched_t = core::com::signal<void ()>;
        static const inline signal_key_t LAUNCHED = "launched";
    };

    SIGHT_DECLARE_SERVICE(config_launcher, service::controller);

    /// Constructor. Does nothing.
    SIGHT_APP_API config_launcher() noexcept;

    /// Destructor. Does nothing.
    SIGHT_APP_API ~config_launcher() noexcept override = default;

protected:

    //// Calls update when the configuration id changes
    service::connections_t auto_connections() const override;

    /// Configures the service
    void configuring(const config_t& _config) override;

    /// Starts the config
    void starting() override;

    /// Stops the config
    void stopping() override;

    /// Does nothing
    void updating() override;

private:

    void start_config();

    /// Slot: stop the config.
    void stop_config();

    /// config manager
    app::helper::config_launcher::uptr m_config_launcher;

    /// Name of the channel used to connect stopConfig slot to the config frame closing.
    std::string m_proxy_channel;

    /// Input data to pass to the configuration - DEPRECATED
    data::ptr_vector<data::object, data::access::inout> m_data {this, app::helper::config_launcher::DATA_GROUP};

    /// Input data to pass to the configuration
    data::ptr_vector<data::object, data::access::inout> m_object {this, app::helper::config_launcher::OBJECT_GROUP};

    /// Input data to pass to the configuration
    data::property<data::string> m_config_id {this, "config", {}};
};

} // namespace sight::app
