/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
#define __FWGDCMIO_READER_IOD_INFORMATIONOBJECTDEFINITION_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"
#include "fwGdcmIO/exception/Failed.hpp"

#include <fwLog/Logger.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <gdcmReader.h>

#include <cstdint>

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

    typedef std::function< void (std::uint64_t) > ProgressCallback;
    typedef std::function< bool () > CancelRequestedCallback;

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API InformationObjectDefinition(const ::fwMedData::DicomSeries::sptr& dicomSeries,
                                             const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                             const ::fwLog::Logger::sptr& logger = nullptr,
                                             ProgressCallback progress = nullptr,
                                             CancelRequestedCallback cancel = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~InformationObjectDefinition();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     */
    FWGDCMIO_API virtual void read(::fwMedData::Series::sptr series) throw(::fwGdcmIO::exception::Failed) = 0;

protected:

    /// DICOM Instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_instance;

    /// Dicom Series
    ::fwMedData::DicomSeries::sptr m_dicomSeries;

    ///Logger
    ::fwLog::Logger::sptr m_logger;

    /// Progress callback for jobs
    ProgressCallback m_progressCallback;

    /// Cancel information for jobs
    CancelRequestedCallback m_cancelRequestedCallback;

};

} // namespace iod
} // namespace reader
} // namespace fwGdcmIO

#endif // __FWGDCMIO_READER_IOD_INFORMATIONOBJECTDEFINITION_HPP__
