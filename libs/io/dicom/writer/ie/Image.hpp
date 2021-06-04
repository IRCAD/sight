/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "io/dicom/writer/ie/InformationEntity.hpp"

#include <data/Image.hpp>

namespace sight::io::dicom
{

namespace writer
{

namespace ie
{

/**
 * @brief Image Information Entity class
 */
class IO_DICOM_CLASS_API Image : public io::dicom::writer::ie::InformationEntity<data::Image>
{
public:

    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] image Image data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API Image(
        const SPTR(::gdcm::Writer)& writer,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const data::Image::csptr& image,
        const core::log::Logger::sptr& logger = nullptr,
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API virtual ~Image();

    /**
     * @brief Write General Image Module tags
     * @see PS 3.3 C.7.6.1
     */
    IO_DICOM_API virtual void writeGeneralImageModule();

    /**
     * @brief Write General Image Module specific tags
     * @see PS 3.3 C.7.6.1
     * @param[in] instanceNumber Instance number
     */
    IO_DICOM_API virtual void writeGeneralImageModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write Image Plane Module tags
     * @see PS 3.3 C.7.6.2
     */
    IO_DICOM_API virtual void writeImagePlaneModule();

    /**
     * @brief Write Image Plane Module tags
     * @see PS 3.3 C.7.6.2
     * @param[in] instanceNumber Instance number
     */
    IO_DICOM_API virtual void writeImagePlaneModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write Image Pixel Module tags
     * @see PS 3.3 C.7.6.3
     */
    IO_DICOM_API virtual void writeImagePixelModule();

    /**
     * @brief Write Image Pixel Module specific tags
     * @see PS 3.3 C.7.6.3
     * @param[in] instanceNumber Instance number
     */
    IO_DICOM_API virtual void writeImagePixelModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write VOI LUT Module tags
     * @see PS 3.3 C.11.2
     */
    IO_DICOM_API virtual void writeVOILUTModule();

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    IO_DICOM_API virtual void writeSOPCommonModule();

    /**
     * @brief Write SOP Common Module specific tags
     * @see PS 3.3 C.12.1
     * @param[in] instanceNumber Instance number
     */
    IO_DICOM_API virtual void writeSOPCommonModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write CT Image Module tags
     * @see PS 3.3 C.8.2.1
     */
    IO_DICOM_API virtual void writeCTImageModule();

    /**
     * @brief Write MR Image Module tags
     * @see PS 3.3 C.8.3.1
     */
    IO_DICOM_API virtual void writeMRImageModule();
};

} // namespace ie

} // namespace writer

} // namespace sight::io::dicom
