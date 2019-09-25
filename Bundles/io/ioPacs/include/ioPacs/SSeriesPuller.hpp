/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "ioPacs/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwIO/IReader.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwPacsIO/SeriesEnquirer.hpp>
#include <fwPacsIO/SeriesRetriever.hpp>

#include <fwServices/IController.hpp>

#include <fwThread/Worker.hpp>

#include <boost/filesystem/path.hpp>

#include <vector>

namespace fwMedData
{
class DicomSeries;
}

namespace ioPacs
{

/**
 * @brief   This service is used to pull series from a PACS.
 *
 * @section Signals Signals
 * - \b progressed(std::string) : Signal to start the progress (bar id).
 * - \b startedProgress(std::string, float, std::string) :  Signal to update the progress (bar id, percentage, message).
 * - \b stoppedProgress(std::string) : Signal to stop the progress (bar id).

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::namespace::classname">
            <in key="pacsConfig" uid="..." />
            <in key="selectedSeries" uid="..." />
            <inout key="seriesDB" uid="..." />
            <config dicomReader="::ioGdcm::SSeriesDBReader" dicomReaderConfig="config" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b pacsConfig [::fwPacsIO::data::PacsConfiguration]: PACS configuration data.
 * - \b selectedSeries [::fwData::Vector]: List of DICOM series to pull from the PACS..
 * @subsection In-Out In-Out:
 * - \b seriesDB [::fwMedData::SeriesDB]: SeriesDB where to put the retrieved dicom series.
 * @subsection Configuration Configuration:
 * - \b dicomReader Reader type to use.
 * - \b dicomReaderConfig Optional configuration for the DICOM Reader.
 */

class IOPACS_CLASS_API SSeriesPuller : public ::fwServices::IController
{
public:

    fwCoreServiceMacro(SSeriesPuller,  ::fwServices::IController );

    typedef ::fwMedData::SeriesDB::ContainerType DicomSeriesContainerType;
    typedef std::vector< std::string > InstanceUIDContainerType;
    typedef std::map < std::string, unsigned int > InstanceCountMapType;
    typedef std::map < std::string, WPTR(::fwMedData::DicomSeries) > DicomSeriesMapType;

    IOPACS_API static const ::fwCom::Slots::SlotKeyType s_READ_SLOT;
    typedef ::fwCom::Slot<void (DicomSeriesContainerType)> ReadDicomSlotType;

    IOPACS_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_SLOT;
    typedef ::fwCom::Slot<void (const std::string&)> DisplayMessageSlotType;

    /// Signal to start the progress (bar id)
    typedef ::fwCom::Signal< void ( std::string ) > StartedProgressSignalType;
    /// Signal to update the progress (bar id, percentage, message)
    typedef ::fwCom::Signal< void ( std::string, float, std::string ) > ProgressedSignalType;
    /// Signal to stop the progress (bar id)
    typedef ::fwCom::Signal< void ( std::string ) > StoppedProgressSignalType;

    /// Key in m_signals map of signal m_sigProgressed
    static const ::fwCom::Signals::SignalKeyType s_PROGRESSED_SIG;
    static const ::fwCom::Signals::SignalKeyType s_STARTED_PROGRESS_SIG;
    static const ::fwCom::Signals::SignalKeyType s_STOPPED_PROGRESS_SIG;

    /**
     * @brief Constructor
     */
    IOPACS_API SSeriesPuller() noexcept;

    /**
     * @brief Destructor
     */
    IOPACS_API virtual ~SSeriesPuller() noexcept;

protected:

    /// Configuring method. This method is used to configure the service.
    IOPACS_API virtual void configuring() override;

    /// Override
    IOPACS_API virtual void starting() override;

    /// Override
    IOPACS_API virtual void stopping() override;

    /// Override
    IOPACS_API void updating() override;

    /// Override
    IOPACS_API void info(std::ostream& _sstream ) override;

protected:

    /// Pull Series
    IOPACS_API void pullSeries();

    /**
     * @brief Read local series
     * @param[in] selectedSeries Dicom Series that must be read
     */
    IOPACS_API void readLocalSeries(DicomSeriesContainerType selectedSeries);

    /**
     * @brief Display an error message
     * @param[in] message Error message to display
     */
    IOPACS_API void displayErrorMessage(const std::string& message) const;

    /**
     * @brief Store instance callback
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] instanceNumber Instance number
     * @param[in] filePath File path
     */
    IOPACS_API void storeInstanceCallback(const std::string& seriesInstanceUID, unsigned int instanceNumber,
                                          const std::string& filePath);

    /// Slot to call readLocalSeries method
    ReadDicomSlotType::sptr m_slotReadLocalSeries;

    /// Slot to call displayErrorMessage method;
    DisplayMessageSlotType::sptr m_slotDisplayMessage;

    /// Slot to call storeInstanceCallback method using C-MOVE Requests
    ::fwPacsIO::SeriesRetriever::ProgressCallbackSlotType::sptr m_slotStoreInstanceCallbackUsingMoveRequests;

    /// Slot to call storeInstanceCallback method using C-GET Requests
    ::fwPacsIO::SeriesEnquirer::ProgressCallbackSlotType::sptr m_slotStoreInstanceCallbackUsingGetRequests;

    /// Signal emitted when the bar is progressing
    ProgressedSignalType::sptr m_sigProgressed;

    /// Signal emitted when the bar is starting
    StartedProgressSignalType::sptr m_sigStartedProgress;

    /// Signal emitted when the bar is stopping
    StoppedProgressSignalType::sptr m_sigStoppedProgress;

    /// Series enquirer
    ::fwPacsIO::SeriesEnquirer::sptr m_seriesEnquirer;

    /// Pacs Configuration object
    ::fwPacsIO::data::PacsConfiguration::csptr m_pacsConfiguration;

    /// Reader
    ::fwIO::IReader::sptr m_dicomReader;

    /// Reader Config
    std::string m_dicomReaderSrvConfig;

    /// IOPACS Reader
    std::string m_dicomReaderType;

    /// Temporary SeriesDB
    ::fwMedData::SeriesDB::sptr m_tempSeriesDB;

    /// Destination SeriesDB
    ::fwMedData::SeriesDB::sptr m_destinationSeriesDB;

    /// Pull Worker
    ::fwThread::Worker::sptr m_pullSeriesWorker;

    /// Local Series
    InstanceUIDContainerType m_localSeries;

    /// Is pulling is set to true when we are pulling series
    bool m_isPulling;

    /// Progress Bar ID
    std::string m_progressbarId;

    /// Total number of downloaded series
    long unsigned int m_seriesCount;

    /// Index of the series being downloaded
    unsigned int m_seriesIndex;

    /// Total number of instances that must be downloaded
    std::size_t m_instanceCount;

    /// Map of Dicom series being pulled
    DicomSeriesMapType m_pullingDicomSeriesMap;
};

} // namespace ioPacs
