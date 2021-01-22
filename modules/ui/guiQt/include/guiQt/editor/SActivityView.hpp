/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "guiQt/config.hpp"

#include <core/tools/Failed.hpp>

#include <fwActivities/registry/Activities.hpp>

#include <fwGui/view/IActivityView.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwServices/IAppConfigManager.hpp>

namespace guiQt
{
namespace editor
{

/**
 * @brief   This editor displays activities in a single view (when a new activity is launched, it replaces the previous
 * one).
 *
 * This service should receive signals containing ActivitySeries connected to the slot \b launchActivity.
 *
 * @section Signals Signals
 * - \b activityLaunched( ::fwMedData::ActivitySeries::sptr ): signal emitted when the activity is launched
 *
 * @section Slots Slots
 * - \b launchActivity( ::fwMedData::ActivitySeries::sptr ): this slot allows to create a view for the given activity
 *   series.
 * - \b launchActivitySeries( ::fwMedData::Series::sptr ): this slot allows to create a view for the given activity
 *   series.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::guiQt::editor::SActivityView" autoConnect="yes" >
     <mainActivity id="SDBActivity" />
     <parameters>
         <parameter replace="SERIESDB" by="medicalData"  />
         <parameter replace="ICON_PATH" by="media-0.1/icons/app.ico"  />
     </parameters>
   </service>
   @endcode
 * - \b mainActivity (optional): information about the main activity. The activity series will be generated.
 *   This activity must not have requirement.
 *   - \b id : identifier of the activity
 * - \b parameters (optional) : additional parameters used to launch the activities
 *    - \b parameter: defines a parameter
 *        - \b replace: name of the parameter as defined in the AppConfig
 *        - \b by: defines the string that will replace the parameter name. It should be a simple string (ex.
 *          frontal) or define a camp path (ex. \@values.myImage). The root object of the sesh@ path if the
 *          composite contained in the ActivitySeries.
 *
 * @subsection Configuration Configuration
 * - \b border (optional, default="-1"): contents margins of the layout.
 */
class GUIQT_CLASS_API SActivityView : public ::fwGui::view::IActivityView
{

public:

    fwCoreServiceMacro(SActivityView, ::fwGui::view::IActivityView)

    /// Constructor. Do nothing.
    GUIQT_API SActivityView();

    /// Destructor. Do nothing.

    GUIQT_API virtual ~SActivityView();

    /**
     * @name Signal API
     * @{
     */
    typedef core::com::Signal< void (::fwMedData::ActivitySeries::sptr ) > ActivityLaunchedSignalType;
    /**
     * @}
     */

protected:

    /// Configures the service.
    void configuring() final;

    /// Install the container.
    virtual void starting() override;

    /// Destroy the container.
    virtual void stopping() override;

    /// Do nothing
    virtual void updating() override;

private:

    /**
     * @brief Slot: Launch the given activity in the current view (stop and destroy the previous one).
     */
    void launchActivity(::fwMedData::ActivitySeries::sptr activitySeries) override;

    /// Helper to launch activity configuration
    ::fwServices::IAppConfigManager::sptr m_configManager;

    /// WID used to register the activity container
    std::string m_wid;

    ActivityLaunchedSignalType::sptr m_sigActivityLaunched;

    /// Contents margins of the layout.
    int m_border {-1};
};

} //namespace editor
} // guiQt
