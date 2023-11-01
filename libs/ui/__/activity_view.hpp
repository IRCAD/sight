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

#include "ui/__/config.hpp"
#include "ui/__/service.hpp"

#include <activity/extension/activity.hpp>
#include <activity/launcher.hpp>

#include <core/com/slots.hpp>

#include <data/activity.hpp>

namespace sight::ui
{

/**
 * @brief Defines the base class for services displaying activity view
 *
 * @section Slots Slots
 * - \b launch_activity( data::activity::sptr ): This slot allows to create a view for the given activity.
 *
 *  * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::ui::qt::activity::dynamic_view" auto_connect="true" >
     <mainActivity id="SDBActivity" />
     <parameters>
         <parameter replace="SERIES_SET" by="medicalData"  />
         <parameter replace="ICON_PATH" by="sight::module::ui::icons/app.ico"  />
     </parameters>
   </service>
   @endcode
 * - \b mainActivity (optional): information about the main activity (first tab). The activity will be generated.
 *   This activity must not have requirement.
 *   - \b id : identifier of the activity
 * - \b parameters (optional) : additional parameters used to launch the activities
 *    - \b parameter: defines a parameter
 *        - \b replace: name of the parameter as defined in the config
 *        - \b by: defines the string that will replace the parameter name.
 */
class UI_CLASS_API activity_view : public ui::service,
                                   public activity::launcher
{
public:

    SIGHT_DECLARE_CLASS(activity_view, ui::service);

    UI_API static const core::com::slots::key_t LAUNCH_ACTIVITY_SLOT;

protected:

    using parameter_t  = activity::extension::activity_config_param;
    using parameters_t = activity::extension::activity_config_params_type;

    /// Constructor. Do nothing.
    UI_API activity_view();

    /// Destructor. Do nothing.
    UI_API ~activity_view() override = default;

    /// Parses the configuration
    UI_API void configuring() override;

    /**
     * @brief Slot: Launch the given activity in a new tab.
     * @note The same activity cannot be launch in two different tabs.
     */
    UI_API virtual void launch_activity(data::activity::sptr _activity) = 0;

    /**
     * @brief Check if the activity is valid by calling the activity validator.
     * @return Return true if the given activity is valid
     */
    UI_API virtual bool validate_activity(data::activity::sptr _activity) const;

    /// Create the activity given in 'mainActivity' configuration
    UI_API data::activity::sptr create_main_activity() const override;

    /// Input data to pass to the configuration
    data::ptr_vector<data::object, data::access::inout> m_data {this, "data"};
};

} // namespace sight::ui
