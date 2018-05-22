/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::namespace::classname">
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
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
 */

class IODICOMWEB_CLASS_API SSeriesPuller : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SSeriesPuller)( ::fwServices::IController ) );

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

    /// Configuring method. This method is used to configure the service.
    IODICOMWEB_API virtual void configuring() override;

    /// Override
    IODICOMWEB_API virtual void starting() override;

    /// Override
    IODICOMWEB_API virtual void stopping() override;

    /// Override
    IODICOMWEB_API void updating() override;

    /// Override
    IODICOMWEB_API void info(std::ostream& _sstream ) override;

private:

    /// Pull Series
    void pullSeries();

    /**
     * @brief Read local series
     * @param[in] selectedSeries Dicom Series that must be read
     */
    void readLocalSeries(DicomSeriesContainerType selectedSeries);

    /**
     * @brief Display an error message
     * @param[in] message Error message to display
     */
    void displayErrorMessage(const std::string& message) const;

    /// Http Qt Client
    ::fwNetworkIO::http::ClientQt m_clientQt;

    /// Reader
    ::fwIO::IReader::sptr m_dicomReader;

    /// Reader Config
    std::string m_dicomReaderSrvConfig;

    /// IODICOMWEB Reader
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
    std::size_t m_instanceCount;

    /// Map of Dicom series being pulled
    DicomSeriesMapType m_pullingDicomSeriesMap;

    /// Returns a well formatted preferences key
    std::string getPreferenceKey(const std::string& key) const;

    /// Server hostname preference key
    std::string m_serverHostnameKey;

    /// Server port preference key
    std::string m_serverPortKey;

    /// Server hostname
    std::string m_serverHostname;

    /// Server port
    int m_serverPort;

    /// DICOM Folder path
    ::boost::filesystem::path m_path;

};

} // namespace ioDicomWeb
