/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_INSTANCE_DICOMIMAGE_HPP__
#define __GDCMIO_WRITER_INSTANCE_DICOMIMAGE_HPP__

#include <string>

#include <fwData/Image.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/writer/instance/DicomInstance.hxx"

namespace gdcmIO
{
namespace writer
{
namespace instance
{

/**
 * @brief   This class write one image (in the DICOM sense).
 *
 * According to this multiFrame flag, It splits or not volume image
 * in several files.
 *
 * @note    This is a common image writer.
 *          It does not certify a properly written DICOM file.
 *
 * @class   DicomImage.
 */
class GDCMIO_CLASS_API DicomImage : public DicomInstance< ::fwData::Image >
{

public :

    /**
     * @brief   Constructor which instantiate a gdcm::ImageWriter.
     */
    GDCMIO_API DicomImage();

    GDCMIO_API virtual ~DicomImage();

    /**
     * @brief   Write common image modules to complete and start files writing.
     *
     * @note    setDataSet() must be called before this method.
     *
     * @see     PS 3.3 form C.7.6.1 to C.7.6.6
     */
    GDCMIO_API void write() throw(::gdcmIO::exception::Failed);

    /**
     * @brief   Check if the image is multiframe (= one 3D image in one file)
     */
    GDCMIO_API bool isMultiFrame() const;

    GDCMIO_API void setMultiFrame(const bool a_state);

    GDCMIO_API fwGettersSettersDocMacro(ImageID, imageID, std::string, The index of image);

    GDCMIO_API void setImageID(const unsigned int a_imageID);

private :

    /**
     * @brief   Write one file for each 2D image.
     * In addition, it completes the storage path.
     */
    void write2DImage();

    /**
     * @brief   Write one file for 3D (volume) image.
     * In addition, it completes the storage path.
     */
    void write3DImage();

    /**
     * @brief   Write Image Plane Module in a data set.
     *
     * @see     PS 3.3 C.7.6.2
     */
    void writeImagePlane();

    /**
     * @brief   Write Image Pixel Module in a data set.
     *
     * @see     PS 3.3 C.7.6.3
     */
    void writeImagePixel();

    /**
     * @brief   Write Multi Frame Module in a data set.
     *
     * @see     PS 3.3 C.7.6.6
     */
    void writeMultiFrame();


    bool        m_isMultiFrame; ///< Split saving with imageID if it is set to false
                                ///< Else, all the frames of a volume image will be saved in one file.

    std::string m_imageID;      ///< Index of the image (eg : 1 or 2 or ...)
};

} // namespace instance
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_INSTANCE_DICOMIMAGE_HPP__ */
