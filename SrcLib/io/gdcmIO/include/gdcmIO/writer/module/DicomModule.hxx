/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_MODULE_DICOMMODULEWRITER_HXX_
#define __GDCMIO_WRITER_MODULE_DICOMMODULEWRITER_HXX_

#include <boost/shared_ptr.hpp>

#include <gdcmDataSet.h>

#include <fwCore/spyLog.hpp>        // for SLM_TRACE_FUNC()

#include <fwTools/Object.hpp>

#include "gdcmIO/config.hpp"
#include "gdcmIO/container/DicomInstance.hpp"

namespace gdcmIO
{
namespace writer
{
namespace module
{

/**
 * @brief   This class contains common members for DICOM module writers.
 *
 * It allows to set a gdcm::DataSet from fwData::Object.
 *
 * @class   DicomModule.
 */
template <class DATATYPE>
class GDCMIO_CLASS_API DicomModule
{

public:

    DicomModule()
    {}

    virtual ~DicomModule()
    {}


    GDCMIO_API ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > getDicomInstance() const;

    GDCMIO_API void setDicomInstance(const ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > a_dicomInstance);

    /**
     * @brief   m_object setter.
     * @param[out] a_obj replace weakptr m_object of the instance reader
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

    ///< Store data on current instance and image instance.
    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance >    m_dicomInstance;
};

//------------------------------------------------------------------------------

template <class DATATYPE>
::boost::shared_ptr< ::gdcmIO::container::DicomInstance > DicomModule<DATATYPE>::getDicomInstance() const
{
    return this->m_dicomInstance;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
void DicomModule<DATATYPE>::setDicomInstance(const ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > a_dicomInstance)
{
    this->m_dicomInstance = a_dicomInstance;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
void DicomModule<DATATYPE>::setObject( ::fwTools::Object::sptr a_obj)
{
    SLM_ASSERT("DynamicPointerCast failed.", ::boost::dynamic_pointer_cast< DATATYPE >( a_obj ) );
    m_object = a_obj;
}

//------------------------------------------------------------------------------

template <class DATATYPE>
::boost::shared_ptr< DATATYPE > DicomModule<DATATYPE>::getConcreteObject() const
{
    SLM_ASSERT("Object expired.", !m_object.expired() );
    return ::boost::dynamic_pointer_cast< DATATYPE >( m_object.lock() );
}

} // namespace module
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_MODULE_DICOMMODULEWRITER_HXX_ */
