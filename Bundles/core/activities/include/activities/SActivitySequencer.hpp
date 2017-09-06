/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ACTIVITIES_SACTIVITYSEQUENCER_HPP__
#define __ACTIVITIES_SACTIVITYSEQUENCER_HPP__

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
 * @warning The first launched activity must be able to start without parameters.
 *
 * @section Signal Signal
 * - \b activityCreated(::fwMedData::ActivitySeries::sptr) : This signal is emitted when an activity is created (using
 *   next() or previous().
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
        <activity>...</activity>
        <activity>...</activity>
        <activity>...</activity>
    </service>
        @endcode
 * - \b activity : id of the activities to launch. The first activity in the list is the first that will be launched.
 *
 * @todo listen the current activity data to notify when the next activity can be created
 */
class ACTIVITIES_CLASS_API SActivitySequencer : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (SActivitySequencer)(::fwServices::IController) );

    /// Constructor. Do nothing.
    ACTIVITIES_API SActivitySequencer() noexcept;

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~SActivitySequencer() noexcept;

    /// Connect the service to the SeriesDB signals
    ACTIVITIES_API virtual KeyConnectionsMap getAutoConnections() const;

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal<void (::fwMedData::ActivitySeries::sptr ) > ActivityCreatedSignalType;
    typedef ::fwCom::Signal<void (bool) > EnabledPreviousSignalType;
    typedef ::fwCom::Signal<void (bool) > EnabledNextSignalType;
    /**
     * @}
     */

protected:

    /// Do nothing
    virtual void starting();

    /// Do nothing
    virtual void stopping();

    /**
     * @brief Analyse the series contained in the current seriesDB.
     *
     * - if the series is not an activity or if it is an unknown activity, it is removed
     * - else, the activity data is stored in m_requirements
     * - the last activity is launched
     */
    virtual void updating();

    /// Parse XML configuration
    virtual void configuring();

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
    EnabledPreviousSignalType::sptr m_sigEnabledPrevious;
    EnabledNextSignalType::sptr m_sigEnabledNext;

};

} // activities

#endif // __ACTIVITIES_SACTIVITYSEQUENCER_HPP__
