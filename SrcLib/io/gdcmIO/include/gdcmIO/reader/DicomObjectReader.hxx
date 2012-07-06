/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMOBJECTREADER_HPP_
#define _GDCMIO_DICOMOBJECTREADER_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <fwCore/spyLog.hpp>        // for SLM_TRACE_FUNC()

#include <fwTools/Object.hpp>

#include "gdcmIO/config.hpp"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class allows to set an fwData::Object for a DICOM reading.
 *
 * @note    Inspired from ::fwDataIO::reader::GenericObjectReader
 *
 * @class   DicomObjectReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
template <class DATATYPE>
class GDCMIO_CLASS_API DicomObjectReader
{

public:

    GDCMIO_API DicomObjectReader()
    {
        SLM_TRACE_FUNC();
    }

    GDCMIO_API virtual ~DicomObjectReader()
    {
        SLM_TRACE_FUNC();
    }

    /**
     * @brief   Interface which has to be overridden.
     */
    GDCMIO_API virtual void read() = 0;

    /**
     * @brief m_object setter.
     * @param[out] _pObject replace weakptr m_object of the instance reader
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
    ::fwTools::Object::wptr             m_object;
};

//------------------------------------------------------------------------------

template <class DATATYPE>
void DicomObjectReader<DATATYPE>::setObject( ::fwTools::Object::sptr a_obj)
{
    assert ( ::boost::dynamic_pointer_cast< DATATYPE >( a_obj ) );
    m_object = a_obj;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
::boost::shared_ptr< DATATYPE > DicomObjectReader<DATATYPE>::getConcreteObject() const
{
    assert( !m_object.expired() );
    return ::boost::dynamic_pointer_cast< DATATYPE >( m_object.lock() );
}

}   // namespace reader

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMOBJECTREADER_HPP_ */
