/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_TID_TEMPLATEID_HPP__
#define __FWGDCMIO_WRITER_TID_TEMPLATEID_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"

#include <fwData/Object.hpp>

#include <gdcmWriter.h>

namespace fwGdcmIO
{
namespace writer
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
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] object FW4SPL data object
     */
    FWGDCMIO_API TemplateID(const SPTR(::gdcm::Writer)& writer,
                            const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                            const SPTR(DATATYPE)& object);

    /// Destructor
    FWGDCMIO_API virtual ~TemplateID();

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
TemplateID<DATATYPE>::TemplateID(const SPTR(::gdcm::Writer)& writer,
                                 const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                 const SPTR(DATATYPE)& object) :
    m_writer(writer),
    m_instance(instance),
    m_object(object)
{
    SLM_ASSERT("Writer should not be null.", writer);
    SLM_ASSERT("Instance should not be null.", instance);
    SLM_ASSERT("Object should not be null.", object);
}

//------------------------------------------------------------------------------

template< class DATATYPE >
TemplateID<DATATYPE>::~TemplateID()
{
}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace writer
} // namespace fwGdcmIO

#endif // __FWGDCMIO_WRITER_TID_TEMPLATEID_HPP__
