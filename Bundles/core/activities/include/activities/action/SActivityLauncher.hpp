/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "activities/config.hpp"

#include <fwActivities/registry/Activities.hpp>
#include <fwActivities/registry/ActivityMsg.hpp>

#include <fwData/Vector.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

namespace activities
{
namespace action
{

/**
 * @brief This action launchs an activity according to the selected data
 *
 * This action works on a ::fwData::Vector. It proposes all the available activity according to the selected data and
 * the given configuration. And then, send a signal with all the activity information.
 *
 * This action should be followed by the service '::guiQt::editor::DynamicView' : this service listens the action
 * signals and launchs the activity in a new tab.
 *
 * @section Slots Slots
 * - \b launchSeries(::fwMedData::Series::sptr) : This slot allows to launch the series. If series is an
 *      ActivitySeries, it is launched, otherwise it launches the first available activity for this series or used
 *      m_quickLaunch information if a default association is defined for this series type. It sends the
 *      signal 'activityLaunched' with all the activity information.
 * - \b launchActivitySeries(::fwMedData::ActivitySeries::sptr) : This slot allows to launch the given activity series.
 *   It sends the signal 'activityLaunched' with all the activity information.
 *
 * - \b updateState() : Updates action state (enable if activities are available for current selection).
 *
 * @section Signal Signal
 * - \b activityLaunched(::fwActivities::registry::ActivityMsg) : This signal is emitted when the activity is created,
 *      it contains the activity information. It should be connected to the slot 'createTab' of the service
 *      '::guiQt::editor::SDynamicView'.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="action_newActivity" type="::activities::action::SActivityLauncher" autoConnect="yes" >
        <in key="series" uid="..." />
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
        @endcode
 * @subsection Input Input
 * - \b series [::fwData::Vector]: vector containg series inherited from ::fwMedData::Series
 * @subsection Configuration Configuration
 * - \b mode (optional): there are two mode: "message" and "immediate"
 *    - \b message (used by défaut): the action send a signal containing the information needed to launch the
 *      choosen activity. The service '::guiQt::editor::DynamicView' allows to launch the activity in a new tab. For
 *      that, it must listen the action signal.
 *    - \b immediate: the activity is automatically started et stopped by this action. It is used to run a process
 *      without creating a new tab, for example, to save the selected data.
 * - \b parameters (optional): list of the parameters used to launch the activity, it is the parameters for the
 *   AppConfig associated to the activity.
 *    - \b parameter: defines a parameter
 *        - \b replace: name of the parameter as defined in the AppConfig
 *        - \b by: defines the string that will replace the parameter name. It should be a simple string (ex.
 *          frontal) or define a sesh@ path (ex. \@values.myImage). The root object of the sesh@ path if the
 *          composite contained in the ActivitySeries.
 * - \b filter (optional): it allows to filter the activity that can be proposed.
 *    - \b mode: 'include' or 'exclude'. Defines if the activity in the following list are proposed (include) or not
 *      (exclude).
 *    - \b id: id of the activity
 * - \b quickLaunch (optional): defines the activity that will be launched on a double-click on a series. The
 *   launched activity depends of the series type (ImageSeries, ModelSeries, ...).
 *    - \b association: allows to associate an activity to launch with a type of series
 *       - \b type: type of series (::fwMedData::ImageSeries, ::fwMedData::ModelSeries, ....)
 *       - \b id: identifier of the activity.
 *
 *
 * @note A sesh@ path is a path used to browse an object (and sub-object) using the introspection (see fwDataCamp).
 *       The path begins with a '@' or a '!'.
 *          - '@' : the returned string is the fwID of the sub-object defined by the path.
 *          - '!' : the returned string is the value of the sub-object, it works only on String, Integer, Float and
 *            Boolean object.
 */
class ACTIVITIES_CLASS_API SActivityLauncher : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SActivityLauncher)(::fwGui::IActionSrv) );

    /// Constructor. Do nothing.
    ACTIVITIES_API SActivityLauncher() noexcept;

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~SActivityLauncher() noexcept;

    /**
     * @name Slot API
     * @{
     */
    ACTIVITIES_API static const ::fwCom::Slots::SlotKeyType s_LAUNCH_SERIES_SLOT;
    ACTIVITIES_API static const ::fwCom::Slots::SlotKeyType s_LAUNCH_ACTIVITY_SERIES_SLOT;
    ACTIVITIES_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_STATE_SLOT;
    /// @}

    /**
     * @name Signal API
     * @{
     */
    typedef ::fwCom::Signal< void ( ::fwActivities::registry::ActivityMsg ) > ActivityLaunchedSignalType;

    /// Key in m_signals map of signal m_sigActivityLaunched
    ACTIVITIES_API static const ::fwCom::Signals::SignalKeyType s_ACTIVITY_LAUNCHED_SIG;
    /// @}

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Vector::s_ADDED_OBJECTS_SIG to this::s_UPDATE_STATE_SLOT
     * Connect Vector::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_STATE_SLOT
     */
    ACTIVITIES_API virtual KeyConnectionsType getObjSrvConnections() const override;

