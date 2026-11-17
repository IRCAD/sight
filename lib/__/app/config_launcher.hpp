/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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
            <config id="..." />
            <object name="object1" uid="..." />
            <object name="object2" uid="..." optional="true" />
            <object name="object3" value="..." />
            <param name="WID_PARENT" value="..." />
            <channel name="channel1" uid="..." />
        </service>
   @endcode
 *
 * @subsection Properties Properties:
 * - \b config [sight::data::string]: identifier of the configuration to launch.
 * @subsection In-Out In-Out:
 * - \b data [sight::data::object]: \b key specifies the name of the parameter in the target configuration and \b uid
 * identifies the objects whose uid are passed as value of the parameter. Alternatively, \b value gives a literal
 * value, and the object is then built and owned by this service. Its type is resolved from the declaration of the
 * matching object in the sub-configuration, when the service starts. The configuration may thus be chosen at runtime.
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

    /// Defers the creation of the objects declared with a literal value, until the sub-configuration is known
    std::optional<std::string> resolve_object_type(std::string_view _key, std::optional<std::size_t> _index)
    const override;

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

    /// Names of the objects passed to the configuration, with the hierarchical syntax
    data::ptr_vector<data::string, data::access::in> m_object_names {
        this, app::helper::config_launcher::OBJECT_NAME_GROUP
    };

    /// Objects passed to the configuration, with the hierarchical syntax. They may be deferred.
    data::ptr_vector<data::object, data::access::inout> m_object_uids {
        this, app::helper::config_launcher::OBJECT_UID_GROUP
    };

    /// Input data to pass to the configuration
    data::property<data::string> m_config_id {this, "config", {}};

    /// Identifier of the configuration to launch, with the hierarchical syntax
    data::ptr<data::string> m_nested_config_id {this, "config.id", {}};

    /// Returns the configuration identifier, whatever the syntax used to declare it
    [[nodiscard]] std::string config_id() const;
};

} // namespace sight::app
