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

#include <activity/extension/activity.hpp>
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
 * It should work with the module::ui::qt::editor::wizard that creates or updates the activity.
 *
 * @section Signals Signals
 * - \b activity_id_selected(std::string) : This signal is emitted when the activity is selected, it
 *      contains all activity identifier. It should be connected to the slot 'createActivity' of the service
 *     'wizard'.
 * - \b load_requested() : This signal is emitted when the "load activity" button is pushed.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="editor_newActivity" type="sight::module::ui::qt::activity::selector" >
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
class MODULE_UI_QT_CLASS_API selector : public QObject,
                                        public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(selector, sight::ui::editor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API selector() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~selector() noexcept override;

    /**
     * @name Signals API
     * @{
     */
    MODULE_UI_QT_API static const core::com::signals::key_t ACTIVITY_ID_SELECTED_SIG;
    using activity_id_selected_signal_t = core::com::signal<void (std::string)>;

    MODULE_UI_QT_API static const core::com::signals::key_t LOAD_REQUESTED_SIG;
    using load_requested_signal_t = core::com::signal<void ()>;

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

    using keys_t = std::vector<std::string>;

private Q_SLOTS:

    void on_clicked(int _id);

private:

    /**
     * @brief Slots to launch the given activity.
     * @param activity the activity to be launched.
     */
    void launch_activity(data::activity::sptr _activity);

    using activity_infos_t = sight::activity::extension::activity::infos_t;

    /// Returns enabled activity infos according to activity filter.
    activity_infos_t get_enabled_activities(const activity_infos_t& _infos);

    /**
     * @brief filter mode : include or exclude activity configurations.
     * @note Allowed values : 'include' or 'exclude'
     */
    std::string m_filter_mode;

    /// Id-s of activity configurations to be enabled or disabled, according to filter mode.
    keys_t m_keys;

    /// Informations used to launch activities
    activity_infos_t m_activities_info;

    /// Pointer on the buttons group
    QPointer<QButtonGroup> m_button_group;
};

} // namespace sight::module::ui::qt::activity
