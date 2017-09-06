/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_EDITOR_SACTIVITYVIEW_HPP__
#define __GUIQT_EDITOR_SACTIVITYVIEW_HPP__

#include "guiQt/config.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwServices/IAppConfigManager.hpp>

#include <fwTools/Failed.hpp>

#include <gui/view/IActivityView.hpp>

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
 * - \b launchActivity( ::fwMedData::ActivitySeries::sptr ): This slot allows to create a view for the given activity
 *   series.
 * - \b launchActivitySeries( ::fwMedData::Series::sptr ): This slot allows to create a view for the given activity
 *   series.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::gui::view::IView" impl="::guiQt::editor::SActivityView" autoConnect="yes" >
     <mainActivity id="SDBActivity" />
     <parameters>
         <parameter replace="SERIESDB" by="medicalData"  />
         <parameter replace="ICON_PATH" by="@BUNDLE_PREFIX@/media_0-1/icons/app.ico"  />
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
 */
class GUIQT_CLASS_API SActivityView : public ::gui::view::IActivityView
{

public:

    fwCoreServiceClassDefinitionsMacro( (SActivityView)(::gui::view::IActivityView) );

    /// Constructor. Do nothing.
    GUIQT_API SActivityView();

    /// Destructor. Do nothing.

    GUIQT_API virtual ~SActivityView();

    /**
     * @name Signal API
     * @{
     */
    typedef ::fwCom::Signal< void (::fwMedData::ActivitySeries::sptr ) > ActivityLaunchedSignalType;
    /**
     * @}
     */

protected:

    /// Install the container.
    virtual void starting();

    /// Destroy the container.
    virtual void stopping();

    /// Do nothing
    virtual void updating();

private:

    /**
     * @brief Slot: Launch the given activity in the current view (stop and destroy the previous one).
     */
    void launchActivity(::fwMedData::ActivitySeries::sptr activitySeries);

    /// Helper to launch activity configuration
    ::fwServices::IAppConfigManager::sptr m_configManager;

    /// WID used to register the activity container
    std::string m_wid;

    ActivityLaunchedSignalType::sptr m_sigActivityLaunched;
};

} //namespace editor
} // guiQt

#endif /*__GUIQT_EDITOR_SACTIVITYVIEW_HPP__*/
