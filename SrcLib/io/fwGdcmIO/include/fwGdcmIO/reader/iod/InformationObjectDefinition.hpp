/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
#define __FWGDCMIO_READER_IOD_INFORMATIONOBJECTDEFINITION_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"
#include "fwGdcmIO/exception/Failed.hpp"

#include <fwMedData/DicomSeries.hpp>
#include <fwLog/Logger.hpp>
#include <fwMedData/Series.hpp>

#include <gdcmReader.h>

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @brief InformationObjectDefinition base class used to read DICOM IODs
 */
class FWGDCMIO_CLASS_API InformationObjectDefinition
{

public:

    typedef std::function< void (unsigned int&) > ProgressCallback;

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     * @param[in] logger Logger
     * @param[in] callback Progress callback
     * @param[in] cancelled cancel information
     */
    FWGDCMIO_API InformationObjectDefinition(::fwMedData::DicomSeries::sptr dicomSeries,
                                             SPTR(::fwGdcmIO::container::DicomInstance)instance,
                                             ::fwLog::Logger::sptr logger,
                                             ProgressCallback& callback, bool& cancelled);

    /// Destructor
    FWGDCMIO_API virtual ~InformationObjectDefinition();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     */
    FWGDCMIO_API virtual void read(::fwMedData::Series::sptr series) = 0;

protected:

    /// DICOM Instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_instance;

    /// Dicom Series
    ::fwMedData::DicomSeries::sptr m_dicomSeries;

    ///Logger
    ::fwLog::Logger::sptr m_logger;

    /// Progress callback for jobs
    ProgressCallback& m_progressCallback;

    /// Cancel information for jobs
    const bool& m_cancelled;

};

} // namespace iod
} // namespace reader
} // namespace fwGdcmIO

#endif // __FWGDCMIO_READER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
