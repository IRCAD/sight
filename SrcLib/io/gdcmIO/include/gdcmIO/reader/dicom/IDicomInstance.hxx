/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_DICOM_IDICOMINSTANCE_HPP__
#define __GDCMIO_READER_DICOM_IDICOMINSTANCE_HPP__

#include "gdcmIO/reader/dicom/IDicomFiles.hxx"
#include "gdcmIO/container/DicomInstance.hpp"

#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

/**
 * @brief   This class contains common members to read DICOM instance.
 *
 * It uses to share data between classes.
 *
 * @class   DicomInstance.
 */
template <class DATATYPE>
class GDCMIO_CLASS_API IDicomInstance : public IDicomFiles< DATATYPE >
{

public:

    IDicomInstance()
    {}

    virtual ~IDicomInstance()
    {}

    /**
     * @brief   Interface which has to be overridden.
     */
    GDCMIO_API virtual void read() = 0;


    GDCMIO_API ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > getDicomInstance() const;

    GDCMIO_API void setDicomInstance( ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > a_dicomInstance);

private :

    ::boost::weak_ptr< ::gdcmIO::container::DicomInstance > m_dicomInstance; ///< Store data on current instance and image instance.
};

//------------------------------------------------------------------------------

template <class DATATYPE>
::boost::shared_ptr< ::gdcmIO::container::DicomInstance > IDicomInstance<DATATYPE>::getDicomInstance() const
{
    SLM_ASSERT("DicomInstance expired.", !m_dicomInstance.expired() );
    return m_dicomInstance.lock();
}

//------------------------------------------------------------------------------

template <class DATATYPE>
void IDicomInstance<DATATYPE>::setDicomInstance( ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > a_dicomInstance)
{
    m_dicomInstance = a_dicomInstance;
}

} // namespace dicom
} // namespace reader
} // namespace gdcmIO

#endif /* __GDCMIO_READER_DICOM_IDICOMINSTANCE_HPP__ */
