/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/exception/Failed.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <fwLog/Logger.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwServices/IService.hpp>

namespace fwJobs
{
class Aggregator;
class IJob;
class Observer;
}

namespace fwGdcmIO
{

namespace reader
{

/**
 * @brief This class adds patient(s) from DICOM file(s) to fwData::SeriesDB.
 */
class FWGDCMIO_CLASS_API SeriesDB : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                                    public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader >,
                                    public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader >,
                                    public ::fwCom::HasSignals
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (SeriesDB)( ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB > ),
        (()), ::fwDataIO::reader::factory::New< SeriesDB >);

    typedef std::vector< SPTR(::fwMedData::DicomSeries) > DicomSeriesContainerType;
    typedef std::vector< std::string > FilenameContainerType;
    typedef std::vector< std::string > SupportedSOPClassContainerType;

    /// Constructor
    FWGDCMIO_API SeriesDB(::fwDataIO::reader::IObjectReader::Key key);

    /// Destructor
    FWGDCMIO_API ~SeriesDB();

    /// Reads DICOM data from configured path and fills SeriesDB object
    FWGDCMIO_API void read() override;

    /**
     * @brief Reads DICOM data from DicomSeries and fills SeriesDB object
     * @param[in] dicomSeriesDB SeriesDB containing DicomSeries that must be read
     * @param[in] notifier Service used to notify changes in SeriesDB
     */
    FWGDCMIO_API void readFromDicomSeriesDB(const ::fwMedData::SeriesDB::csptr& dicomSeriesDB,
                                            const ::fwServices::IService::sptr& notifier
                                                = ::fwServices::IService::sptr());

    /**
     * @brief Reads DICOM data from configured path and fills SeriesDB object with DicomSeries
     */
    FWGDCMIO_API void readDicomSeries();

    /// Return true if a dicomdir file can be read.
    FWGDCMIO_API bool isDicomDirAvailable();

    /// Return DicomSeries container
    FWGDCMIO_API DicomSeriesContainerType& getDicomSeries();

    /// Get Set whether the reader must use the dicomdir file or not
    const bool& getDicomdirActivated() const
    {
        return m_isDicomdirActivated;
    }

    ///Set Set whether the reader must use the dicomdir file or not
    void setDicomdirActivated(const bool& isDicomdirActivated)
    {
        m_isDicomdirActivated = isDicomdirActivated;
    }

    /// Get Dicom filter type that must be applied prior to the reading process
    const std::string& getDicomFilterType() const
    {
        return m_dicomFilterType;
    }

    ///Set Dicom filter type that must be applied prior to the reading process
    void setDicomFilterType(const std::string& dicomFilterType)
    {
        m_dicomFilterType = dicomFilterType;
    }

    /// Get Supported SOP Class
    const SupportedSOPClassContainerType& getSupportedSOPClassContainer() const
    {
        return m_supportedSOPClassContainer;
    }

    ///Set Supported SOP Class
    void setSupportedSOPClassContainer(const SupportedSOPClassContainerType& supportedSOPClassContainer)
    {
        m_supportedSOPClassContainer = supportedSOPClassContainer;
    }

    /// Get Logger
    const ::fwLog::Logger::sptr& getLogger() const
    {
        return m_logger;
    }

    ///Set Logger
    void setLogger(const ::fwLog::Logger::sptr& logger)
    {
        m_logger = logger;
    }

    /// Getter for reader's job
    FWGDCMIO_API SPTR(::fwJobs::IJob) getJob() const override;

    /// Enable buffer rotation
    void setBufferRotationEnabled(bool enabled)
    {
        m_enableBufferRotation = enabled;
    }

private:

    /**
     * @brief Read DICOM series
     */
    void readDicom();

    /**
     * @brief Convert DicomSeries to Image or Model Series
     * @param[in] dicomSeries Dicom Series that must be converted
     */
    void convertDicomSeries(const ::fwServices::IService::sptr& notifier = ::fwServices::IService::sptr());

    /**
     * @brief Function used to sort DicomSeries
     * @param[in] a First DicomSeries
     * @param[in] b Second DicomSeries
     */
    static bool dicomSeriesComparator(const SPTR(::fwMedData::DicomSeries)& a,
                                      const SPTR(::fwMedData::DicomSeries)& b);

    /// Object Reader Map
    DicomSeriesContainerType m_dicomSeriesContainer;

    /// True if the reader can use the dicomdir file.
    bool m_isDicomdirActivated;

    /// Dicom filter type that must be applied prior to the reading process
    std::string m_dicomFilterType;

    /// Supported SOP Class container
    SupportedSOPClassContainerType m_supportedSOPClassContainer;

    /// Logger
    ::fwLog::Logger::sptr m_logger;

    /// Observer managing all subjobs
    SPTR(::fwJobs::Aggregator) m_job;

    /// Enable buffer rotation
    bool m_enableBufferRotation;

    SPTR(::fwJobs::Observer) m_dicomdirFileLookupJob;
    SPTR(::fwJobs::Observer) m_regularFileLookupJob;
    SPTR(::fwJobs::Observer) m_readerJob;
    SPTR(::fwJobs::Observer) m_completeDicomSeriesJob;
    SPTR(::fwJobs::Observer) m_converterJob;
};

}  // namespace reader

}  // namespace fwGdcmIO
