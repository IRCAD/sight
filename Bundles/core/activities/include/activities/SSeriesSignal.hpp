/************************************************************************
 *
 * Copyright (C) 2016-2018 IRCAD France
 * Copyright (C) 2016-2018 IHU Strasbourg
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
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

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
    <service uid="action_newActivity" type="::activities::action::SSeriesSignal" autoConnect="yes" >
        <in key="seriesDB" uid="..." />
        <!-- Filter mode 'include' allows all given types.
             Filter mode 'exclude' allows all series excepted given ones. -->
        <filter>
            <mode>include</mode>
            <type>::fwMedData::ImageSeries</type>
            <type>::fwMedData::ModelSeries</type>
        </filter>
    </service>
        @endcode
 * @subsection Input Input
 * - \b seriesDB [::fwMedData::SeriesDB]: seriesDB to listen to forward the added series signal.
 *
 * @subsection Configuration Configuration
 * - \b filter (optional): it allows to filter the series that can be notified.
 *    - \b mode: 'include' or 'exclude'. Defines if the series is notified (include) or not (exclude).
 *    - \b id: id of the activity
 */
class ACTIVITIES_CLASS_API SSeriesSignal : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (SSeriesSignal)(::fwServices::IController) )

    /// Constructor. Do nothing.
    ACTIVITIES_API SSeriesSignal() noexcept;

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~SSeriesSignal() noexcept;

    ACTIVITIES_API static const ::fwCom::Slots::SlotKeyType s_REPORT_SERIES_SLOT;

    typedef ::fwCom::Signal< void ( ::fwMedData::Series::sptr ) > SeriesAddedSignalType;
    ACTIVITIES_API static const ::fwCom::Signals::SignalKeyType s_SERIES_ADDED_SIG;

protected:

    /// Do nothing
    virtual void starting() override;

    /// Do nothing
    virtual void stopping() override;

    /// Do nothing
    virtual void updating() override;

    /// Parse XML configuration
    virtual void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Vector::s_ADDED_OBJECTS_SIG to this::s_UPDATE_STATE_SLOT
     * Connect Vector::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_STATE_SLOT
     */
    ACTIVITIES_API KeyConnectionsMap getAutoConnections() const override;

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
