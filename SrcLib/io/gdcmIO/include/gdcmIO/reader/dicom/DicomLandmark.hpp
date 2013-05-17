/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_DICOM_DICOMLANDMARK_HPP__
#define __GDCMIO_READER_DICOM_DICOMLANDMARK_HPP__

#include <boost/shared_ptr.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/reader/dicom/IDicomInstance.hxx"
#include "gdcmIO/container/DicomLandmark.hpp"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

/**
 * @brief   This class define a reader of landmark(s) container taken from a DICOM file.
 *
 * It search and read a specific template from the root of the document SR.
 * Currently, the template is based on TID 320 (taken from PS 3.16).
 *
 * @class   DicomLandmark
 */
class GDCMIO_CLASS_API DicomLandmark : public IDicomInstance< fwData::Image >
{

public :

    GDCMIO_API DicomLandmark();

    GDCMIO_API ~DicomLandmark();

    /**
     * @brief   Read landmark(s) from a DICOM file.
     *
     * @note    setReader() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw (::gdcmIO::exception::Failed);

private :

    /**
     * @brief    Read landmark(s) from its data set.
     * It will read one landmark with a non contiguous content.
     *
     * @param   a_ds            Data set of landmark.
     * @param   a_continuity    Continuity of content.
     */
    void readLandmark(const ::gdcm::DataSet & a_ds, const bool a_continuity = false) throw (::gdcmIO::exception::Failed);


    ::boost::shared_ptr< ::gdcmIO::container::DicomLandmark > m_dicomLandmarks;  ///< Store and transform into fwData
};

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO

#endif /* __GDCMIO_READER_DICOM_DICOMLANDMARK_HPP__ */
