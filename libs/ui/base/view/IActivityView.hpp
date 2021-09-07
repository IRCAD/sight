/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/IGuiContainer.hpp"

#include <activity/extension/Activity.hpp>
#include <activity/IActivityLauncher.hpp>

#include <core/com/Slots.hpp>

#include <data/ActivitySeries.hpp>
#include <data/Series.hpp>

namespace sight::ui::base
{

namespace view
{

/**
 * @brief Defines the base class for services displaying activity view
 *
 * @section Slots Slots
 * - \b launchActivity( data::ActivitySeries::sptr ): This slot allows to create a view for the given activity
 *   series.
 * - \b launchActivitySeries( data::Series::sptr ): This slot allows to create a view for the given activity
 *   series.
 *
 *  * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::ui::qt::activity::SDynamicView" autoConnect="true" >
     <mainActivity id="SDBActivity" />
     <parameters>
         <parameter replace="SERIESDB" by="medicalData"  />
         <parameter replace="ICON_PATH" by="sight::module::ui::icons/app.ico"  />
     </parameters>
   </service>
   @endcode
 * - \b mainActivity (optional): information about the main activity (first tab). The activity series will be generated.
 *   This activity must not have requirement.
 *   - \b id : identifier of the activity
 * - \b parameters (optional) : additional parameters used to launch the activities
 *    - \b parameter: defines a parameter
 *        - \b replace: name of the parameter as defined in the AppConfig
 *        - \b by: defines the string that will replace the parameter name. It should be a simple string (ex.
 *          frontal) or define a camp path (ex. \@values.myImage). The root object of the sesh@ path if the
 *          composite contained in the ActivitySeries.
 */
class UI_BASE_CLASS_API IActivityView : public ui::base::IGuiContainer,
                                        public activity::IActivityLauncher
{
public:

    SIGHT_DECLARE_CLASS(IActivityView, ui::base::IGuiContainer);

    UI_BASE_API static const core::com::Slots::SlotKeyType s_LAUNCH_ACTIVITY_SLOT;
    UI_BASE_API static const core::com::Slots::SlotKeyType s_LAUNCH_ACTIVITY_SERIES_SLOT;

protected:

    typedef activity::extension::ActivityAppConfigParam ParameterType;
    typedef activity::extension::ActivityAppConfig::ActivityAppConfigParamsType ParametersType;
    typedef std::map<std::string, std::string> ReplaceMapType;

    /// Constructor. Do nothing.
    UI_BASE_API IActivityView();

    /// Destructor. Do nothing.
    UI_BASE_API ~IActivityView() override;

    /// Parses the configuration
    UI_BASE_API void configuring() override;

    /**
     * @brief Slot: Launch the given activity in a new tab.
     * @note The same activity series cannot be launch in two different tabs.
     */
    UI_BASE_API virtual void launchActivity(data::ActivitySeries::sptr activitySeries) = 0;

    /**
     * @brief Slot: Launch the given activity in a new tab.
     * @note The same activity series cannot be launch in two different tabs.
     */
    UI_BASE_API virtual void launchActivitySeries(data::Series::sptr series);

    /**
     * @brief Check if the activity is valid by calling the activity validator.
     * @return Return true if the given activity is valid
     */
    UI_BASE_API virtual bool validateActivity(data::ActivitySeries::sptr activitySeries) const;

    /// Create the activity series given in 'mainActivity' configuration
    UI_BASE_API data::ActivitySeries::sptr createMainActivity() const override;
};

} // namespace view

} // namespace sight::ui::base
