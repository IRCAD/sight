/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/ui/qt/config.hpp"

#include <activity/ActivityMsg.hpp>
#include <activity/extension/Activity.hpp>

#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/EConfigurationElement.hpp>

#include <data/Vector.hpp>

#include <ui/base/IAction.hpp>

namespace sight::module::ui::qt
{
namespace activity
{

/**
 * @brief This action launchs an activity according to the selected data
 *
 * This action works on a data::Vector. It proposes all the available activity according to the selected data and
 * the given configuration. And then, send a signal with all the activity information.
 *
 * This action should be followed by the service '::module::ui::qt::editor::SDynamicView' : this service listens the
 * action
 * signals and launchs the activity in a new tab.
 *
 * @section Slots Slots
 * - \b launchSeries(data::Series::sptr) : This slot allows to launch the series. If series is an
 *      ActivitySeries, it is launched, otherwise it launches the first available activity for this series or used
 *      m_quickLaunch information if a default association is defined for this series type. It sends the
 *      signal 'activityLaunched' with all the activity information.
 * - \b launchActivitySeries(data::ActivitySeries::sptr) : This slot allows to launch the given activity series.
 *   It sends the signal 'activityLaunched' with all the activity information.
 *
 * - \b updateState() : Updates action state (enable if activities are available for current selection).
 *
 * @section Signal Signal
 * - \b activityLaunched(activity::ActivityMsg) : This signal is emitted when the activity is created,
 *      it contains the activity information. It should be connected to the slot 'createTab' of the service
 *      '::module::ui::qt::editor::SDynamicView'.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="action_newActivity" type="sight::module::ui::qt::activity::SLauncher" autoConnect="yes" >
        <in key="series" uid="..." />
        <config>
            <!-- SLauncher mode : immediate or message(default)
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
                <association type="data::ImageSeries" id="2DVisualizationActivity" />
                <association type="data::ModelSeries" id="3DVisualizationActivity" />
            </quickLaunch>
        </config>
    </service>
        @endcode
 * @subsection Input Input
 * - \b series [sight::data::Vector]: vector containg series inherited from data::Series
 * @subsection Configuration Configuration
 * - \b mode (optional): there are two mode: "message" and "immediate"
 *    - \b message (used by default): the action send a signal containing the information needed to launch the
 *      chosen activity. The service '::module::ui::qt::editor::SDynamicView' allows to launch the activity in a new
 * tab. For
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
 *       - \b type: type of series (data::ImageSeries, data::ModelSeries, ....)
 *       - \b id: identifier of the activity.
 *
 *
 * @note A sesh@ path is a path used to browse an object (and sub-object) using the introspection (see fwDataCamp).
 *       The path begins with a '@' or a '!'.
 *          - '@' : the returned string is the fwID of the sub-object defined by the path.
 *          - '!' : the returned string is the value of the sub-object, it works only on String, Integer, Float and
 *            Boolean object.
 */
class MODULE_UI_QT_CLASS_API SLauncher : public ::sight::ui::base::IAction
{

public:

    SIGHT_DECLARE_SERVICE(SLauncher, ::sight::ui::base::IAction)

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SLauncher() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~SLauncher() noexcept override;

    /**
     * @name Slot API
     * @{
     */
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_LAUNCH_SERIES_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_LAUNCH_ACTIVITY_SERIES_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_UPDATE_STATE_SLOT;
    /// @}

    /**
     * @name Signal API
     * @{
     */
    typedef core::com::Signal< void ( sight::activity::ActivityMsg ) > ActivityLaunchedSignalType;

    /// Key in m_signals map of signal m_sigActivityLaunched
    MODULE_UI_QT_API static const core::com::Signals::SignalKeyType s_ACTIVITY_LAUNCHED_SIG;
    /// @}

protected:

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Vector::s_ADDED_OBJECTS_SIG to this::s_UPDATE_STATE_SLOT
     * Connect Vector::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_STATE_SLOT
     */
    MODULE_UI_QT_API virtual KeyConnectionsMap getAutoConnections() const override;

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
     * @see sight::ui::base::IAction::initialize()
     */
    virtual void configuring() override;

    typedef sight::activity::extension::ActivityAppConfig::ActivityAppConfigParamsType ParametersType;

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
    bool launchAS(const data::Vector::csptr& selection);

    /**
     * @brief Slots to launch the given series.
     * @param series the activity is launched on this series.
     *
     * If series is an ActivitySeries, it is launched, otherwise it launches the first available activity for
     * this series or used m_quickLaunch information if a default association is defined for this series type.
     */
    void launchSeries(data::Series::sptr series);

    /**
     * @brief Slots to launch the given activity series.
     * @param series the activity series to launch.
     */
    void launchActivitySeries(data::ActivitySeries::sptr series);

    /**
     * @brief Send message to launch new tab view
     * If given activity info contains an activity::IValidator, first checks if activity is valid according to
     * validator, then build activity with activity builder.
     *
     * @param info activity information
     */
    void sendConfig( const sight::activity::extension::ActivityInfo& info );

    /**
     * @brief Builds and launch activity with the input data given in selection.
     *
     * @param info activity information
     * @param selection input data to launch the activity
     */
    void buildActivity(const sight::activity::extension::ActivityInfo& info, const data::Vector::csptr& selection);

    typedef sight::activity::extension::Activity::ActivitiesType ActivityInfoContainer;

    /// Show custom dialog box
    sight::activity::extension::ActivityInfo show( const ActivityInfoContainer& infos );

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

    /// SLauncher's mode (message or immediate)
    std::string m_mode;

    /// Defines quick association between series type (a classname) and an activity id used by launchSeries method
    QuickLaunchType m_quickLaunch;
};

} //action
} // gui
