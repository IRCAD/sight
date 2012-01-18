/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMMODULEWRITER_HXX_
#define _GDCMIO_DICOMMODULEWRITER_HXX_

#include <boost/shared_ptr.hpp>

#include <gdcmDataSet.h>

#include <fwCore/spyLog.hpp>        // for SLM_TRACE_FUNC()

#include <fwTools/Object.hpp>

#include "gdcmIO/config.hpp"
#include "gdcmIO/DicomInstance.hpp"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class contains common members for DICOM module writers.
 *
 * It allows to set a gdcm::DataSet from fwData::Object.
 *
 * @class   DicomModuleWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
template <class DATATYPE>
class GDCMIO_CLASS_API DicomModuleWriter
{

public:

    GDCMIO_API DicomModuleWriter()
    {
        SLM_TRACE_FUNC();
    }

    GDCMIO_API virtual ~DicomModuleWriter()
    {
        SLM_TRACE_FUNC();
    }


    GDCMIO_API ::boost::shared_ptr< DicomInstance > getDicomInstance() const;

    GDCMIO_API void setDicomInstance(const ::boost::shared_ptr< DicomInstance > a_dicomInstance);

    /**
     * @brief   m_object setter.
     * @param[out] _pObject replace weakptr m_object of the instance reader
     *
     * This method overload setObject to ensure that given object correspond to DATATYPE
     */
    GDCMIO_API void setObject( ::fwTools::Object::sptr a_obj);

    /**
     * @brief   m_object getter.
     * @return  m_object
     *
     * This method automatic cast object in correct DataType.
     */
    GDCMIO_API ::boost::shared_ptr< DATATYPE > getConcreteObject() const;

protected :

    /**
     * @brief   Object write on filesystem by the process.
     *
     * This object is given in parameter of setObject method but it is conserved with a weakptr.
     */
    ::fwTools::Object::wptr                 m_object;

private :

    ::boost::shared_ptr< DicomInstance >    m_dicomInstance;    ///< Store data on current instance and image instance.
};

//------------------------------------------------------------------------------

template <class DATATYPE>
::boost::shared_ptr< DicomInstance > DicomModuleWriter<DATATYPE>::getDicomInstance() const
{
    return this->m_dicomInstance;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
void DicomModuleWriter<DATATYPE>::setDicomInstance(const ::boost::shared_ptr< DicomInstance > a_dicomInstance)
{
    this->m_dicomInstance = a_dicomInstance;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
void DicomModuleWriter<DATATYPE>::setObject( ::fwTools::Object::sptr a_obj)
{
    assert ( ::boost::dynamic_pointer_cast< DATATYPE >( a_obj ) );
    m_object = a_obj;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
::boost::shared_ptr< DATATYPE > DicomModuleWriter<DATATYPE>::getConcreteObject() const
{
    assert( !m_object.expired() );
    return ::boost::dynamic_pointer_cast< DATATYPE >( m_object.lock() );
}

}   // namespace writer

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMMODULEWRITER_HXX_ */
