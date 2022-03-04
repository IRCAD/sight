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

#include <data/DicomSeries.hpp>
#include <data/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <io/base/service/IReader.hpp>
#include <io/dimse/data/PacsConfiguration.hpp>
#include <io/dimse/SeriesRetriever.hpp>

#include <service/IController.hpp>
#include <service/IHasServices.hpp>

#include <vector>

namespace sight::module::io::dimse
{

/**
 * @brief This service is used to pull series from a PACS.
 *
 * @section Signals Signals
 * - \b progressed(std::string): sent when the process start (bar id).
 * - \b progressStarted(std::string, float, std::string): sent when the process is updated (bar id,percentage,message).
 * - \b progressStopped(std::string): sent when the process ended (bar id).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::io::dimse::SSeriesPuller">
        <in key="pacsConfig" uid="..." />
        <in key="selectedSeries" uid="..." />
        <inout key="seriesDB" uid="..." />
        <config dicomReader="sight::module::io::dicom::SSeriesDBReader" readerConfig="config" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b pacsConfig [sight::io::dimse::data::PacsConfiguration]: PACS configuration data.
 * - \b selectedSeries [sight::data::Vector]: list of DICOM series to pull from the PACS.
 *
 * @subsection In-Out In-Out:
 * - \b seriesDB [sight::data::SeriesDB]: series DB where to put the retrieved dicom series.
 *
 * @subsection Configuration Configuration:
 * - \b dicomReader (mandatory, string): reader type to use.
 * - \b readerConfig (optional, string, default=""): configuration for the DICOM Reader.
 */
class MODULE_IO_DIMSE_CLASS_API SSeriesPuller final : public service::IController,
                                                      public service::IHasServices
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SSeriesPuller, sight::service::IController);

    /// Creates the service and slots.
    MODULE_IO_DIMSE_API SSeriesPuller() noexcept;

    /// Destroys the service.
    MODULE_IO_DIMSE_API virtual ~SSeriesPuller() noexcept;

protected:

    /// Configures the service.
    MODULE_IO_DIMSE_API void configuring() override;

    /// Creates the DICOM reader.
    MODULE_IO_DIMSE_API void starting() override;

    /// Stops the DICOM reader.
    MODULE_IO_DIMSE_API void stopping() override;

    /// Pulls series.
    MODULE_IO_DIMSE_API void updating() override;

private:

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connects data::SeriesDB::s_REMOVED_SERIES_SIG of s_SERIES_DB_INOUT to s_REMOVE_SERIES_SLOT (removeSeries)
     */
    MODULE_IO_DIMSE_API KeyConnectionsMap getAutoConnections() const override;

    typedef data::SeriesDB::ContainerType DicomSeriesContainerType;
    typedef core::com::Slot<void (DicomSeriesContainerType)> ReadDicomSlotType;
    typedef core::com::Signal<void (std::string)> ProgressStartedSignalType;
    typedef core::com::Signal<void (std::string, float, std::string)> ProgressedSignalType;
    typedef core::com::Signal<void (std::string)> ProgressStoppedSignalType;

    /// Pulls series from the PACS.
    void pullSeries();

    /**
     * @brief Reads local series.
     * @param _selectedSeries DICOM series that must be read.
     */
    void readLocalSeries(DicomSeriesContainerType _selectedSeries);

    /**
     * @brief Stores instance callback.
     * @param _seriesInstanceUID series instance UID.
     * @param _instanceNumber instance number.
     * @param _filePath file path.
     */
    void storeInstanceCallback(
        const std::string& _seriesInstanceUID,
        unsigned _instanceNumber,
        const std::string& _filePath
    );

    ///SLOT: removes series from m_localSeries, when deleted in a gui Selector for instance.
    void removeSeries(data::SeriesDB::ContainerType _removedSeries);

    /// Defines the worker of the series enquire thread.
    core::thread::Worker::sptr m_requestWorker;

    /// Defines the DICOM reader implementation.
    std::string m_dicomReaderImplementation {""};

    /// Contains the optional configuration to set to reader implementation.
    std::string m_readerConfig {""};

    /// Contains the DICOM reader.
    sight::io::base::service::IReader::sptr m_dicomReader {nullptr};

    /// Contains the seriesDB where the DICOM reader sets its output.
    data::SeriesDB::sptr m_seriesDB {nullptr};

    /// Contains the slot to call storeInstanceCallback method using C-MOVE requests.
    sight::io::dimse::SeriesRetriever::ProgressCallbackSlotType::sptr m_slotStoreInstance {nullptr};

    /// Contains the signal emitted when the progress bar is started.
    ProgressStartedSignalType::sptr m_sigProgressStarted {nullptr};

    /// Contains the signal emitted when the progress bar is updated.
    ProgressedSignalType::sptr m_sigProgressed {nullptr};

    /// Contains the signal emitted when the progress bar is stopped.
    ProgressStoppedSignalType::sptr m_sigProgressStopped {nullptr};

    /// Stores local series.
    std::vector<std::string> m_localSeries;

    /// Defines the progress bar ID.
    std::string m_progressbarId {"pullDicomProgressBar"};

    /// Defines the total number of instances that must be downloaded.
    std::size_t m_instanceCount {0};

    /// Stores a map of DICOM series being pulled.
    std::map<std::string, data::DicomSeries::wptr> m_pullingDicomSeriesMap;

    data::ptr<sight::io::dimse::data::PacsConfiguration, data::Access::in> m_config {this, "pacsConfig"};
    data::ptr<sight::data::Vector, data::Access::in> m_selectedSeries {this, "selectedSeries"};

    static constexpr std::string_view s_SERIES_DB_INOUT = "seriesDB";
    data::ptr<sight::data::SeriesDB, data::Access::inout> m_destSeriesDB {this, s_SERIES_DB_INOUT};
};

} // namespace sight::module::io::dimse.
