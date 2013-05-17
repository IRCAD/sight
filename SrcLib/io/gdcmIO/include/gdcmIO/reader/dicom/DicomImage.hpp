/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_DICOM_DICOMIMAGE_HPP__
#define __GDCMIO_READER_DICOM_DICOMIMAGE_HPP__

#include <gdcmImage.h>

#include <fwData/Image.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/container/DicomInstance.hpp"
#include "gdcmIO/reader/dicom/IDicomInstance.hxx"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

/**
 * @brief   This class read an DICOM image from DICOM file.
 *
 * It is also the common image reader. Image readers which
 * implement specific modalities must inherit of this class.
 *
 * @class   DicomImage
 */
class GDCMIO_CLASS_API DicomImage : public IDicomInstance< ::fwData::Image >
{

public :

    /**
     * @brief   Constructor which instantiates a gdcm::ImageReader.
     */
    GDCMIO_API DicomImage();

    GDCMIO_API ~DicomImage();

    /**
     * @brief   Launch DICOM image (2D or 3D) reading from DICOM file(s).
     * It sorts files if necessary and can compute depth spacing.
     *
     * @note    setFileNames() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw(::gdcmIO::exception::Failed);

private :

    /**
     * @brief   Read DICOM image (2D or 3D) from DICOM file(s).
     *
     * @param   img Image to set.
     */
    void readImage( ::fwData::Image::sptr img) throw(::gdcmIO::exception::Failed);

    /**
     * @brief   Read 3D image from several files.
     *
     * @return  Global raw buffer of the entire 3D image.
     */
    void * read2DImages() throw(::gdcmIO::exception::Failed);

    /**
     * @brief   Set the image data buffer .
     *
     * @note    This method doesn't manage any rescale of data buffer(for this see rescaleImageBuffer(...)).
     */
    void setImageBuffer(::gdcm::Image & gImg,
                        ::fwData::Image::sptr img,
                        ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance) throw(::gdcmIO::exception::Failed);

    void rescaleImageBuffer(::gdcm::Image & gImg,
                            ::fwData::Image::sptr img,
                            const gdcm::PixelFormat::ScalarType& outputPixelType ) throw(::gdcmIO::exception::Failed);

    /// Spacing in depth of a 3D image
    float  m_zSpacing;

};

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO

#endif /*_GDCMIO_READER_DICOM_DICOMIMAGE_HPP_*/
