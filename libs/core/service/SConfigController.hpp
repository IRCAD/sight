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

#include "service/config.hpp"
#include "service/helper/ConfigLauncher.hpp"
#include "service/IController.hpp"

namespace sight::service
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
        <service type="sight::service::SConfigController" >
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
 * - \b data [sight::data::Object]: \b key specifies the name of the parameter in the target configuration and \b uid
 * identifies the objects whose uid are passed as value of the parameter.
 * @subsection Configuration Configuration:
 * - \b parameter: \b replace specifies the name of the parameter in the target configuration and \b by the value of
 * this parameter. The variable GENERIC_UID can be used as unique identifier when the configuration is launched.
 */
class SERVICE_CLASS_API SConfigController : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SConfigController, service::IController);

    /// Constructor. Does nothing.
    SERVICE_API SConfigController() noexcept;

    /// Destructor. Does nothing.
    SERVICE_API ~SConfigController() noexcept override;

protected:

    /// Starts the config
    void starting() override;

    /// Stops the config
    void stopping() override;

    /// Does nothing
    void updating() override;

    /// Configures the service
    void configuring() override;

    /// Overrides
    void info(std::ostream& _sstream) override;

private:

    /// AppConfig manager
    service::helper::ConfigLauncher::uptr m_configLauncher;

    /// Input data to pass to the configuration
    data::ptr_vector<data::Object, data::Access::inout> m_data {this, service::helper::ConfigLauncher::s_DATA_GROUP};
};

} // namespace sight::service
