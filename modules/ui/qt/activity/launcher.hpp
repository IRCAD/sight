/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <activity/extension/activity.hpp>
#include <activity/message.hpp>

#include <data/series.hpp>
#include <data/vector.hpp>

#include <ui/__/action.hpp>

namespace sight::module::ui::qt::activity
{

/**
 * @brief This action launches an activity according to the selected data
 *
 * This action works on a data::vector. It proposes all the available activity according to the selected data and
 * the given configuration. And then, send a signal with all the activity information.
 *
 * This action should be followed by the service '::module::ui::qt::editor::dynamic_view' : this service listens the
 * action
 * signals and launches the activity in a new tab.
 *
 * @section Slots Slots
 * - \b launch_series(data::series::sptr) : This slot launches the first available activity for this series or uses
 *   m_quickLaunch information if a default association is defined for this series type. It sends the signal
 *   'activityLaunched' with all the activity information.
 *
 * - \b launch_activity(data::activity::sptr) : This slot allows to launch the given activity.
 *   It sends the signal 'activityLaunched' with all the activity information.
 *
 * - \b update_state() : Updates action state (enable if activities are available for current selection).
 *
 * @section Signal Signal
 * - \b activity_launched(activity::message) : This signal is emitted when the activity is created,
 *      it contains the activity information. It should be connected to the slot 'createTab' of the service
 *      '::module::ui::qt::editor::dynamic_view'.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="action_newActivity" type="sight::module::ui::qt::activity::launcher" auto_connect="true" >
        <in key="series" uid="..." />
        <config>
            <!-- launcher mode : immediate or message(default)
                 Immediate mode starts and stop immediately the activity's config -->
            <mode>immediate</mode>
            <parameters>
                <parameter replace="SERIES_SET" by="medicalData" />
            </parameters>

            <!-- filter mode 'include' allows all given activity id-s.
                 filter mode 'exclude' allows all activity id-s excepted given ones. -->
            <filter>
                <mode>include</mode>
                <id>activity_viz_negato</id>
                <id>3DVisualizationActivity</id>
                <id>VolumeRenderingActivity</id>
            </filter>

            <!-- Definition of quick association between series type and an activity id.
                 This mapping is used by launchSeries slot (see the function to have few details ) -->
            <quickLaunch>
                <association type="data::image_series" id="activity_viz_negato" />
                <association type="data::model_series" id="3DVisualizationActivity" />
            </quickLaunch>
        </config>
    </service>
        @endcode
 * @subsection Input Input
 * - \b series [sight::data::vector]: vector containing series inherited from data::series
 * @subsection Configuration Configuration
 * - \b mode (optional): there are two mode: "message" and "immediate"
 *    - \b message (used by default): the action send a signal containing the information needed to launch the
 *      chosen activity. The service '::module::ui::qt::editor::dynamic_view' allows to launch the activity in a new
 *      tab. For that, it must listen the action signal.
 *    - \b immediate: the activity is automatically started et stopped by this action. It is used to run a process
 *      without creating a new tab, for example, to save the selected data.
 * - \b parameters (optional): list of the parameters used to launch the activity, it is the parameters for the
 *   config associated to the activity.
 *    - \b parameter: defines a parameter
 *        - \b replace: name of the parameter as defined in the config
 *        - \b by: defines the string that will replace the parameter name. It shall be a simple string (ex. frontal)
 * - \b filter (optional): it allows to filter the activity that can be proposed.
 *    - \b mode: 'include' or 'exclude'. Defines if the activity in the following list are proposed (include) or not
 *      (exclude).
 *    - \b id: id of the activity
 * - \b quickLaunch (optional): defines the activity that will be launched on a double-click on a series. The
 *   launched activity depends of the series type (ImageSeries, ModelSeries, ...).
 *    - \b association: allows to associate an activity to launch with a type of series
 *       - \b type: type of series (data::image_series, data::model_series, ....)
 *       - \b id: identifier of the activity.
 */
class MODULE_UI_QT_CLASS_API launcher : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(launcher, sight::ui::action);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API launcher() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~launcher() noexcept override;

