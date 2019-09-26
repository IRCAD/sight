/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
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

#include "ioDicomWeb/config.hpp"

#include <fwIO/IReader.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwNetworkIO/http/ClientQt.hpp>

#include <fwServices/IController.hpp>

#include <boost/filesystem/path.hpp>

namespace fwMedData
{
class DicomSeries;
}

namespace ioDicomWeb
{

/**
 * @brief   This service is used to pull series from a PACS (Orthanc).
 *

 * @section Slots Slots
 * - \b displayErrorMessage(const std::string&) : display an error message.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioDicomWeb::SSeriesPuller">
            <in key="selectedSeries" uid="..." />
            <inout key="seriesDB" uid="..." />
            <config dicomReader="::ioGdcm::SSeriesDBReader" dicomReaderConfig="config" />
            <server>%SERVER_HOSTNAME%:%SERVER_PORT%</server>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selectedSeries [::fwData::Vector]: List of DICOM series to pull from the PACS..
 * @subsection In-Out In-Out:
 * - \b seriesDB [::fwMedData::SeriesDB]: SeriesDB where to put the retrieved dicom series.
 * @subsection Configuration Configuration:
 * - \b dicomReaderConfig Optional configuration for the DICOM Reader.
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service are from the preference settings.
 */

class IODICOMWEB_CLASS_API SSeriesPuller : public ::fwServices::IController
{
public:

    fwCoreServiceMacro(SSeriesPuller,  ::fwServices::IController );

    typedef ::fwMedData::SeriesDB::ContainerType DicomSeriesContainerType;
    typedef std::vector< std::string > InstanceUIDContainerType;
    typedef std::map < std::string, unsigned int > InstanceCountMapType;
    typedef std::map < std::string, WPTR(::fwMedData::DicomSeries) > DicomSeriesMapType;

    /**
     * @brief Constructor
     */
    IODICOMWEB_API SSeriesPuller() noexcept;

    /**
     * @brief Destructor
     */
    IODICOMWEB_API virtual ~SSeriesPuller() noexcept;

protected:

    /// Gets the configuration.
    IODICOMWEB_API virtual void configuring() override;

    /// Registers the DICOM reader.
    IODICOMWEB_API virtual void starting() override;

    /// Stops the DICOM reader.
    IODICOMWEB_API virtual void stopping() override;

    /// Checks the configuration and pull the series.
    IODICOMWEB_API void updating() override;

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
    ::fwNetworkIO::http::ClientQt m_clientQt;

    /// Reader
    ::fwIO::IReader::sptr m_dicomReader;

    /// Reader Config
    std::string m_dicomReaderSrvConfig;

    /// DicomWeb Reader
    std::string m_dicomReaderType;

    /// Temporary SeriesDB
    ::fwMedData::SeriesDB::sptr m_tempSeriesDB;

    /// Destination SeriesDB
    ::fwMedData::SeriesDB::sptr m_destinationSeriesDB;

    /// Local Series
    InstanceUIDContainerType m_localSeries;

    /// Is pulling is set to true when we are pulling series
    bool m_isPulling;

    /// Index of the series being downloaded
    unsigned int m_seriesIndex;

    /// Total number of instances that must be downloaded
    size_t m_instanceCount;

    /// Map of Dicom series being pulled
    DicomSeriesMapType m_pullingDicomSeriesMap;

    /// Server hostname preference key
    std::string m_serverHostnameKey;

    /// Server port preference key
    std::string m_serverPortKey;

    /// Server hostname
    std::string m_serverHostname{"localhost"};

    /// Server port
    int m_serverPort{4242};

    /// DICOM Folder path
    ::boost::filesystem::path m_path;

};

} // namespace ioDicomWeb
