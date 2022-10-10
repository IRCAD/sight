/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/ui/base/config.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

#include <service/helper/ConfigLauncher.hpp>

#include <ui/base/IAction.hpp>

namespace sight::module::ui::base
{

/**
 * @brief   This action starts/stops a template configuration.
 *
 *  The parameters of the template configuration <param name="..." /> are filled according to the
 *  <inout> and <parameter> tags. Using <inout> is especially useful to wait for deferred objects, but it is strongly
 * recommended to use it to pass any object.
 * Note that <in> is not supported. This would have no meaning, because we can't ensure the object won't be modified
 * in the configuration. <out> is also not supported because if we assume that the target configuration produces the
 * object, thus we would not get a valid id for the matching parameter.
 *
 * @section Signals Signals
 * - \b launched(): emitted when the configuration is launched
 *
 * @section Slots Slots
 * - \b stopConfig(): called to stop the running configuration
 *
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::service::SConfigController" >
            <appConfig id="IdOfConfig" />
            <parameter replace="channel" by="changeValueChannel"  />
            <parameter replace="service" by="serviceUid" />
        </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b parameter: \b replace specifies the name of the parameter in the target configuration and \b by the value of
 * this parameter. The variable GENERIC_UID can be used as unique identifier when the configuration is launched.
 */
class MODULE_UI_BASE_CLASS_API SConfigLauncher : public sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(SConfigLauncher, sight::ui::base::IAction);

    /// Constructor. Do nothing.
    MODULE_UI_BASE_API SConfigLauncher() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_BASE_API ~SConfigLauncher() noexcept override;

    /// Set the action service is activated/disable.
    MODULE_UI_BASE_API void setChecked(bool isChecked) override;

    // Launched signal key
    MODULE_UI_BASE_API static const core::com::Signals::SignalKeyType s_LAUNCHED_SIG;

protected:

    /**
     * @name Defines signal triggered when config is started
     * @{ */
    typedef core::com::Signal<void ()> LaunchedSignalType;
    LaunchedSignalType::sptr m_sigLaunched;
    /**  @} */

    ///This method launches the IAction::starting method.
    void starting() override;

    ///This method launches the IAction::stopping method.
    void stopping() override;

    /**
     * @brief Starts the view and initialize the operator.
     */
    void updating() override;

    /**
     * @brief Declare the configuration to associate with an operator type and a view config
     *
     * Call the IAction::configuring()
     *
     * Example of this service configuration
     * @code{.xml}
       <service impl="sight::module::ui::base::SConfigLauncher" type="ui::base::IAction">
           <config>
                <appConfig id="Visu2DID" >
                    <parameters>
                        <parameter replace="SERIES_SET" by="medicalData" />
                    </parameters>
                </appConfig>
            </config>
       </service>
        @endcode
     */
    void configuring() override;

    /**
     * @name Slots
     * @}
     */
    /// Slot: stop the config.
    void stopConfig();
    /**
     * @}
     */

    service::helper::ConfigLauncher::uptr m_configLauncher;
    std::string m_proxychannel; ///< Name of the channel used to connect stopConfig slot to the config frame closing.
};

} // namespace sight::module::ui::base