    /**
     * @name Slot API
     * @{
     */
    MODULE_UI_QT_API static const core::com::slots::key_t LAUNCH_SERIES_SLOT;
    MODULE_UI_QT_API static const core::com::slots::key_t LAUNCH_ACTIVITY_SLOT;
    MODULE_UI_QT_API static const core::com::slots::key_t UPDATE_STATE_SLOT;

    /// @}

    /**
     * @name Signal API
     * @{
     */
    using activity_launched_signal_t = core::com::signal<void (sight::activity::message)>;

    /// Key in m_signals map of signal m_sigActivityLaunched
    MODULE_UI_QT_API static const core::com::signals::key_t ACTIVITY_LAUNCHED_SIG;

    /// @}

protected:

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Vector::ADDED_OBJECTS_SIG to this::UPDATE_STATE_SLOT
     * Connect Vector::REMOVED_OBJECTS_SIG to this::UPDATE_STATE_SLOT
     */
    MODULE_UI_QT_API connections_t auto_connections() const override;

    ///This method launches the action::starting method.
    void starting() override;

    ///This method launches the action::stopping method.
    void stopping() override;

    /**
     * @brief Show activity selector.
     */
    void updating() override;

    /**
     * @brief Initialize the action.
     * @see sight::ui::action::initialize()
     */
    void configuring() override;

    using parameters_t = sight::activity::extension::activity_config_params_type;

    using keys_t = std::vector<std::string>;

    using quick_launch_t = std::map<std::string, std::string>;

    //// SLOT: Updates action state (enable if activities are available for current selection).
    virtual void update_state();

private:

    /**
     * @brief Launches activity if only Activity are selected.
     * @return Returns true if only Activity are selected.
     */
    bool launch_as(const data::vector::csptr& _selection);

    /**
     * @brief Slots to launch the given series.
     * @param series the activity is launched on this series.
     *
     * If series is an Activity, it is launched, otherwise it launches the first available activity for
     * this series or used m_quickLaunch information if a default association is defined for this series type.
     */
    void launch_series(data::series::sptr _series);

    /**
     * @brief Slots to launch the given activity.
     * @param series the activity to launch.
     */
    void launch_activity(data::activity::sptr _activity);

    /**
     * @brief Send message to launch new tab view
     * If given activity info contains an activity::validator::base, first checks if activity is valid according to
     * validator, then build activity with activity builder.
     *
     * @param info activity information
     */
    void send_config(const sight::activity::extension::activity_info& _info);

    /**
     * @brief Builds and launch activity with the input data given in selection.
     *
     * @param info activity information
     * @param selection input data to launch the activity
     */
    void build_activity(const sight::activity::extension::activity_info& _info, const data::vector::csptr& _selection);

    using activity_infos_t = sight::activity::extension::activity::infos_t;

    /// Show custom dialog box
    static sight::activity::extension::activity_info show(const activity_infos_t& _infos);

    /// Returns enabled activity infos according to activity filter.
    activity_infos_t get_enabled_activities(const activity_infos_t& _infos);

    parameters_t m_parameters;

    /**
     * @brief filter mode : include or exclude activity configurations.
     * @note Allowed values : 'include' or 'exclude'
     */
    std::string m_filter_mode;

    /// Id-s of activity configurations to be enabled or disabled, according to filter mode.
    keys_t m_keys;

    /// Signal emitted when activity is launched. Send a message containing the activity information.
    activity_launched_signal_t::sptr m_sig_activity_launched;

    /// launcher's mode (message or immediate)
    std::string m_mode;

    /// Defines quick association between series type (a classname) and an activity id used by launchSeries method
    quick_launch_t m_quick_launch;

    static constexpr std::string_view SERIES = "series";

    data::ptr<data::vector, sight::data::access::in> m_series {this, SERIES};
};

} // namespace sight::module::ui::qt::activity
