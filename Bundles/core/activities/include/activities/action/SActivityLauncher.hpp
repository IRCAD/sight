/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ACTIVITIES_ACTION_SACTIVITYLAUNCHER_HPP__
#define __ACTIVITIES_ACTION_SACTIVITYLAUNCHER_HPP__

#include <fwActivities/registry/Activities.hpp>

#include <fwData/Vector.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include "activities/config.hpp"

namespace activities
{
namespace action
{

class ACTIVITIES_CLASS_API SActivityLauncher : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SActivityLauncher)(::fwGui::IActionSrv) );

    /// Constructor. Do nothing.
    ACTIVITIES_API SActivityLauncher() throw();

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~SActivityLauncher() throw();

    ACTIVITIES_API static const ::fwCom::Slots::SlotKeyType s_LAUNCH_SERIES_SLOT;
    typedef ::fwCom::Slot< void (SPTR( ::fwMedData::Series )) > LaunchSeriesSlotType;

    typedef ::fwCom::Signal< void ( CSPTR(::fwServices::ObjectMsg) ) > ActivityLaunchedSignalType;

    /// Key in m_signals map of signal m_sigActivityLaunched
    ACTIVITIES_API static const ::fwCom::Signals::SignalKeyType s_ACTIVITY_LAUNCHED_SIG;

protected:

    ///This method launches the IAction::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IAction::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief This method is used to update services on notification. ( overrides ).
     * Enable the action according to the available activities.
     */
    virtual void receiving( CSPTR(::fwServices::ObjectMsg) msg ) throw(::fwTools::Failed);

    /**
     * @brief Show activity selector.
     */
    virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief Initialize the action.
     * @see fwGui::IActionSrv::initialize()
     *
     * @verbatim
       <service uid="action_newActivity" type="::fwGui::IActionSrv" impl="::activities::action::SActivityLauncher" autoConnect="yes" >
         <config>
             <!-- SActivityLauncher mode : immediate or message(default)
             Immediate mode starts and stop immediatly the activity's config -->
             <mode>immediate</mode>
             <parameters>
                 <parameter replace="SERIESDB" by="medicalData" />
                 <parameter replace="IMAGE" by="@values.ImageSeries.image" />
             </parameters>

             <!-- Filter mode 'include' allows all given activity id-s.
                  Filter mode 'exclude' allows all activity id-s excepted given ones. -->
             <filter>
                <mode>include</mode>
                <id>2DVisualizationActivity</id>
                <id>3DVisualizationActivity</id>
                <id>VolumeRenderingActivity</id>
            </filter>

            <!-- Definition of quick association between series type and an activity id.
                 This mapping is used by launchSeries slot (see the function to have few details ) -->
            <quickLaunch>
                <association type="::fwMedData::ImageSeries" id="2DVisualizationActivity" />
                <association type="::fwMedData::ModelSeries" id="3DVisualizationActivity" />
            </quickLaunch>

         </config>
       </service>
       @endverbatim
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );

    typedef ::fwActivities::registry::ActivityAppConfig::ActivityAppConfigParamsType ParametersType;

    typedef std::vector< std::string > KeysType;

    typedef std::map< std::string, std::string > QuickLaunchType;

    /**
     * @brief Updates action state (enable if activities are available for current selection).
     */
    virtual void updateState();

    /**
     * @brief Interpret configuration parameters.
     */
    ParametersType translateParameters( const ParametersType& parameters);

private:

    /**
     * @brief Launches activity series if only ActivitySeries are selected.
     * @return Returns true if only ActivitySeries are selected.
     */
    bool launchAS(::fwData::Vector::sptr &selection);

    /**
     * @brief Slots to launch the given series.
     * @param series the activity is launched on this series.
     *
     * If series is an ActivitySeries, it is launched, otherwise it launches the first available activity for
     * this series or used m_quickLaunch information if a default association is defined for this series type.
     */
    void launchSeries(::fwMedData::Series::sptr series);

    /**
     * @brief Send message to launch new tab view
     * If given activity info contains an ::fwActivities::IValidator, first checks if activity is valid according to
     * validator, then build activity with activity builder.
     *
     * @param info activity information
     */
    void sendConfig( const ::fwActivities::registry::ActivityInfo & info );

    /**
     * @brief Builds and launch activity with the input data given in selection.
     *
     * @param info activity information
     * @param selection input data to launch the activity
     */
    void buildActivity(const ::fwActivities::registry::ActivityInfo & info, const ::fwData::Vector::sptr& selection);

    typedef ::fwActivities::registry::Activities::ActivitiesType ActivityInfoContainer;

    /// Show custom dialog box
    ::fwActivities::registry::ActivityInfo show( const ActivityInfoContainer & infos );

    /// Returns enabled activity infos according to activity filter.
    ActivityInfoContainer getEnabledActivities(const ActivityInfoContainer& infos);

    ParametersType m_parameters;

    /**
     * @brief Filter mode : include or exclude activity configurations.
     * @note Allowed values : 'include' or 'exclude'
     */
    std::string m_filterMode;

    /// Id-s of activity configurations to be enabled or disabled, according to filter mode.
    KeysType m_keys;

    /// Slot to call launchSeries method
    LaunchSeriesSlotType::sptr m_slotLaunchSeries;

    /// Signal emitted when activity is launched. Send a message containing the activity information.
    ActivityLaunchedSignalType::sptr m_sigActivityLaunched;

    /// SActivityLauncher's mode (message or immediate)
    std::string m_mode;

    /// Defines quick association between series type (a classname) and an activity id used by launchSeries method
    QuickLaunchType m_quickLaunch;
};

} //action
} // gui


#endif // __ACTIVITIES_ACTION_SACTIVITYLAUNCHER_HPP__


