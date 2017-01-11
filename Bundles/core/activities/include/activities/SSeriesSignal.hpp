/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ACTIVITIES_SSERIESSIGNAL_HPP__
#define __ACTIVITIES_SSERIESSIGNAL_HPP__

#include "activities/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Series.hpp>

#include <fwServices/IController.hpp>

namespace activities
{

/**
 * @brief This service emits a signal for each added series (the signal contains the added series)
 *
 * This action works on a ::fwMedData::SeriesDB. It listens all the added series in the SeriesDB and emits the signal.
 *
 * It is possible to configure the service to report only one type of series.
 *
 * @section Slots Slots
 * - \b reportSeries(::fwMedData::SeriesDB::ContainerType) : This slot is called when a series is added in the SeriesDB.
 *   It emits a signal for each added series corresponding to the configured type.
 *
 * @section Signal Signal
 * - \b seriesAdded(::fwMedData::Series::sptr) : This signal is emitted when the added series correspond to the
 *   configured type.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="action_newActivity" type="::fwGui::IActionSrv" impl="::activities::action::SSeriesSignal" autoConnect="yes" >
        <!-- Filter mode 'include' allows all given types.
             Filter mode 'exclude' allows all series excepted given ones. -->
        <filter>
            <mode>include</mode>
            <type>::fwMedData::ImageSeries</type>
            <type>::fwMedData::ModelSeries</type>
        </filter>
    </service>
        @endcode
 * - \b filter (optional): it allows to filter the series that can be notified.
 *    - \b mode: 'include' or 'exclude'. Defines if the series is notified (include) or not (exclude).
 *    - \b id: id of the activity
 */
class ACTIVITIES_CLASS_API SSeriesSignal : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SSeriesSignal)(::fwServices::IController) );

    /// Constructor. Do nothing.
    ACTIVITIES_API SSeriesSignal() throw();

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~SSeriesSignal() throw();

    ACTIVITIES_API static const ::fwCom::Slots::SlotKeyType s_REPORT_SERIES_SLOT;

    typedef ::fwCom::Signal< void ( ::fwMedData::Series::sptr ) > SeriesAddedSignalType;
    ACTIVITIES_API static const ::fwCom::Signals::SignalKeyType s_SERIES_ADDED_SIG;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Vector::s_ADDED_OBJECTS_SIG to this::s_UPDATE_STATE_SLOT
     * Connect Vector::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_STATE_SLOT
     */
    ACTIVITIES_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    /// Do nothing
    virtual void starting() throw(::fwTools::Failed);

    /// Do nothing
    virtual void stopping() throw(::fwTools::Failed);

    /// Do nothing
    virtual void updating() throw(::fwTools::Failed);

    /// Parse XML configuration
    virtual void configuring() throw(fwTools::Failed);

private:

    typedef std::vector< std::string > TypesType;

    /**
     * @brief This slot is called when a series is added in the SeriesDB.
     * It emits a signal for each added series corresponding to the configured type.
     */
    void reportSeries(::fwMedData::SeriesDB::ContainerType addedSeries);

    /**
     * @brief Filter mode : include or exclude activity configurations.
     * @note Allowed values : 'include' or 'exclude'
     */
    std::string m_filterMode;

    /// Types of series to be enabled or disabled, according to filter mode.
    TypesType m_types;

    /// Signal emitted when the added series correspond to the configured type.
    SeriesAddedSignalType::sptr m_sigSeriesAdded;

};

} // gui


#endif // __ACTIVITIES_SSERIESSIGNAL_HPP__
