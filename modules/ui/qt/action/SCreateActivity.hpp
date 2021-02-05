/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include <activities/registry/Activities.hpp>
#include <activities/registry/ActivityMsg.hpp>

#include <data/Vector.hpp>

#include <ui/base/IActionSrv.hpp>

namespace sight::modules::ui::qt
{
namespace action
{

/**
 * @brief This action launchs an activity according to the given configuration
 *
 * This action works on a data::Vector. It proposes all the available activity according to the given configuration.
 * And then, it sends a signal with the activity identifier.
 *
 * It should work with the modules::ui::qt::editor::SActivityWizard that creates or updates the activitySeries.
 *
 * @section Signals Signals
 * - \b activityIDSelected(std::string) : This signal is emitted when the activity is selected, it
 *      contains all activity identifier. It should be connected to the slot 'createActivity' of the service
 *     'SActivityWizard'.
 * - \b activitySelected(data::ActivitySeries::sptr) : This signal is emitted when the activity is selected in
 *      the current vector. It should be connected to the slot 'updateActivity' of the service 'SActivityWizard'.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::sight::modules::ui::qt::action::SCreateActivity" >
       <!-- Filter mode 'include' allows all given activity id-s.
            Filter mode 'exclude' allows all activity id-s excepted given ones. -->
       <filter>
           <mode>include</mode>
           <id>2DVisualizationActivity</id>
           <id>3DVisualizationActivity</id>
           <id>VolumeRenderingActivity</id>
       </filter>
   </service>
   @endcode
 *
 * - \b filter (optional): it allows to filter the activity that can be proposed.
 *    - \b mode: 'include' or 'exclude'. Defines if the activities in the following list are proposed (include) or not
 *      (exclude).
 *    - \b id: id of the activity
 */
class MODULE_UI_QT_CLASS_API SCreateActivity : public ::sight::ui::base::IActionSrv
{

public:

    fwCoreServiceMacro(SCreateActivity, ::sight::ui::base::IActionSrv)

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SCreateActivity() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~SCreateActivity() noexcept override;

    /**
     * @name Signals API
     * @{
     */
    MODULE_UI_QT_API static const core::com::Signals::SignalKeyType s_ACTIVITY_SELECTED_SIG;
    typedef core::com::Signal< void ( data::ActivitySeries::sptr ) > ActivitySelectedSignalType;

    MODULE_UI_QT_API static const core::com::Signals::SignalKeyType s_ACTIVITY_ID_SELECTED_SIG;
    typedef core::com::Signal< void (std::string) > ActivityIDSelectedSignalType;
    /// @}

protected:

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
     * @see sight::ui::base::IActionSrv::initialize()
     */
    virtual void configuring() override;

    typedef std::vector< std::string > KeysType;

private:

    /**
     * @brief Slots to launch the given activitySeries.
     * @param activitySeries the activity is launched on this series.
     */
    void launchActivity(data::ActivitySeries::sptr activitySeries);

    typedef activities::registry::Activities::ActivitiesType ActivityInfoContainer;

    /// Show custom dialog box
    activities::registry::ActivityInfo show( const ActivityInfoContainer& infos );

    /// Returns enabled activity infos according to activity filter.
    ActivityInfoContainer getEnabledActivities(const ActivityInfoContainer& infos);

    /**
     * @brief Filter mode : include or exclude activity configurations.
     * @note Allowed values : 'include' or 'exclude'
     */
    std::string m_filterMode;

    /// Id-s of activity configurations to be enabled or disabled, according to filter mode.
    KeysType m_keys;

    /// Signal emitted when activity is selected. Contains the activity identifier.
    ActivityIDSelectedSignalType::sptr m_sigActivityIDSelected;

    /// Signal emitted when activity is selected. Contains the activity.
    ActivitySelectedSignalType::sptr m_sigActivitySelected;
};

} // namespace action
} // namespace sight::modules::ui::qt
