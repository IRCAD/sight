/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
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

#include <sight/ui/__/config.hpp>

#include "ui/__/service.hpp"

#include <activity/extension/activity.hpp>
#include <activity/launcher.hpp>

#include <core/com/slots.hpp>

#include <data/activity.hpp>
#include <data/string.hpp>

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
     <object name="SERIES_SET" uid="${medical_data}" />
     <object name="MODEL" uid="${model}" optional="true" />
     <object name="ACTIVITY_NAME" value="my activity" />
     <param name="ICON_PATH" value="sight::module::ui::icons/app.ico" />
     <channel name="..." uid="..." />
   </service>
   @endcode
 *
 * The former syntax is still supported:
 *
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
 * - \b data (optional): object passed to the activity. \b name is the name of the parameter in the activity
 *   configuration, and \b uid the object to pass, or \b value a literal value whose type is resolved from the
 *   activity configuration. \b optional allows the service to start before a deferred object is available.
 * - \b parameters (optional) : additional parameters used to launch the activities
 *    - \b parameter: defines a parameter
 *        - \b replace: name of the parameter as defined in the config
 *        - \b by: defines the string that will replace the parameter name.
 */
class SIGHT_UI_CLASS_API activity_view : public ui::service,
                                         public activity::launcher
{
public:

    SIGHT_DECLARE_CLASS(activity_view, ui::service);

    struct slots
    {
        static inline const slot_key_t LAUNCH_ACTIVITY = "launch_activity";
    };

    /// Destructor. Do nothing.
    SIGHT_UI_API ~activity_view() override = default;

protected:

    using parameter_t  = activity::extension::activity_config_param;
    using parameters_t = activity::extension::activity_config_params_type;

    /// Constructor. Do nothing.
    SIGHT_UI_API activity_view();

    /// Parses the configuration
    SIGHT_UI_API void configuring() override;

    /**
     * @brief Defers the creation of the objects declared with a literal value.
     *
     * The type of such an object is declared by the activity configuration, which is only known when the activity is
     * launched. The objects are thus built at that moment, see service::materialize_value_parameters().
     */
    SIGHT_UI_API std::optional<std::string> resolve_object_type(
        std::string_view _key,
        std::optional<std::size_t> _index
    ) const override;

    /**
     * @brief Slot: Launch the given activity in a new tab.
     * @note The same activity cannot be launch in two different tabs.
     */
    SIGHT_UI_API virtual void launch_activity(data::activity::sptr _activity) = 0;

    /**
     * @brief Check if the activity is valid by calling the activity validator.
     * @return Return true if the given activity is valid
     */
    SIGHT_UI_API virtual bool validate_activity(data::activity::sptr _activity) const;

    /// Create the activity given in 'mainActivity' configuration
    SIGHT_UI_API data::activity::sptr create_main_activity() const override;

private:

    /// Input data to pass to the configuration
    data::ptr_vector<data::object, data::access::inout> m_data {this, "data"};

    /// Names of the objects passed to the configuration, with the hierarchical syntax
    data::ptr_vector<data::string, data::access::in> m_data_names {this, "data.name"};

    /// Objects passed to the configuration, with the hierarchical syntax. They may be deferred.
    data::ptr_vector<data::object, data::access::inout> m_data_uids {this, "data.uid"};
};

} // namespace sight::ui
