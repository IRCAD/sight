/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IE_INFORMATIONENTITY_HPP__
#define __FWGDCMIO_WRITER_IE_INFORMATIONENTITY_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"

#include <fwData/Object.hpp>
#include <fwLog/Logger.hpp>

#include <gdcmWriter.h>

#include <cstdint>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @brief InformationEntity base class used to write modules
 */
template< class DATATYPE >
class FWGDCMIO_CLASS_API InformationEntity
{

public:
    typedef std::function< void(std::uint64_t) > ProgressCallback;
    typedef std::function< bool() > CancelRequestedCallback;

    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] object FW4SPL data object
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API InformationEntity(const SPTR(::gdcm::Writer)& writer,
                                   const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                   const SPTR(DATATYPE)& object,
                                   const ::fwLog::Logger::sptr& logger = nullptr,
                                   ProgressCallback progress = nullptr,
                                   CancelRequestedCallback cancel = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~InformationEntity();

protected:

    /// GDCM Writer
    SPTR(::gdcm::Writer) m_writer;

    /// DICOM Instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_instance;

    /// FW4SPL Object
    SPTR(DATATYPE) m_object;

    /// Logger
    ::fwLog::Logger::sptr m_logger;

    /// Progress callback for jobs
    ProgressCallback m_progressCallback;

    /// Cancel information for jobs
    CancelRequestedCallback m_cancelRequestedCallback;
};

//------------------------------------------------------------------------------

template< class DATATYPE >
InformationEntity<DATATYPE>::InformationEntity(const SPTR(::gdcm::Writer)& writer,
                                               const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                               const SPTR(DATATYPE)& object,
                                               const ::fwLog::Logger::sptr& logger,
                                               ProgressCallback progress,
                                               CancelRequestedCallback cancel) :
    m_writer(writer),
    m_instance(instance),
    m_object(object),
    m_logger(logger),
    m_progressCallback(progress),
    m_cancelRequestedCallback(cancel)
{
    SLM_ASSERT("Writer should not be null.", writer);
    SLM_ASSERT("Instance should not be null.", instance);
    SLM_ASSERT("Object should not be null.", object);
}

//------------------------------------------------------------------------------

template< class DATATYPE >
InformationEntity<DATATYPE>::~InformationEntity()
{
}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO

#endif // __FWGDCMIO_WRITER_IE_INFORMATIONENTITY_HPP__
