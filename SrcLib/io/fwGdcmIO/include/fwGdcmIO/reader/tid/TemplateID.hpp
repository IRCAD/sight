/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_TID_TEMPLATEID_HPP__
#define __FWGDCMIO_READER_TID_TEMPLATEID_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"

#include <fwData/Object.hpp>
#include <fwLog/Logger.hpp>

#include <gdcmReader.h>

namespace fwGdcmIO
{
namespace reader
{
namespace tid
{

/**
 * @brief TemplateID base class used to write templates
 */
template< class DATATYPE >
class FWGDCMIO_CLASS_API TemplateID
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] object FW4SPL data object
     * @param[in] logger Logger
     */
    FWGDCMIO_API TemplateID(
        SPTR(::fwMedData::DicomSeries)dicomSeries,
        SPTR(::gdcm::Reader)reader,
        SPTR(::fwGdcmIO::container::DicomInstance)instance,
        SPTR(DATATYPE)object,
        ::fwLog::Logger::sptr logger);

    /// Destructor
    FWGDCMIO_API virtual ~TemplateID();

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
};

//------------------------------------------------------------------------------

template< class DATATYPE >
TemplateID<DATATYPE>::TemplateID(
    SPTR(::fwMedData::DicomSeries)dicomSeries,
    SPTR(::gdcm::Reader)reader,
    SPTR(::fwGdcmIO::container::DicomInstance)instance, SPTR(DATATYPE)object, ::fwLog::Logger::sptr logger) :
    m_dicomSeries(dicomSeries), m_reader(reader), m_instance(instance), m_object(object), m_logger(logger)
{
    SLM_ASSERT("DicomSeries should not be null.", dicomSeries);
    SLM_ASSERT("Reader should not be null.", reader);
    SLM_ASSERT("Instance should not be null.", instance);
    SLM_ASSERT("Object should not be null.", object);
    SLM_ASSERT("Logger should not be null.", logger);
}

//------------------------------------------------------------------------------

template< class DATATYPE >
TemplateID<DATATYPE>::~TemplateID()
{
}

} // namespace tid
} // namespace reader
} // namespace fwGdcmIO

#endif // __FWGDCMIO_READER_TID_TEMPLATEID_HPP__
