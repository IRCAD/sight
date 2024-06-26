/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <ui/__/parameter.hpp>

namespace sight::app
{

/**
 * @brief   This service starts/stops a template configuration.
 *
 * On the contrary to config_controller, the present service supports a list of configs.
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
        <service type="sight::app::multi_config_controller" >
            <appConfig key="..." default="config1">
                <config name="config1" id="..." />
                <config name="config2" id="..." />
                <config name="config3" id="..." />
                <parameter replace="model" by="model_key"  />
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
 * - \b data [sight::data::object]: \b key specifies the name of the parameter in the target configuration and \b uid
 * identifies the objects whose uid are passed as value of the parameter.
 *
 * @subsection Configuration Configuration:
 * - \b parameter: \b replace specifies the name of the parameter in the target configuration and \b by the value of
 * this parameter. The variable GENERIC_UID can be used as unique identifier when the configuration is launched.
 * - \b appConfig: this section allows to specify the configs which can be started. Each config is associated to a name
 *                 which has to be unique.
 *                 On update, the multi_config_controller will start a given config based on its name, and stop the
 *                 other ones.
 *   - \b default: a default name can be specified, to indicate which config should be started when no config have been
 *                 precises specifically.
 *   - \b key: the set_config slot is adapted to a classical SParameter signal. For a given multi_config_controller to
 *             be sure that the setConfig is actually addressed to it, the key attribute from the config will be *
 *             compared to the key attribute of the set_config slot.
 *             Only signals with the same key will be considered.
 *   - \b parameter: the set_config slot can also set random parameters that will be passed to the selected
 *                   config. If the the key from the set_config slot matches the key from the config, the parameters
 *                   will be replaced by the value from the set_config slot.
 *
 * @subsection slots slots:
 * - \b set_config( sight::ui::parameter_t, std::string): allows to change the currently started config. The first
 *                attribute should match the appConfig key, otherwise the service will ignore it. The second parameter *
 *                should be a supported config name.
 *                This slot only specifies which config will be started. Though to effectively start it a update is
 *                required.
 */
class SIGHT_APP_CLASS_API multi_config_controller : public service::controller
{
public:

    struct SIGHT_APP_CLASS_API slots final
    {
        using key_t = sight::core::com::slots::key_t;
        static inline const key_t SET_CONFIG = "set_config";
    };

    struct SIGHT_APP_CLASS_API signals final
    {
        using key_t           = sight::core::com::signals::key_t;
        using string_signal_t = sight::core::com::signal<void (std::string)>;

        static inline const key_t CONFIG_STARTED = "config_started";
    };

    SIGHT_DECLARE_SERVICE(multi_config_controller, service::controller);

    /// Constructor. Does nothing.
    SIGHT_APP_API multi_config_controller() noexcept;

    /// Destructor. Does nothing.
    SIGHT_APP_API ~multi_config_controller() noexcept override = default;

    /// SLOT: Allows changing the config to start
    SIGHT_APP_API void set_config(sight::ui::parameter_t _val, std::string _key);

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
    app::helper::config_launcher::uptr m_config_launcher;

    /// the config name for configuration set key evaluation
    std::string m_key {"multiConfigController"};

    /// Will store the extra parameters map filled by set_parameter slot
    sight::app::field_adaptor_t m_config_parameters_map;

    /// Input data to pass to the configuration
    data::ptr_vector<data::object, data::access::inout> m_data {this, app::helper::config_launcher::DATA_GROUP};
};

} // namespace sight::app
