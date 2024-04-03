/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <activity/sequencer.hpp>

#include <data/activity.hpp>
#include <data/activity_set.hpp>

#include <ui/qml/editor.hpp>

#include <QStringList>

namespace sight::module::ui::qml::activity
{

/**
 * @brief This editor displays an activity stepper that allows to select the activity to launch, and display the
 * current selection
 *
 * The order of the activities is given in the Qml file.
 *
 * Activity are created for each activity using the data produced by the previous activities. This activities are
 * stored in the current ActivitySet.
 *
 * This service should be associated to the view to display the current activity
 *
 * @section Signal Signal
 * - \b activity_created(data::activity::sptr) : This signal is emitted when an activity is created (using
 *   next() or previous().
 * - \b data_required() : This signal is emitted when the activity can not be launch because it requires data.
 *
 * @section Slots Slots
 * - \b go_to(int) : Create the activity at the given index
 * - \b check_next() : Check if the next activities can be enabled
 *
 * @section config Configuration
 *
 * @subsection Qml Qml Configuration
 *
 * To use this service, import 'uiActivitiesQml' in your qml file and use the ActivitySequencer object:
 *
 * @code{.qml}
 *  ActivitySequencer {
        activityIdsList: activityLauncher.activityIdsList
        activityNameList: activityLauncher.activityNameList

        onServiceCreated: {
            appManager.onServiceCreated(srv)
        }
    }
    @endcode
 * - \n activityIdsList: identifiers of the activities to launch
 * - \n activityNameList: name of the activities to launch, that will be displays in the stepper
 *
 * @subsection Cpp C++ Configuration
 *
 * Listen the creation of the service in your AppManager with the method "onServiceCreated(srv)" and then associate the
 * the activity_set inout and register the service.
 *
 * @code{.cpp}
     m_activitySequencer->set_inout(m_activity_set, "activitySet", true);
   @endcode
 *
 *  @subsubsection In-Out In-Out
 * - \b activity_set [sight::data::activity_set]: used to store the Activity of the managed activities
 */
class sequencer : public sight::ui::qml::editor,
                  public sight::activity::sequencer
{
Q_OBJECT
Q_PROPERTY(QStringList activityIds MEMBER m_activity_list)

public:

    SIGHT_DECLARE_SERVICE(sequencer, sight::ui::qml::editor);

    /// Constructor. Do nothing.
    sequencer();

    /// Destructor. Do nothing.

    ~sequencer() override;

    /**
     * @name Signals API
     * @{
     */
    using activity_created_signal_t = core::com::signal<void (data::activity::sptr)>;
    using data_required_signal_t    = core::com::signal<void (data::activity::sptr)>;
/**
 * @}
 */

Q_SIGNALS:

    // Emitted when the activity at the given index can be launched
    void enable(int _index);
    void select(int _index);

public Q_SLOTS:

    /// Slot: create the activity at the given index, emit 'dataRequired' signal if the activity require additional data
    void go_to(int _index);

protected:

    /// Do nothing
    void configuring() override;

    /// Parse the Activity ids list
    void starting() override;

    /// Do nothing
    void stopping() override;

    /**
     * @brief Analyse the activity contained in the current activity_set.
     *
     * - if the activity is unknown, it is removed
     * - else, the activity data is stored in m_requirements
     * - the last activity is launched
     */
    void updating() override;

    /// Connect the service to the ActivitySet signals
    connections_t auto_connections() const override;

private:

    /// Slot: Check if the next activities can be enabled
    void check_next();

    /// Slot: Create the next activity, emit 'dataRequired' signal if the activity require additional data
    void next();

    /// Slot: Create the previous activity, emit 'dataRequired' signal if the activity require additional data
    void previous();

    activity_created_signal_t::sptr m_sig_activity_created;
    data_required_signal_t::sptr m_sig_data_required;

    /// List of activity ids
    QStringList m_activity_list;

    static constexpr std::string_view ACTIVITY_SET_INOUT = "activitySet";
    data::ptr<data::activity_set, data::access::inout> m_activity_set {this, ACTIVITY_SET_INOUT, true};
};

} // namespace sight::module::ui::qml::activity
