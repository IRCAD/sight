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

#include <app/config_manager.hpp>

#include <data/activity.hpp>

#include <ui/__/view/IActivityView.hpp>
#include <ui/qt/container/widget.hpp>

#include <QObject>
#include <QPointer>

#include <map>
#include <set>

class QTabWidget;
namespace sight::data
{

class composite;

} // namespace sight::data
namespace sight::module::ui::qt::activity
{

/**
 * @brief   This editor manages tabs containing activities.
 *
 * This service should received signals containing Activity connected to the slot \b launchActivity. It will
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
 * - \b launchActivity( data::activity::sptr ): this slot allows to create a tab with the given activity.
 * - \b createTab( activity::message ): this slot allows to create a tab with the given activity information.
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
class MODULE_UI_QT_CLASS_API dynamic_view : public QObject,
                                            public sight::ui::view::IActivityView
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(dynamic_view, sight::ui::view::IActivityView);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API dynamic_view() noexcept;

    /// Destructor. Do nothing.

    MODULE_UI_QT_API ~dynamic_view() noexcept override;

    typedef core::com::signal<void (data::object::sptr)> ActivitySelectedSignalType;
    MODULE_UI_QT_API static const core::com::signals::key_t ACTIVITY_SELECTED_SIG;
    typedef core::com::signal<void ()> NothingSelectedSignalType;
    MODULE_UI_QT_API static const core::com::signals::key_t NOTHING_SELECTED_SIG;

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

    typedef std::set<std::string> ActivityIdType;

    struct dynamic_viewInfo
    {
        sight::ui::qt::container::widget::sptr container;
        sight::app::config_manager::sptr helper;
        std::string wid;
        std::string title;
        bool closable {};
        std::string icon;
        std::string tooltip;
        std::string tabID;
        std::string viewConfigID;
        std::map<std::string, std::string> replacementMap;
        data::activity::sptr activity;
    };

    typedef std::map<QWidget*, dynamic_viewInfo> dynamic_viewInfoMapType;

    /**
     * @brief Launch tab
     */
    void launchTab(dynamic_viewInfo& info);

    /**
     * @brief Slot: Launch the given activity in a new tab.
     * @note The same activity cannot be launched in two different tabs.
     */
    void launchActivity(data::activity::sptr activity) override;

    /// launch a new tab according to the receiving msg
    void createTab(sight::activity::message info);

    /// Create the main activity and launch the activity
    virtual void buildMainActivity();

    /// Create view info from activity
    dynamic_viewInfo createViewInfo(data::activity::sptr activity);

    /**
     * @brief Close the tab at the given index.
     * @param index : index of the tab to close
     * @param forceClose : if true, close the tab even if the tab is not "closable"
     */
    void closeTab(int index, bool forceClose);

protected Q_SLOTS:

    /// Called when the tab close button is clicked: close the tab if it is "closable"
    void closeTabSignal(int index);

    /// Called when the current tab selection changed
    void changedTab(int index);

private:

    std::map<std::string, unsigned int> m_titleToCount;
    std::set<std::string> m_tabIDList;

    ActivityIdType m_activityIds;

    dynamic_viewInfoMapType m_dynamicInfoMap;
    bool m_dynamicConfigStartStop {false};

    QPointer<QTabWidget> m_tabWidget;
    QPointer<QWidget> m_currentWidget;

    ActivitySelectedSignalType::sptr m_sigActivitySelected;
    NothingSelectedSignalType::sptr m_sigNothingSelected;

    bool m_mainActivityClosable {true};

    /// Allows to set the document mode.
    bool m_documentMode {true};
};

} // namespace sight::module::ui::qt::activity
