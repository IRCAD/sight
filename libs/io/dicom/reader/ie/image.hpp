/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/dicom/exception/Failed.hpp"
#include "io/dicom/reader/ie/InformationEntity.hpp"

#include <data/image.hpp>

namespace sight::io::dicom::reader::ie
{

/**
 * @brief image Information Entity class
 */
class IO_DICOM_CLASS_API image : public io::dicom::reader::ie::InformationEntity<data::image>
{
public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image image data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API image(
        const CSPTR(data::dicom_series)& dicomSeries,
        const SPTR(gdcm::Reader)& reader,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const data::image::sptr& image,
        const core::log::logger::sptr& logger = nullptr,
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API ~image() override;

    /**
     * @brief Read image Plane Module tags
     * @see PS 3.3 C.7.6.2
     */
    IO_DICOM_API virtual void readImagePlaneModule();

    /**
     * @brief Read image Pixel Module tags
     * @see PS 3.3 C.7.6.3
     */
    IO_DICOM_API virtual void readImagePixelModule();

    /**
     * @brief Read VOI LUT Module tags
     * @see PS 3.3 C.11.2
     */
    IO_DICOM_API virtual void readVOILUTModule();

    /// Enable buffer rotation
    void setBufferRotationEnabled(bool enabled)
    {
        m_enableBufferRotation = enabled;
    }

protected:

    /**
     * @brief Read image buffer
     * @param[in] dimensions image dimensions
     * @param[in] bitsAllocated Number of bits allocated before rescale
     * @param[in] newBitsAllocated Number of bits allocated after rescale
     * @param[in] performRescale Set to true when a rescale must be perfromed
     * @return Global raw buffer of the image.
     * @throw(io::dicom::exception::Failed)
     */
    char* readImageBuffer(
        const std::vector<unsigned int>& dimensions,
        core::type imageType,
        std::uint16_t bitsAllocated,
        std::uint16_t newBitsAllocated,
        bool performRescale
    );

    /**
     * @brief Correct image buffer according to image orientation
     * @param[in,out] buffer image buffer
     * @param[in] dimensions image dimensions
     * @param[in] bitsAllocated Number of bits allocated before rescale
     */
    char* correctImageOrientation(
        char* buffer,
        std::vector<unsigned int>& dimensions,
        std::uint16_t bitsAllocated
    );

    /// Enable buffer rotation
    bool m_enableBufferRotation {true};
};

} // namespace sight::io::dicom::reader::ie
