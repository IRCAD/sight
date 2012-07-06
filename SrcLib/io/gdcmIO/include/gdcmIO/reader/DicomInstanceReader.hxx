/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMINSTANCEREADER_HPP_
#define _GDCMIO_DICOMINSTANCEREADER_HPP_

#include "gdcmIO/reader/DicomFilesReader.hxx"
#include "gdcmIO/DicomInstance.hpp"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class contains common members to read DICOM instance.
 *
 * It uses to share data between classes.
 *
 * @class   DicomInstanceReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
template <class DATATYPE>
class GDCMIO_CLASS_API DicomInstanceReader : public DicomFilesReader< DATATYPE >
{

public:

    GDCMIO_API DicomInstanceReader()
    {
        SLM_TRACE_FUNC();
    }

    GDCMIO_API virtual ~DicomInstanceReader()
    {
        SLM_TRACE_FUNC();
    }

    /**
     * @brief   Interface which has to be overridden.
     */
    GDCMIO_API virtual void read() = 0;


    GDCMIO_API ::boost::shared_ptr< DicomInstance > getDicomInstance() const;

    GDCMIO_API void setDicomInstance( ::boost::shared_ptr< DicomInstance > a_dicomInstance);

private :

    ::boost::weak_ptr< DicomInstance > m_dicomInstance; ///< Store data on current instance and image instance.
};

//------------------------------------------------------------------------------

template <class DATATYPE>
::boost::shared_ptr< DicomInstance > DicomInstanceReader<DATATYPE>::getDicomInstance() const
{
    assert( !m_dicomInstance.expired() );
    return m_dicomInstance.lock();
}

//------------------------------------------------------------------------------

template <class DATATYPE>
void DicomInstanceReader<DATATYPE>::setDicomInstance( ::boost::shared_ptr< DicomInstance > a_dicomInstance)
{
    m_dicomInstance = a_dicomInstance;
}


}   // namespace reader

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMINSTANCEREADER_HPP_ */
