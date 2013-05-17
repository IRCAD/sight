/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_DICOM_IDICOMOBJECT_HPP__
#define __GDCMIO_READER_DICOM_IDICOMOBJECT_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <fwCore/spyLog.hpp>        // for SLM_TRACE_FUNC()

#include <fwTools/Object.hpp>

#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

/**
 * @brief   This class allows to set an fwData::Object for a DICOM reading.
 *
 * @note    Inspired from ::fwDataIO::reader::GenericObjectReader
 *
 * @class   IDicomObject
 */
template <class DATATYPE>
class GDCMIO_CLASS_API IDicomObject
{

public:

    IDicomObject()
    {}

    virtual ~IDicomObject()
    {}

    /**
     * @brief   Interface which has to be overridden.
     */
    GDCMIO_API virtual void read() = 0;

    /**
     * @brief m_object setter.
     * @param[out] a_obj replace weakptr m_object of the instance reader
     *
     * This method overload setObject to ensure that given object correspond to DATATYPE
     */
    GDCMIO_API void setObject( ::fwTools::Object::sptr a_obj);

    /**
     * @brief m_object getter.
     * @return m_object
     *
     * This method automatic cast object in correct DataType.
     */
    GDCMIO_API ::boost::shared_ptr< DATATYPE > getConcreteObject() const;

protected :

    /**
     * @brief Object read on filesystem by the process.
     *
     * This object is given in parameter of setObject method but it is conserved with a weakptr.
     */
    ::fwTools::Object::wptr m_object;
};

//------------------------------------------------------------------------------

template <class DATATYPE>
void IDicomObject<DATATYPE>::setObject(::fwTools::Object::sptr a_obj)
{
    SLM_ASSERT("DynamicPointerCast failed.", ::boost::dynamic_pointer_cast< DATATYPE >( a_obj ) );
    m_object = a_obj;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
::boost::shared_ptr< DATATYPE > IDicomObject<DATATYPE>::getConcreteObject() const
{
    SLM_ASSERT("Object expired.", !m_object.expired() );
    return ::boost::dynamic_pointer_cast< DATATYPE >( m_object.lock() );
}

} // namespace dicom
} // namespace reader
} // namespace gdcmIO

#endif /* __GDCMIO_READER_DICOM_IDICOMOBJECT_HPP__ */
