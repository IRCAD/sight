/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "modules/ui/qt/config.hpp"

#include <activity/extension/activity.hpp>

#include <app/config_manager.hpp>

#include <core/tools/failed.hpp>

#include <data/activity.hpp>

#include <ui/__/activity_view.hpp>

namespace sight::module::ui::qt::activity
{

/**
 * @brief   This editor displays activities in a single view (when a new activity is launched, it replaces the previous
 * one).
 *
 * This service should receive signals containing Activity connected to the slot \b launch_activity.
 *
 * @section Signals Signals
 * - \b activity_launched( data::activity::sptr ): signal emitted when the activity is launched
 *
 * @section Slots Slots
 * - \b launch_activity( data::activity::sptr ): this slot allows to create a view for the given activity
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::ui::qt::activity::view" auto_connect="true" >
     <mainActivity id="Activity" />
     <parameters>
         <parameter replace="activitySet" by="medicalActivity"  />
         <parameter replace="ICON_PATH" by="sight::module::ui::icons/app.ico"  />
     </parameters>
   </service>
   @endcode
 * - \b mainActivity (optional): information about the main activity. The activity will be generated.
 *   This activity must not have requirement.
 *   - \b id : identifier of the activity
 * - \b parameters (optional) : additional parameters used to launch the activities
 *    - \b parameter: defines a parameter
 *        - \b replace: name of the parameter as defined in the config
 *        - \b by: defines the string that will replace the parameter name.
 *
 * @subsection Configuration Configuration
 * - \b border (optional, default="-1"): contents margins of the layout.
 */
class MODULE_UI_QT_CLASS_API view : public sight::ui::activity_view
{
public:

    SIGHT_DECLARE_SERVICE(view, sight::ui::activity_view);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API view();

    /// Destructor. Do nothing.

    MODULE_UI_QT_API ~view() override;

    /**
     * @name Signal API
     * @{
     */
    using activity_launched_signal_t = core::com::signal<void (data::activity::sptr)>;
/**
 * @}
 */

protected:

    /// Configures the service.
    void configuring() final;

    /// Install the container.
    void starting() override;

    /// Destroy the container.
    void stopping() override;

    /// Do nothing
    void updating() override;

private:

    /**
     * @brief Slot: Launch the given activity in the current view (stop and destroy the previous one).
     */
    void launch_activity(data::activity::sptr _activity) override;

    /// Helper to launch activity configuration
    sight::app::config_manager::sptr m_config_manager;

    /// WID used to register the activity container
    std::string m_wid;

    activity_launched_signal_t::sptr m_sig_activity_launched;

    /// Contents margins of the layout.
    int m_border {-1};
};

} // namespace sight::module::ui::qt::activity
