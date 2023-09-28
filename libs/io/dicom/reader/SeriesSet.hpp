/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/config.hpp"
#include "io/dicom/exception/Failed.hpp"

#include <core/location/multiple_files.hpp>
#include <core/location/single_folder.hpp>
#include <core/log/logger.hpp>

#include <data/DicomSeries.hpp>
#include <data/SeriesSet.hpp>

#include <io/__/reader/GenericObjectReader.hpp>

#include <service/base.hpp>

namespace sight::core::jobs
{

class aggregator;
class base;
class observer;

}

namespace sight::io::dicom
{

namespace reader
{

/**
 * @brief This class adds patient(s) from DICOM file(s) to data::SeriesSet.
 */
class IO_DICOM_CLASS_API SeriesSet : public io::reader::GenericObjectReader<data::SeriesSet>,
                                     public core::location::single_folder,
                                     public core::location::multiple_files,
                                     public core::com::has_signals
{
public:

    SIGHT_DECLARE_CLASS(
        SeriesSet,
        io::reader::GenericObjectReader<data::SeriesSet>,
        io::reader::factory::make<SeriesSet>
    );

    typedef std::vector<SPTR(data::DicomSeries)> DicomSeriesContainerType;
    typedef std::vector<std::string> FilenameContainerType;
    typedef std::vector<std::string> SupportedSOPClassContainerType;

    /// Constructor
    IO_DICOM_API SeriesSet();

    /// Destructor
    IO_DICOM_API ~SeriesSet() override = default;

    /// Reads DICOM data from configured path and fills SeriesSet object
    IO_DICOM_API void read() override;

    /**
     * @brief Reads DICOM data from DicomSeries and fills SeriesSet object
     * @param[in] dicom_series_set SeriesSet containing DicomSeries that must be read
     * @param[in] notifier Service used to notify changes in SeriesSet
     */
    IO_DICOM_API void readFromDicomSeriesSet(
        const data::SeriesSet::csptr& dicom_series_set,
        const sight::service::base::sptr& notifier = sight::service::base::sptr()
    );

    /**
     * @brief Reads DICOM data from configured path and fills SeriesSet object with DicomSeries
     */
    IO_DICOM_API void readDicomSeries();

    /// Return true if a dicomdir file can be read.
    IO_DICOM_API bool isDicomDirAvailable();

    /// Return DicomSeries container
    IO_DICOM_API DicomSeriesContainerType& getDicomSeries();

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
    const core::log::logger::sptr& getLogger() const
    {
        return m_logger;
    }

    ///Set Logger
    void setLogger(const core::log::logger::sptr& logger)
    {
        m_logger = logger;
    }

    /// Getter for reader's job
    IO_DICOM_API SPTR(core::jobs::base) getJob() const override;

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
    void convertDicomSeries(const sight::service::base::sptr& notifier = sight::service::base::sptr());

    /**
     * @brief Function used to sort DicomSeries
     * @param[in] a First DicomSeries
     * @param[in] b Second DicomSeries
     */
    static bool dicomSeriesComparator(
        const SPTR(data::DicomSeries)& a,
        const SPTR(data::DicomSeries)& b
    );

    /// Object Reader Map
    DicomSeriesContainerType m_dicomSeriesContainer;

    /// True if the reader can use the dicomdir file.
    bool m_isDicomdirActivated;

    /// Dicom filter type that must be applied prior to the reading process
    std::string m_dicomFilterType;

    /// Supported SOP Class container
    SupportedSOPClassContainerType m_supportedSOPClassContainer;

    /// Logger
    core::log::logger::sptr m_logger;

    /// Observer managing all subjobs
    SPTR(core::jobs::aggregator) m_job;

    /// Enable buffer rotation
    bool m_enableBufferRotation;

    SPTR(core::jobs::observer) m_dicomdirFileLookupJob;
    SPTR(core::jobs::observer) m_regularFileLookupJob;
    SPTR(core::jobs::observer) m_readerJob;
    SPTR(core::jobs::observer) m_completeDicomSeriesJob;
    SPTR(core::jobs::observer) m_converterJob;
};

} // namespace reader

} // namespace sight::io::dicom
