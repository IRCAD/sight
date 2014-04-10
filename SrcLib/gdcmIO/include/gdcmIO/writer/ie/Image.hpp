/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_IE_IMAGE_HPP__
#define __GDCMIO_WRITER_IE_IMAGE_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/writer/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @class Image
 * @brief Image Information Entity class
 */
class GDCMIO_CLASS_API Image : public ::gdcmIO::writer::ie::InformationEntity< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] image Image data
     */
    GDCMIO_API Image(SPTR(::gdcm::Writer) writer,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            ::fwData::Image::sptr image);

    /// Destructor
    GDCMIO_API virtual ~Image();

    /**
     * @brief Write General Image Module tags
     * @see PS 3.3 C.7.6.1
     */
    GDCMIO_API virtual void writeGeneralImageModule();

    /**
     * @brief Write General Image Module specific tags
     * @see PS 3.3 C.7.6.1
     * @param[in] instanceNumber Instance number
     */
    GDCMIO_API virtual void writeGeneralImageModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write Image Plane Module tags
     * @see PS 3.3 C.7.6.2
     */
    GDCMIO_API virtual void writeImagePlaneModule();

    /**
     * @brief Write Image Plane Module tags
     * @see PS 3.3 C.7.6.2
     * @param[in] instanceNumber Instance number
     */
    GDCMIO_API virtual void writeImagePlaneModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write Image Pixel Module tags
     * @see PS 3.3 C.7.6.3
     */
    GDCMIO_API virtual void writeImagePixelModule();

    /**
     * @brief Write Image Pixel Module specific tags
     * @see PS 3.3 C.7.6.3
     * @param[in] instanceNumber Instance number
     */
    GDCMIO_API virtual void writeImagePixelModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write VOI LUT Module tags
     * @see PS 3.3 C.11.2
     */
    GDCMIO_API virtual void writeVOILUTModule();

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    GDCMIO_API virtual void writeSOPCommonModule();

    /**
     * @brief Write SOP Common Module specific tags
     * @see PS 3.3 C.12.1
     * @param[in] instanceNumber Instance number
     */
    GDCMIO_API virtual void writeSOPCommonModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write CT Image Module tags
     * @see PS 3.3 C.8.2.1
     */
    GDCMIO_API virtual void writeCTImageModule();

    /**
     * @brief Write MR Image Module tags
     * @see PS 3.3 C.8.3.1
     */
    GDCMIO_API virtual void writeMRImageModule();


};

} // namespace ie
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_IE_IMAGE_HPP__ */
