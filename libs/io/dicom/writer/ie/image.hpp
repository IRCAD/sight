/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/image.hpp>

namespace sight::io::dicom::writer::ie
{

/**
 * @brief image Information Entity class
 */
class IO_DICOM_CLASS_API image : public io::dicom::writer::ie::InformationEntity<data::image>
{
public:

    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] image image data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API image(
        const SPTR(gdcm::Writer)& _writer,
        const SPTR(io::dicom::container::DicomInstance)& _instance,
        const data::image::csptr& _image,
        const core::log::logger::sptr& _logger = nullptr,
        ProgressCallback _progress             = nullptr,
        CancelRequestedCallback _cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API ~image() override;

    /**
     * @brief Write General image Module tags
     * @see PS 3.3 C.7.6.1
     */
    IO_DICOM_API virtual void writeGeneralImageModule();

    /**
     * @brief Write General image Module specific tags
     * @see PS 3.3 C.7.6.1
     * @param[in] instanceNumber Instance number
     */
    IO_DICOM_API virtual void writeGeneralImageModuleSpecificTags(unsigned int _instance_number);

    /**
     * @brief Write image Plane Module tags
     * @see PS 3.3 C.7.6.2
     */
    IO_DICOM_API virtual void writeImagePlaneModule();

    /**
     * @brief Write image Plane Module tags
     * @see PS 3.3 C.7.6.2
     * @param[in] instanceNumber Instance number
     */
    IO_DICOM_API virtual void writeImagePlaneModuleSpecificTags(unsigned int _instance_number);

    /**
     * @brief Write image Pixel Module tags
     * @see PS 3.3 C.7.6.3
     */
    IO_DICOM_API virtual void writeImagePixelModule();

    /**
     * @brief Write image Pixel Module specific tags
     * @see PS 3.3 C.7.6.3
     * @param[in] instanceNumber Instance number
     */
    IO_DICOM_API virtual void writeImagePixelModuleSpecificTags(unsigned int _instance_number);

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
    IO_DICOM_API virtual void writeSOPCommonModuleSpecificTags(unsigned int _instance_number);

    /**
     * @brief Write CT image Module tags
     * @see PS 3.3 C.8.2.1
     */
    IO_DICOM_API virtual void writeCTImageModule();

    /**
     * @brief Write MR image Module tags
     * @see PS 3.3 C.8.3.1
     */
    IO_DICOM_API virtual void writeMRImageModule();
};

} // namespace sight::io::dicom::writer::ie
