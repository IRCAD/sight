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

#include <gdcmWriter.h>

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
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] object FW4SPL data object
     */
    FWGDCMIO_API InformationEntity(
        SPTR(::gdcm::Writer)writer,
        SPTR(::fwGdcmIO::container::DicomInstance)instance,
        SPTR(DATATYPE)object);

    /// Destructor
    FWGDCMIO_API virtual ~InformationEntity();

protected:

    /// GDCM Writer
    SPTR(::gdcm::Writer) m_writer;

    /// DICOM Instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_instance;

    /// FW4SPL Object
    SPTR(DATATYPE) m_object;
};

//------------------------------------------------------------------------------

template< class DATATYPE >
InformationEntity<DATATYPE>::InformationEntity(SPTR(::gdcm::Writer)writer,
                                               SPTR(::fwGdcmIO::container::DicomInstance)instance,
                                               SPTR(DATATYPE)object) :
    m_writer(writer), m_instance(instance), m_object(object)
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

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO

#endif // __FWGDCMIO_WRITER_IE_INFORMATIONENTITY_HPP__
