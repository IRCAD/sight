/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IE_INFORMATIONENTITY_HPP__
#define __GDCMIO_READER_IE_INFORMATIONENTITY_HPP__

#include <gdcmReader.h>

#include <fwData/Object.hpp>

#include "gdcmIO/container/DicomInstance.hpp"
#include "gdcmIO/config.hpp"

namespace fwDicomData
{
    class DicomSeries;
}

namespace gdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @class InformationEntity
 * @brief InformationEntity base class used to read modules
 */
template< class DATATYPE >
class GDCMIO_CLASS_API InformationEntity
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] object FW4SPL data object
     */
    GDCMIO_API InformationEntity(
            SPTR(::fwDicomData::DicomSeries) dicomSeries,
            SPTR(::gdcm::Reader) reader,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            SPTR(DATATYPE) object);

    /// Destructor
    GDCMIO_API virtual ~InformationEntity();

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
InformationEntity<DATATYPE>::InformationEntity(
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
InformationEntity<DATATYPE>::~InformationEntity()
{
}

} // namespace ie
} // namespace reader
} // namespace gdcmIO

#endif // __GDCMIO_READER_IE_INFORMATIONENTITY_HPP__
