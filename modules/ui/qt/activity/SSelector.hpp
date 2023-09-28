/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include <activity/extension/Activity.hpp>
#include <activity/message.hpp>

#include <ui/__/editor.hpp>

#include <QButtonGroup>
#include <QObject>
#include <QPointer>

namespace sight::module::ui::qt::activity
{

/**
 * @brief This editor launches an activity according to the given configuration
 *
 * This editor proposes all the available activities according to the given configuration.
 * It sends a signal with the activity identifier when a button is pushed.
 *
 * It should work with the module::ui::qt::editor::SWizard that creates or updates the activity.
 *
 * @section Signals Signals
 * - \b activityIDSelected(std::string) : This signal is emitted when the activity is selected, it
 *      contains all activity identifier. It should be connected to the slot 'createActivity' of the service
 *     'SWizard'.
 * - \b loadRequested() : This signal is emitted when the "load activity" button is pushed.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="editor_newActivity" type="sight::module::ui::qt::activity::SSelector" >
       <filter>
           <mode>include</mode>
           <id>activity_viz_negato</id>
           <id>3DVisualizationActivity</id>
           <id>VolumeRenderingActivity</id>
       </filter>
   </service>
   @endcode
 *
 * - \b filter (optional): it allows to filter the activity that can be proposed.
 *    - \b mode: 'include' or 'exclude'. Defines if the activity in the following list are proposed (include) or not
 *      (exclude).
 *    - \b id: id of the activity
 */
class MODULE_UI_QT_CLASS_API SSelector : public QObject,
                                         public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SSelector, sight::ui::editor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SSelector() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~SSelector() noexcept override;

    /**
     * @name Signals API
     * @{
     */
    MODULE_UI_QT_API static const core::com::signals::key_t ACTIVITY_ID_SELECTED_SIG;
    typedef core::com::signal<void (std::string)> ActivityIDSelectedSignalType;

    MODULE_UI_QT_API static const core::com::signals::key_t LOAD_REQUESTED_SIG;
    typedef core::com::signal<void ()> LoadRequestedSignalType;

    /// @}

protected:

    /// Initialize the editor.
    void configuring() override;

    /// This method launches the editor::starting method.
    void starting() override;

    /// This method launches the editor::stopping method.
    void stopping() override;

    /// Show activity selector.
    void updating() override;

    typedef std::vector<std::string> KeysType;

private Q_SLOTS:

    void onClicked(int id);

private:

    /**
     * @brief Slots to launch the given activity.
     * @param activity the activity to be launched.
     */
    void launchActivity(data::Activity::sptr activity);

    typedef sight::activity::extension::Activity::ActivitiesType ActivityInfoContainer;

    /// Returns enabled activity infos according to activity filter.
    ActivityInfoContainer getEnabledActivities(const ActivityInfoContainer& infos);

    /**
     * @brief Filter mode : include or exclude activity configurations.
     * @note Allowed values : 'include' or 'exclude'
     */
    std::string m_filterMode;

    /// Id-s of activity configurations to be enabled or disabled, according to filter mode.
    KeysType m_keys;

    /// Informations used to launch activities
    ActivityInfoContainer m_activitiesInfo;

    /// Pointer on the buttons group
    QPointer<QButtonGroup> m_buttonGroup;
};

} // namespace sight::module::ui::qt::activity
