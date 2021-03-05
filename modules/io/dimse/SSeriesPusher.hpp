/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/io/dimse/config.hpp"

#include <core/com/Slot.hpp>
#include <core/thread/Worker.hpp>

#include <io/dimse/data/PacsConfiguration.hpp>
#include <io/dimse/SeriesEnquirer.hpp>

#include <service/IController.hpp>

#include <vector>

namespace sight::data
{
class Series;
}

namespace sight::module::io::dimse
{

/**
 * @brief   This service is used to push a DICOM series to a PACS.
 *
 * @section Signals Signals
 * - \b progressed(std::string) : Signal to start the progress (bar id).
 * - \b startedProgress(std::string, float, std::string) :  Signal to update the progress (bar id, percentage, message).
 * - \b stoppedProgress(std::string) : Signal to stop the progress (bar id).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::dimse::SSeriesPusher">
            <in key="pacsConfig" uid="..." />
            <in key="selectedSeries" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b pacsConfig [sight::io::dimse::data::PacsConfiguration]: PACS configuration data.
 * - \b selectedSeries [sight::data::Vector]: List of DICOM series to push to the PACS.
 */
class MODULE_IO_DIMSE_CLASS_API SSeriesPusher : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SSeriesPusher,  service::IController )

    typedef std::vector< CSPTR(data::Series) > DicomSeriesContainerType;

    MODULE_IO_DIMSE_API static const core::com::Slots::SlotKeyType s_DISPLAY_SLOT;
    typedef core::com::Slot<void (const std::string&, bool)> DisplayMessageSlotType;

    /// Signal to start the progress (bar id)
    typedef core::com::Signal< void ( std::string ) > StartedProgressSignalType;
    /// Signal to update the progress (bar id, percentage, message)
    typedef core::com::Signal< void ( std::string, float, std::string ) > ProgressedSignalType;
    /// Signal to stop the progress (bar id)
    typedef core::com::Signal< void ( std::string ) > StoppedProgressSignalType;

    /// Key in m_signals map of signal m_sigProgressed
    static const core::com::Signals::SignalKeyType s_PROGRESSED_SIG;
    static const core::com::Signals::SignalKeyType s_STARTED_PROGRESS_SIG;
    static const core::com::Signals::SignalKeyType s_STOPPED_PROGRESS_SIG;

    /**
     * @brief Constructor
     */
    MODULE_IO_DIMSE_API SSeriesPusher() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_IO_DIMSE_API virtual ~SSeriesPusher() noexcept;

protected:

    /// Does nothing.
    MODULE_IO_DIMSE_API void configuring() override;

    /// Override
    MODULE_IO_DIMSE_API void starting() override;

    /// Override
    MODULE_IO_DIMSE_API void stopping() override;

    /// Override
    MODULE_IO_DIMSE_API void updating() override;

    /// Override
    MODULE_IO_DIMSE_API void info(std::ostream& _sstream) override;

protected:

    /**
     * @brief Check whether some series are already on the PACS
     * @return True if the series must be pushed
     */
    MODULE_IO_DIMSE_API bool checkSeriesOnPACS();

    /// Pull Series
    MODULE_IO_DIMSE_API void pushSeries();

    /**
     * @brief Display a message
     * @param[in] message Message to display
     * @param[in] error True if the message is an error messsage
     */
    MODULE_IO_DIMSE_API void displayMessage(const std::string& message, bool error) const;

    /**
     * @brief Progress callback
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] instanceNumber Instance number
     * @param[in] filePath File path
     */
    MODULE_IO_DIMSE_API void progressCallback(const std::string& seriesInstanceUID, unsigned int instanceNumber,
                                              const std::string& filePath);

    /// Slot to call displayMessage method;
    DisplayMessageSlotType::sptr m_slotDisplayMessage;

    /// Slot to call progressCallback method
    sight::io::dimse::SeriesEnquirer::ProgressCallbackSlotType::sptr m_slotProgressCallback;

    /// Signal emitted when the bar is progressing
    ProgressedSignalType::sptr m_sigProgressed;

    /// Signal emitted when the bar is starting
    StartedProgressSignalType::sptr m_sigStartedProgress;

    /// Signal emitted when the bar is stopping
    StoppedProgressSignalType::sptr m_sigStoppedProgress;

    /// Progress Bar ID
    std::string m_progressbarId;

    /// Series enquirer
    sight::io::dimse::SeriesEnquirer::sptr m_seriesEnquirer;

    /// Pacs Configuration object
    sight::io::dimse::data::PacsConfiguration::csptr m_pacsConfiguration;

    /// Push Worker
    core::thread::Worker::sptr m_pushSeriesWorker;

    /// Is pushing is set to true when we are puishing series
    bool m_isPushing;

    /// Total number of instances that must be uploaded
    long unsigned int m_instanceCount;

};

} // namespace sight::module::io::dimse
