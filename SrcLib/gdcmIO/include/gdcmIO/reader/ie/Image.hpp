/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IE_IMAGE_HPP__
#define __GDCMIO_READER_IE_IMAGE_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/reader/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @class Image
 * @brief Image Information Entity class
 */
class GDCMIO_CLASS_API Image : public ::gdcmIO::reader::ie::InformationEntity< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Image data
     */
    GDCMIO_API Image(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                     SPTR(::gdcm::Reader)reader,
                     SPTR(::gdcmIO::container::DicomInstance)instance,
                     ::fwData::Image::sptr image);

    /// Destructor
    GDCMIO_API virtual ~Image();

    /**
     * @brief Read Image Plane Module tags
     * @see PS 3.3 C.7.6.2
     */
    GDCMIO_API virtual void readImagePlaneModule();

    /**
     * @brief Read Image Pixel Module tags
     * @see PS 3.3 C.7.6.3
     */
    GDCMIO_API virtual void readImagePixelModule();

    /**
     * @brief Read VOI LUT Module tags
     * @see PS 3.3 C.11.2
     */
    GDCMIO_API virtual void readVOILUTModule();

protected:
    /**
     * @brief Read image buffer
     * @return Global raw buffer of the image.
     */
    void* readImageBuffer() throw(::gdcmIO::exception::Failed);


};

} // namespace ie
} // namespace reader
} // namespace gdcmIO

#endif /* __GDCMIO_READER_IE_IMAGE_HPP__ */
