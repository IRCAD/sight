/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IE_INFORMATIONENTITY_HPP__
#define __FWGDCMIO_READER_IE_INFORMATIONENTITY_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"

#include <fwData/Object.hpp>
#include <fwLog/Logger.hpp>

#include <gdcmReader.h>

#include <cstdint>

namespace fwMedData
{
class DicomSeries;
}

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @brief InformationEntity base class used to read modules
 */
template< class DATATYPE >
class FWGDCMIO_CLASS_API InformationEntity
{

public:

    typedef std::function< void(std::uint64_t) > ProgressCallback;
    typedef std::function< bool() > CancelRequestedCallback;

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] object FW4SPL data object
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API InformationEntity(const SPTR(::fwMedData::DicomSeries)& dicomSeries,
                                   const SPTR(::gdcm::Reader)& reader,
                                   const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                   const SPTR(DATATYPE)& object,
                                   const ::fwLog::Logger::sptr& logger = nullptr,
                                   const ProgressCallback progress = nullptr,
                                   CancelRequestedCallback cancel = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~InformationEntity();

protected:

    /// Dicom Series
    SPTR(::fwMedData::DicomSeries) m_dicomSeries;

    /// GDCM Reader
    SPTR(::gdcm::Reader) m_reader;

    /// DICOM Instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_instance;

    /// FW4SPL Object
    SPTR(DATATYPE) m_object;

    ///Logger
    ::fwLog::Logger::sptr m_logger;

    /// Progress callback for jobs
    ProgressCallback m_progressCallback;

    /// Cancel information for jobs
    CancelRequestedCallback m_cancelRequestedCallback;
};

//------------------------------------------------------------------------------

template< class DATATYPE >
InformationEntity<DATATYPE>::InformationEntity(const SPTR(::fwMedData::DicomSeries)& dicomSeries,
                                               const SPTR(::gdcm::Reader)& reader,
                                               const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                               const SPTR(DATATYPE)& object,
                                               const ::fwLog::Logger::sptr& logger,
                                               ProgressCallback progress,
                                               CancelRequestedCallback cancel):
    m_dicomSeries(dicomSeries),
    m_reader(reader),
    m_instance(instance),
    m_object(object),
    m_logger(logger),
    m_progressCallback(progress),
    m_cancelRequestedCallback(cancel)
{
    SLM_ASSERT("DicomSeries should not be null.", dicomSeries);
    SLM_ASSERT("Reader should not be null.", reader);
    SLM_ASSERT("Instance should not be null.", instance);
    SLM_ASSERT("Object should not be null.", object);
    SLM_ASSERT("Logger should not be null.", logger);
}

//------------------------------------------------------------------------------

template< class DATATYPE >
InformationEntity<DATATYPE>::~InformationEntity()
{
}

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO

#endif // __FWGDCMIO_READER_IE_INFORMATIONENTITY_HPP__
