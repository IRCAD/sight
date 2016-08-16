/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IE_IMAGE_HPP__
#define __FWGDCMIO_WRITER_IE_IMAGE_HPP__

#include "fwGdcmIO/writer/ie/InformationEntity.hpp"

#include <fwData/Image.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @brief Image Information Entity class
 */
class FWGDCMIO_CLASS_API Image : public ::fwGdcmIO::writer::ie::InformationEntity< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] image Image data
     */
    FWGDCMIO_API Image(SPTR(::gdcm::Writer)writer,
                       SPTR(::fwGdcmIO::container::DicomInstance)instance,
                       ::fwData::Image::sptr image);

    /// Destructor
    FWGDCMIO_API virtual ~Image();

    /**
     * @brief Write General Image Module tags
     * @see PS 3.3 C.7.6.1
     */
    FWGDCMIO_API virtual void writeGeneralImageModule();

    /**
     * @brief Write General Image Module specific tags
     * @see PS 3.3 C.7.6.1
     * @param[in] instanceNumber Instance number
     */
    FWGDCMIO_API virtual void writeGeneralImageModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write Image Plane Module tags
     * @see PS 3.3 C.7.6.2
     */
    FWGDCMIO_API virtual void writeImagePlaneModule();

    /**
     * @brief Write Image Plane Module tags
     * @see PS 3.3 C.7.6.2
     * @param[in] instanceNumber Instance number
     */
    FWGDCMIO_API virtual void writeImagePlaneModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write Image Pixel Module tags
     * @see PS 3.3 C.7.6.3
     */
    FWGDCMIO_API virtual void writeImagePixelModule();

    /**
     * @brief Write Image Pixel Module specific tags
     * @see PS 3.3 C.7.6.3
     * @param[in] instanceNumber Instance number
     */
    FWGDCMIO_API virtual void writeImagePixelModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write VOI LUT Module tags
     * @see PS 3.3 C.11.2
     */
    FWGDCMIO_API virtual void writeVOILUTModule();

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    FWGDCMIO_API virtual void writeSOPCommonModule();

    /**
     * @brief Write SOP Common Module specific tags
     * @see PS 3.3 C.12.1
     * @param[in] instanceNumber Instance number
     */
    FWGDCMIO_API virtual void writeSOPCommonModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write CT Image Module tags
     * @see PS 3.3 C.8.2.1
     */
    FWGDCMIO_API virtual void writeCTImageModule();

    /**
     * @brief Write MR Image Module tags
     * @see PS 3.3 C.8.3.1
     */
    FWGDCMIO_API virtual void writeMRImageModule();


};

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_WRITER_IE_IMAGE_HPP__ */
