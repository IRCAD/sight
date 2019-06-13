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

#include "activities/config.hpp"

#include <fwCom/Signal.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwServices/IController.hpp>

namespace activities
{

/**
 * @brief This service allows to launch activities sequentially.
 *
 * The order of the activities is given in the configuration.
 *
 * ActivitySeries are created for each activity using the data produced by the previous activities. This activities are
 * stored in the current SeriesDB.
 *
 * @warning The first launched activity must be able to start without parameters, or they must be supplied in the
 *     `requirementOverrides` composite.
 *
 * @section Signal Signal
 * - \b activityCreated(::fwMedData::ActivitySeries::sptr) : This signal is emitted when an activity is created (using
 *   next() or previous().
 * - \b dataRequired() : This signal is emitted when the activity can not be launch because it requires data.
 * - \b enabledNext(bool): This signal is emitted when the next button is enabled (when the activity is not the last
 *   one)
 * - \b enabledPrevious(bool): This signal is emitted when the previous button is enabled (when the activity is not the
 *   first one)
 *
 * @section Slots Slots
 * - \b next() : Create the next activity series
 * - \b previous() : Create the next activity series
 * - \b sendInfo() : Send the 'enabledNext' and 'enablePrevious' signals for the current activity
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service  type="::activities::SActivitySequencer">
        <inout key="seriesDB" uid=""  autoConnect="yes" />
        <in    key="requirementOverrides" uid="..." />
        <activity>...</activity>
        <activity>...</activity>
        <activity>...</activity>
    </service>
   @endcode
 * @subsection In In
 * - \b requirementOverrides [::fwData::Composite] (optional): provide some data that will be passed as activity
 *   requirements. Can be used to supply required data that cannot be created by the activity, or to override
 *   data that would normally be passed from an activity to the next.
 *
 * @subsection In-Out In-Out
 * - \b seriesDB [::fwMedData::SeriesDB]: used to store the ActivitySeries of the managed activities
 *
 * @subsection Configuration Configuration
 * - \b activity : id of the activities to launch. The first activity in the list is the first that will be launched.
 *
 * @todo listen the current activity data to notify when the next activity can be created
 */
class ACTIVITIES_CLASS_API SActivitySequencer : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (SActivitySequencer)(::fwServices::IController) )

    /// Constructor. Do nothing.
    ACTIVITIES_API SActivitySequencer() noexcept;

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~SActivitySequencer() noexcept;

    /// Connect the service to the SeriesDB signals
    ACTIVITIES_API virtual KeyConnectionsMap getAutoConnections() const override;

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal<void (::fwMedData::ActivitySeries::sptr ) > ActivityCreatedSignalType;
    typedef ::fwCom::Signal<void (::fwMedData::ActivitySeries::sptr) > DataRequiredSignalType;
    typedef ::fwCom::Signal<void (bool) > EnabledPreviousSignalType;
    typedef ::fwCom::Signal<void (bool) > EnabledNextSignalType;
    /**
     * @}
     */

protected:

    /// Do nothing
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

    /// Parse XML configuration
    virtual void configuring() override;

private:

    typedef std::vector< std::string > ActivitesType;
    typedef std::map< std::string, ::fwData::Object::sptr > RequirementsType;

    /// Slot: Create the next activity series
    void next();

    /// Slot: Create the previous activity series
    void previous();

    /// Slot: Send the 'enabledNext' and 'enablePrevious' signals for the current activity
    void sendInfo() const;

    /// Store the current activity data
    void storeActivityData();

    /// Create activity corresponding to the given index in the activity list
    ::fwMedData::ActivitySeries::sptr getActivity(size_t index);

    /// Check if the activity can be launch. If showDialog = true, display the reason
    bool checkValidity(const ::fwMedData::ActivitySeries::csptr& activity, bool showDialog = true) const;

    /// List of the activity to create.
    ActivitesType m_activityIds;

    /// Index of the current activity
    int m_currentActivity;

    /// Map containing all the data produced by the activities. It is used to create the next one.
    RequirementsType m_requirements;

    ActivityCreatedSignalType::sptr m_sigActivityCreated;
    DataRequiredSignalType::sptr m_sigDataRequired;
    EnabledPreviousSignalType::sptr m_sigEnabledPrevious;
    EnabledNextSignalType::sptr m_sigEnabledNext;

};

} // activities
