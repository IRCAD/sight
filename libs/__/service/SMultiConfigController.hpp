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

#include "service/config.hpp"
#include "service/controller.hpp"
#include "service/helper/ConfigLauncher.hpp"

#include <ui/__/parameter.hpp>

namespace sight::service
{

/**
 * @brief   This service starts/stops a template configuration.
 *
 * On the contrary to SConfigController, the present service supports a list of configs.
 * Each of them is associated to a name, which has to be unique.
 * The currently started config can be set through a setConfig slot or through the xml configuration.
 * The config is started on the setConfig call or the update call.
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
        <service type="sight::service::SMultiConfigController" >
            <appConfig key="..." default="config1">
                <config name="config1" id="..." />
                <config name="config2" id="..." />
                <config name="config3" id="..." />
            </appConfig>
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
 *
 * @subsection Configuration Configuration:
 * - \b parameter: \b replace specifies the name of the parameter in the target configuration and \b by the value of
 * this parameter. The variable GENERIC_UID can be used as unique identifier when the configuration is launched.
 * - \b appConfig: this section allows to specify the configs which can be started. Each config is associated to a name
 * which has to be unique.
 * On update, the SMultiConfigController will start a given config based on its name, and stop the other ones.
 * - \b default: a default name can be specified, to indicate which config should be started when no config have been
 * precises specifically.
 * - \b key: the setConfig slot is adapted to a classical SParameter signal. For a given SMultiConfigController to be
 * sure that the setConfig is actually addressed to it, the key attribute from the config will be compared to the key
 * attribute of the setConfig slot.
 * Only signals with the same key will be considered.
 *
 *
 * @subsection slots slots:
 * - \b setConfig( sight::ui::parameter_t, std::string): allows to change the currently started config. The first
 * attribute should
 * match the
 * appConfig key, otherwise the service will ignore it. The second parameter should be a supported config name.
 * This slot only specifies which config will be started. Though to effectively start it a update is required.
 */
class SERVICE_CLASS_API SMultiConfigController : public service::controller
{
public:

    struct slots
    {
        using key_t = sight::core::com::slots::key_t;
        static inline const key_t SET_CONFIG = "setConfig";
    };

    SIGHT_DECLARE_SERVICE(SMultiConfigController, service::controller);

    /// Constructor. Does nothing.
    SERVICE_API SMultiConfigController() noexcept;

    /// Destructor. Does nothing.
    SERVICE_API ~SMultiConfigController() noexcept override = default;

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

    /// Allows changing the config to start
    void setConfig(sight::ui::parameter_t val, std::string key);

private:

    /// AppConfig manager
    service::helper::ConfigLauncher::uptr m_configLauncher;

    // the config name for configuration set key evaluation
    std::string m_key {"multiConfigController"};

    /// Input data to pass to the configuration
    data::ptr_vector<data::Object, data::Access::inout> m_data {this, service::helper::ConfigLauncher::s_DATA_GROUP};
};

} // namespace sight::service
