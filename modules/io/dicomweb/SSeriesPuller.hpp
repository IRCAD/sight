/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/io/dicomweb/config.hpp"

#include <data/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <io/base/service/IReader.hpp>
#include <io/http/ClientQt.hpp>

#include <service/IController.hpp>

#include <filesystem>

namespace sight::data
{

class DicomSeries;

}

namespace sight::module::io::dicomweb
{

/**
 * @brief   This service is used to pull series from a PACS (Orthanc).
 *

 * @section Slots Slots
 * - \b displayErrorMessage(const std::string&) : display an error message.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::dicomweb::SSeriesPuller">
            <in key="selectedSeries" uid="..." />
            <inout key="seriesDB" uid="..." />
            <config dicomReader="sight::module::io::dicom::SSeriesDBReader" readerConfig="config" />
            <server>%SERVER_HOSTNAME%:%SERVER_PORT%</server>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selectedSeries [sight::data::Vector]: List of DICOM series to pull from the PACS..
 * @subsection In-Out In-Out:
 * - \b seriesDB [sight::data::SeriesDB]: SeriesDB where to put the retrieved dicom series.
 * @subsection Configuration Configuration:
 * - \b readerConfig Optional configuration for the DICOM Reader.
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service are from the preference settings.
 */

class MODULE_IO_DICOMWEB_CLASS_API SSeriesPuller : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SSeriesPuller, sight::service::IController);

    typedef data::SeriesDB::ContainerType DicomSeriesContainerType;
    typedef std::vector<std::string> InstanceUIDContainerType;
    typedef std::map<std::string, unsigned int> InstanceCountMapType;
    typedef std::map<std::string, WPTR(data::DicomSeries)> DicomSeriesMapType;

    /**
     * @brief Constructor
     */
    MODULE_IO_DICOMWEB_API SSeriesPuller() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_IO_DICOMWEB_API virtual ~SSeriesPuller() noexcept;

protected:

    /// Gets the configuration.
    MODULE_IO_DICOMWEB_API void configuring() override;

    /// Registers the DICOM reader.
    MODULE_IO_DICOMWEB_API void starting() override;

    /// Stops the DICOM reader.
    MODULE_IO_DICOMWEB_API void stopping() override;

    /// Checks the configuration and pull the series.
    MODULE_IO_DICOMWEB_API void updating() override;

private:

    /// Pull the Series from the Pacs.
    void pullSeries();

    /**
     * @brief Read local series.
     * @param[in] selectedSeries Series to read
     */
    void readLocalSeries(DicomSeriesContainerType selectedSeries);

    /**
     * @brief Display an error message.
     * @param[in] message Error message to display
     */
    void displayErrorMessage(const std::string& message) const;

    /// Http Qt Client
    sight::io::http::ClientQt m_clientQt;

    /// Reader
    sight::io::base::service::IReader::sptr m_dicomReader;

    /// Reader Config
    std::string m_dicomReaderSrvConfig;

    /// DicomWeb Reader
    std::string m_dicomReaderType;

    /// Temporary SeriesDB
    data::SeriesDB::sptr m_tempSeriesDB;

    /// Local Series
    InstanceUIDContainerType m_localSeries;

    /// Is pulling is set to true when we are pulling series
    bool m_isPulling;

    /// Index of the series being downloaded
    unsigned int m_seriesIndex;

    /// Total number of instances that must be downloaded
    std::size_t m_instanceCount;

    /// Map of Dicom series being pulled
    DicomSeriesMapType m_pullingDicomSeriesMap;

    /// Server hostname preference key
    std::string m_serverHostnameKey;

    /// Server port preference key
    std::string m_serverPortKey;

    /// Server hostname
    std::string m_serverHostname {"localhost"};

    /// Server port
    int m_serverPort {4242};

    /// DICOM Folder path
    std::filesystem::path m_path;

    sight::data::ptr<sight::data::Vector, sight::data::Access::in> m_selectedSeries {this, "selectedSeries"};
    sight::data::ptr<sight::data::SeriesDB, sight::data::Access::inout> m_seriesDB {this, "seriesDB"};
};

} // namespace sight::module::io::dicomweb
