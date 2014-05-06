/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_TID_TEMPLATEID_HPP__
#define __GDCMIO_READER_TID_TEMPLATEID_HPP__

#include <gdcmReader.h>

#include <fwData/Object.hpp>

#include "gdcmIO/container/DicomInstance.hpp"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace tid
{

/**
 * @class TemplateID
 * @brief TemplateID base class used to write templates
 */
template< class DATATYPE >
class GDCMIO_CLASS_API TemplateID
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] object FW4SPL data object
     */
    GDCMIO_API TemplateID(
            SPTR(::fwDicomData::DicomSeries) dicomSeries,
            SPTR(::gdcm::Reader) reader,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            SPTR(DATATYPE) object);

    /// Destructor
    GDCMIO_API virtual ~TemplateID();

protected:

    /// Dicom Series
    SPTR(::fwDicomData::DicomSeries) m_dicomSeries;

    /// GDCM Reader
    SPTR(::gdcm::Reader) m_reader;

    /// DICOM Instance
    SPTR(::gdcmIO::container::DicomInstance) m_instance;

    /// FW4SPL Object
    SPTR(DATATYPE) m_object;
};

//------------------------------------------------------------------------------

template< class DATATYPE >
TemplateID<DATATYPE>::TemplateID(
        SPTR(::fwDicomData::DicomSeries) dicomSeries,
        SPTR(::gdcm::Reader) reader,
        SPTR(::gdcmIO::container::DicomInstance) instance, SPTR(DATATYPE) object):
        m_dicomSeries(dicomSeries), m_reader(reader), m_instance(instance), m_object(object)
{
    SLM_ASSERT("DicomSeries should not be null.", dicomSeries);
    SLM_ASSERT("Reader should not be null.", reader);
    SLM_ASSERT("Instance should not be null.", instance);
    SLM_ASSERT("Object should not be null.", object);
}

//------------------------------------------------------------------------------

template< class DATATYPE >
TemplateID<DATATYPE>::~TemplateID()
{
}

} // namespace tid
} // namespace reader
} // namespace gdcmIO

#endif // __GDCMIO_READER_TID_TEMPLATEID_HPP__
