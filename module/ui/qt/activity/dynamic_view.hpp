/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <activity/extension/activity.hpp>
#include <activity/message.hpp>

#include <app/config_manager.hpp>

#include <data/activity.hpp>

#include <ui/__/activity_view.hpp>
#include <ui/qt/container/widget.hpp>

#include <QObject>
#include <QPointer>

#include <map>
#include <set>

class QTabWidget;
namespace sight::data
{

class map;

} // namespace sight::data
namespace sight::module::ui::qt::activity
{

/**
 * @brief   This editor manages tabs containing activities.
 *
 * This service should received signals containing Activity connected to the slot \b launch_activity. It will
 * launch the activity in a new tab.
 *
 * @note The same activity cannot be launch in two different tabs.
 *
 * @section Signal Signal
 * - \b activitySelected( data::object::sptr ): this signal is emitted when the current tab selection
 *   changed, it contains the associated Activity. The activity is send as a data::object in order to
 *   connect this signal to slots receiving a data::object.
 * - \b nothingSelected(): this signal is emitted when no tab are selected.
 *
 * @section Slots Slots
 * - \b launch_activity( data::activity::sptr ): this slot allows to create a tab with the given activity.
 * - \b create_tab( activity::message ): this slot allows to create a tab with the given activity information.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::ui::qt::activity::dynamic_view" auto_connect="true" >
     <mainActivity id="SDBActivity" closable="false" />
     <parameters>
         <parameter replace="SERIES_SET" by="medicalData"  />
         <parameter replace="ICON_PATH" by="sight::module::ui::icons/app.ico"  />
     </parameters>
     <config document="true" />
   </service>
   @endcode
 * - \b mainActivity (optional): information about the main activity (first tab). The activity will be generated.
 *   This activity must not have requirement.
 *   - \b id : identifier of the activity
 *   - \b closable (optional, default 'no') : defines if the user can close this tab.
 * - \b parameters (optional) : additional parameters used to launch the activities
 *    - \b parameter: defines a parameter
 *        - \b replace: name of the parameter as defined in the config
 *        - \b by: defines the string that will replace the parameter name.
 *
 * @subsection Configuration Configuration:
 *  - \b document (optional, default="true") : sets the document mode of the tab bar.
 */
class dynamic_view : public QObject,
                     public sight::ui::activity_view
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(dynamic_view, sight::ui::activity_view);

    /// Constructor. Do nothing.
    dynamic_view() noexcept;

    /// Destructor. Do nothing.

    ~dynamic_view() noexcept override;

    using activity_selected_signal_t = core::com::signal<void (data::object::sptr)>;
    static const core::com::signals::key_t ACTIVITY_SELECTED_SIG;
    using nothing_selected_signal_t = core::com::signal<void ()>;
    static const core::com::signals::key_t NOTHING_SELECTED_SIG;

protected:

    /**
     * @brief Install the container.
     */
    void starting() override;

    /**
     * @brief Destroy the container.
     */
    void stopping() override;

    /**
     * @brief Update
     *
     */
    void updating() override;

    /**
     * @brief Configure the view
     * @see sight::ui::service::initialize()
     */
    void configuring() override;

    void info(std::ostream& _sstream) override;

private:

    using activity_id_t = std::set<std::string>;

    struct dynamic_view_info
    {
        sight::ui::qt::container::widget::sptr container;
        sight::app::config_manager::sptr helper;
        std::string wid;
        std::string title;
        bool closable {};
        std::string icon;
        std::string tooltip;
        std::string tab_id;
        std::string view_config_id;
        std::map<std::string, std::string> replacement_map;
        data::activity::sptr activity;
    };

    using dynamic_view_info_map_type = std::map<QWidget*, dynamic_view_info>;

    /**
     * @brief Launch tab
     */
    void launch_tab(dynamic_view_info& _info);

    /**
     * @brief Slot: Launch the given activity in a new tab.
     * @note The same activity cannot be launched in two different tabs.
     */
    void launch_activity(data::activity::sptr _activity) override;

    /// launch a new tab according to the receiving msg
    void create_tab(sight::activity::message _info);

    /// Create the main activity and launch the activity
    virtual void build_main_activity();

    /// Create view info from activity
    dynamic_view_info create_view_info(data::activity::sptr _activity);

    /**
     * @brief Close the tab at the given index.
     * @param _index : index of the tab to close
     * @param _force_close : if true, close the tab even if the tab is not "closable"
     */
    void close_tab(int _index, bool _force_close);

protected Q_SLOTS:

    /// Called when the tab close button is clicked: close the tab if it is "closable"
    void close_tab_signal(int _index);

    /// Called when the current tab selection changed
    void changed_tab(int _index);

private:

    std::map<std::string, unsigned int> m_title_to_count;
    std::set<std::string> m_tab_id_list;

    activity_id_t m_activity_ids;

    dynamic_view_info_map_type m_dynamic_info_map;
    bool m_dynamic_config_start_stop {false};

    QPointer<QTabWidget> m_tab_widget;
    QPointer<QWidget> m_current_widget;

    activity_selected_signal_t::sptr m_sig_activity_selected;
    nothing_selected_signal_t::sptr m_sig_nothing_selected;

    bool m_main_activity_closable {true};

    /// Allows to set the document mode.
    bool m_document_mode {true};
};

} // namespace sight::module::ui::qt::activity
