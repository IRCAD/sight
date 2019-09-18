/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
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

#include "uiActivitiesQml/config.hpp"

#include <fwActivities/IActivitySequencer.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwQml/IQmlEditor.hpp>

#include <QStringList>

namespace uiActivitiesQml
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
 * This service should be associated to the SActivityView to display the current activity
 *
 * @section Signal Signal
 * - \b activityCreated(::fwMedData::ActivitySeries::sptr) : This signal is emitted when an activity is created (using
 *   next() or previous().
 * - \b dataRequired() : This signal is emitted when the activity can not be launch because it requires data.
 *
 * @section Slots Slots
 * - \b goTo(int) : Create the activity series at the given index
 * - \b checkNext() : Chech if the next activities can be enabled
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
 * Liste the creation of the service in your AppManager ("onServiceCreated").
 *
 * @code{.cpp}
     m_activitySequencer->registerInOut(m_seriesDB, "seriesDB", true);
   @endcode
 *
 *  @subsubsection In-Out In-Out
 * - \b seriesDB [::fwMedData::SeriesDB]: used to store the ActivitySeries of the managed activities
 */
class UIACTIVITIESQML_CLASS_API SActivitySequencer : public ::fwQml::IQmlEditor,
                                                     public ::fwActivities::IActivitySequencer
{

Q_OBJECT
Q_PROPERTY(QStringList activityIds MEMBER m_qActivityIds)
public:

    fwCoreServiceClassDefinitionsMacro( (SActivitySequencer)(::fwQml::IQmlEditor) )

    /// Constructor. Do nothing.
    UIACTIVITIESQML_API SActivitySequencer();

    /// Destructor. Do nothing.

    UIACTIVITIESQML_API virtual ~SActivitySequencer() override;

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal<void (::fwMedData::ActivitySeries::sptr ) > ActivityCreatedSignalType;
    typedef ::fwCom::Signal<void (::fwMedData::ActivitySeries::sptr) > DataRequiredSignalType;
    /**
     * @}
     */

Q_SIGNALS:

    // Emitted when the activity at the given index can be launched
    void enable(int index);

public Q_SLOTS:

    /// Slot: create the activity at the given index, emit 'dataRequired' signal if the activity require additional data
    void goTo(int index);

protected:

    /// Do nothing
    virtual void configuring() override;

    /// Parse the Activity ids list
    virtual void starting() override;

    /// Do nothing
    virtual void stopping() override;

    /**
     * @brief Analyse the series contained in the current seriesDB.
     *
     * - if the series is not an activity or if it is an unknown activity, it is removed
     * - else, the activity data is stored in m_requirements
     * - the last activity is launched
     */
    virtual void updating() override;

    /// Connect the service to the SeriesDB signals
    virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// Slot: Chech if the next activities can be enabled
    void checkNext();

    /// Slot: Create the next activity series, emit 'dataRequired' signal if the activity require additional data
    void next();

    /// Slot: Create the previous activity series, emit 'dataRequired' signal if the activity require additional data
    void previous();

    ActivityCreatedSignalType::sptr m_sigActivityCreated;
    DataRequiredSignalType::sptr m_sigDataRequired;

    /// List of activity ids
    QStringList m_qActivityIds;
};

} // uiActivitiesQml
