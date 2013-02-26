/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ACTIVITIES_ACTION_SACTIVITYLAUNCHER_HPP_
#define _ACTIVITIES_ACTION_SACTIVITYLAUNCHER_HPP_

#include <fwTools/Failed.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwActivities/registry/Activities.hpp>

#include "activities/config.hpp"

namespace activities
{
namespace action
{

class ACTIVITIES_CLASS_API SActivityLauncher : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SActivityLauncher)(::fwGui::IActionSrv) ) ;

    /// Constructor. Do nothing.
    ACTIVITIES_API SActivityLauncher() throw() ;

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~SActivityLauncher() throw() ;

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
         </config>
     </service>
     @endverbatim
      */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

    typedef ::fwActivities::registry::ActivityAppConfig::ActivityAppConfigParamsType ParametersType;

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
     * this series.
     */
    void launchSeries(::fwMedData::Series::sptr series);

    /// Send message to launch new tab view
    void sendConfig( const ::fwActivities::registry::ActivityInfo & info );

    typedef ::fwActivities::registry::Activities::ActivitiesType ActivityInfoContainer;

    /// Show custom dialog box
    ::fwActivities::registry::ActivityInfo show( const ActivityInfoContainer & infos );

    ParametersType m_parameters;

    /**
     * @brief Filter mode : include or exclude activity configurations.
     * @note Allowed values : 'include' or 'exclude' 
     */
    std::string m_mode;

    /// Id-s of activity configurations to be enabled.
    std::vector< std::string > m_allowedKeys;

    /// Slot to call launchSeries method
    LaunchSeriesSlotType::sptr m_slotLaunchSeries;

    /// Signal emitted when activity is launched. Send a message containing the activity information.
    ActivityLaunchedSignalType::sptr m_sigActivityLaunched;
};

} //action
} // gui


#endif // _ACTIVITIES_ACTION_SACTIVITYLAUNCHER_HPP_