protected:

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Vector::s_ADDED_OBJECTS_SIG to this::s_UPDATE_STATE_SLOT
     * Connect Vector::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_STATE_SLOT
     */
    ACTIVITIES_API virtual KeyConnectionsMap getAutoConnections() const override;

    ///This method launches the IAction::starting method.
    virtual void starting() override;

    ///This method launches the IAction::stopping method.
    virtual void stopping() override;

    /**
     * @brief Show activity selector.
     */
    virtual void updating() override;

    /**
     * @brief Initialize the action.
     * @see fwGui::IActionSrv::initialize()
     */
    virtual void configuring() override;

    typedef ::fwActivities::registry::ActivityAppConfig::ActivityAppConfigParamsType ParametersType;

    typedef std::vector< std::string > KeysType;

    typedef std::map< std::string, std::string > QuickLaunchType;

    //// SLOT: Updates action state (enable if activities are available for current selection).
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
    bool launchAS(const ::fwData::Vector::csptr& selection);

    /**
     * @brief Slots to launch the given series.
     * @param series the activity is launched on this series.
     *
     * If series is an ActivitySeries, it is launched, otherwise it launches the first available activity for
     * this series or used m_quickLaunch information if a default association is defined for this series type.
     */
    void launchSeries(::fwMedData::Series::sptr series);

    /**
     * @brief Slots to launch the given activity series.
     * @param series the activity series to launch.
     */
    void launchActivitySeries(::fwMedData::ActivitySeries::sptr series);

    /**
     * @brief Send message to launch new tab view
     * If given activity info contains an ::fwActivities::IValidator, first checks if activity is valid according to
     * validator, then build activity with activity builder.
     *
     * @param info activity information
     */
    void sendConfig( const ::fwActivities::registry::ActivityInfo& info );

    /**
     * @brief Builds and launch activity with the input data given in selection.
     *
     * @param info activity information
     * @param selection input data to launch the activity
     */
    void buildActivity(const ::fwActivities::registry::ActivityInfo& info, const ::fwData::Vector::csptr& selection);

    typedef ::fwActivities::registry::Activities::ActivitiesType ActivityInfoContainer;

    /// Show custom dialog box
    ::fwActivities::registry::ActivityInfo show( const ActivityInfoContainer& infos );

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

    /// Signal emitted when activity is launched. Send a message containing the activity information.
    ActivityLaunchedSignalType::sptr m_sigActivityLaunched;

    /// SActivityLauncher's mode (message or immediate)
    std::string m_mode;

    /// Defines quick association between series type (a classname) and an activity id used by launchSeries method
    QuickLaunchType m_quickLaunch;
};

} //action
} // gui
