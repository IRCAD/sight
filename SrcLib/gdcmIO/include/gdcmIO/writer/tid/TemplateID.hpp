/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_TID_TEMPLATEID_HPP__
#define __GDCMIO_WRITER_TID_TEMPLATEID_HPP__

#include <gdcmWriter.h>

#include <fwData/Object.hpp>

#include "gdcmIO/container/DicomInstance.hpp"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace writer
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
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] object FW4SPL data object
     */
    GDCMIO_API TemplateID(
        SPTR(::gdcm::Writer)writer,
        SPTR(::gdcmIO::container::DicomInstance)instance,
        SPTR(DATATYPE)object);

    /// Destructor
    GDCMIO_API virtual ~TemplateID();

protected:

    /// GDCM Writer
    SPTR(::gdcm::Writer) m_writer;

    /// DICOM Instance
    SPTR(::gdcmIO::container::DicomInstance) m_instance;

    /// FW4SPL Object
    SPTR(DATATYPE) m_object;
};

//------------------------------------------------------------------------------

template< class DATATYPE >
TemplateID<DATATYPE>::TemplateID(SPTR(::gdcm::Writer)writer,
                                 SPTR(::gdcmIO::container::DicomInstance)instance, SPTR(DATATYPE)object) :
    m_writer(writer), m_instance(instance), m_object(object)
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

} // namespace tid
} // namespace writer
} // namespace gdcmIO

#endif // __GDCMIO_WRITER_TID_TEMPLATEID_HPP__
