/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "modules/ui/qml/config.hpp"

#include <activity/IActivitySequencer.hpp>

#include <data/ActivitySeries.hpp>

#include <ui/qml/IQmlEditor.hpp>

#include <QStringList>

namespace sight::module::ui::qml::activity
{

/**
 * @brief This editor displays an activity stepper that allows to select the activity to launch, and display the
 * current selection
 *
 * The order of the activities is given in the Qml file.
 *
 * ActivitySeries are created for each activity using the data produced by the previous activities. This activities are
 * stored in the current SeriesDB.
 *
 * This service should be associated to the SView to display the current activity
 *
 * @section Signal Signal
 * - \b activityCreated(data::ActivitySeries::sptr) : This signal is emitted when an activity is created (using
 *   next() or previous().
 * - \b dataRequired() : This signal is emitted when the activity can not be launch because it requires data.
 *
 * @section Slots Slots
 * - \b goTo(int) : Create the activity series at the given index
 * - \b checkNext() : Check if the next activities can be enabled
 *
 * @section Config Configuration
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
 * the seriesDB inout and register the service.
 *
 * @code{.cpp}
     m_activitySequencer->setInOut(m_seriesDB, "seriesDB", true);
   @endcode
 *
 *  @subsubsection In-Out In-Out
 * - \b seriesDB [sight::data::SeriesDB]: used to store the ActivitySeries of the managed activities
 */
class MODULE_UI_QML_CLASS_API SSequencer : public sight::ui::qml::IQmlEditor,
                                           public sight::activity::IActivitySequencer
{
Q_OBJECT
Q_PROPERTY(QStringList activityIds MEMBER m_qActivityIds)

public:

    SIGHT_DECLARE_SERVICE(SSequencer, sight::ui::qml::IQmlEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QML_API SSequencer();

    /// Destructor. Do nothing.

    MODULE_UI_QML_API ~SSequencer() override;

    /**
     * @name Signals API
     * @{
     */
    typedef core::com::Signal<void (data::ActivitySeries::sptr)> ActivityCreatedSignalType;
    typedef core::com::Signal<void (data::ActivitySeries::sptr)> DataRequiredSignalType;
/**
 * @}
 */

Q_SIGNALS:

    // Emitted when the activity at the given index can be launched
    void enable(int index);
    void select(int index);

public Q_SLOTS:

    /// Slot: create the activity at the given index, emit 'dataRequired' signal if the activity require additional data
    void goTo(int index);

protected:

    /// Do nothing
    void configuring() override;

    /// Parse the Activity ids list
    void starting() override;

    /// Do nothing
    void stopping() override;

    /**
     * @brief Analyse the series contained in the current seriesDB.
     *
     * - if the series is not an activity or if it is an unknown activity, it is removed
     * - else, the activity data is stored in m_requirements
     * - the last activity is launched
     */
    void updating() override;

    /// Connect the service to the SeriesDB signals
    KeyConnectionsMap getAutoConnections() const override;

private:

    /// Slot: Check if the next activities can be enabled
    void checkNext();

    /// Slot: Create the next activity series, emit 'dataRequired' signal if the activity require additional data
    void next();

    /// Slot: Create the previous activity series, emit 'dataRequired' signal if the activity require additional data
    void previous();

    ActivityCreatedSignalType::sptr m_sigActivityCreated;
    DataRequiredSignalType::sptr m_sigDataRequired;

    /// List of activity ids
    QStringList m_qActivityIds;

    static constexpr std::string_view s_SERIESDB_INOUT = "seriesDB";
    data::ptr<data::SeriesDB, data::Access::inout> m_seriesDB {this, "seriesDB", true};
};

} // uiActivitiesQml
