/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMIMAGEREADER_HPP_
#define _GDCMIO_DICOMIMAGEREADER_HPP_

#include <fwData/Acquisition.hpp>

#include "gdcmIO/reader/DicomInstanceReader.hxx"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class read an DICOM image from DICOM file.
 *
 * It is also the common image reader. Image readers which
 * implement specific modalities must inherit of this class.
 *
 * @class   DicomImageReader
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomImageReader : public DicomInstanceReader< ::fwData::Acquisition >
{

public :

    /**
     * @brief   Constructor which instantiates a gdcm::ImageReader.
     */
    GDCMIO_API DicomImageReader();

    GDCMIO_API ~DicomImageReader();

    /**
     * @brief   Launch DICOM image (2D or 3D) reading from DICOM file(s).
     * It sorts files if necessary and can compute depth spacing.
     *
     * @note    setFileNames() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw(::fwTools::Failed);

private :

    /**
     * @brief   Read DICOM image (2D or 3D) from DICOM file(s).
     *
     * @param   img Image to set.
     */
    void readImage( ::fwData::Image::sptr img) throw(::fwTools::Failed);

    /**
     * @brief   Read 3D image from several files.
     *
     * @return  Global raw buffer of the entire 3D image.
     */
    void * read2DImages() throw(::fwTools::Failed);

    /**
     * @brief   Set the image data buffer .
     *
     * @note    This method doesn't manage any rescale of data buffer(for this see rescaleImageBuffer(...)).
     */
    void setImageBuffer(::gdcm::Image & gImg, ::fwData::Image::sptr img, ::boost::shared_ptr< DicomInstance > dicomInstance) throw(::fwTools::Failed);

    void rescaleImageBuffer(::gdcm::Image & gImg, ::fwData::Image::sptr img,
                            const gdcm::PixelFormat::ScalarType& outputPixelType ) throw(::fwTools::Failed);

    float                                m_zSpacing;    ///< Spacing in depth of a 3D image

};

}  // namespace reader

}  // namespace gdcmIO

#endif /*_GDCMIO_DICOMIMAGEREADER_HPP_*/
