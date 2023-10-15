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

#include "app/config.hpp"
#include "app/helper/config_launcher.hpp"

#include "service/controller.hpp"

namespace sight::app
{

/**
 * @brief   This service starts/stops a template configuration.
 *
 *  The parameters of the template configuration <param name="..." /> are filled according to the
 *  <inout> and <parameter> tags. Using <inout> is especially useful to wait for deferred objects, but it is strongly
 * recommended to use it to pass any object.
 * Note that <in> is not supported. This would have no meaning, because we can't ensure the object won't be modified
 * in the configuration. <out> is also not supported because if we assume that the target configuration produces the
 * object, thus we would not get a valid id for the matching parameter.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::app::config_controller" >
            <appConfig id="configId" />
            <inout group="data">
                <key name="object1" uid="..." />
                <key name="object2" uid="..." />
                ...
            </inout>
            <parameter replace="channel" by="changeValueChannel"  />
            <parameter replace="service" by="serviceUid" />
        </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b data [sight::data::object]: \b key specifies the name of the parameter in the target configuration and \b uid
 * identifies the objects whose uid are passed as value of the parameter.
 * @subsection Configuration Configuration:
 * - \b parameter: \b replace specifies the name of the parameter in the target configuration and \b by the value of
 * this parameter. The variable GENERIC_UID can be used as unique identifier when the configuration is launched.
 */
class APP_CLASS_API config_controller : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(config_controller, service::controller);

    /// Constructor. Does nothing.
    APP_API config_controller() noexcept;

    /// Destructor. Does nothing.
    APP_API ~config_controller() noexcept override = default;

protected:

    /// Configures the service
    void configuring(const config_t& _config) override;

    /// Starts the config
    void starting() override;

    /// Stops the config
    void stopping() override;

    /// Does nothing
    void updating() override;

    /// Overrides
    void info(std::ostream& _sstream) override;

private:

    /// config manager
    app::helper::config_launcher::uptr m_configLauncher;

    /// Input data to pass to the configuration
    data::ptr_vector<data::object, data::Access::inout> m_data {this, app::helper::config_launcher::s_DATA_GROUP};
};

} // namespace sight::app
