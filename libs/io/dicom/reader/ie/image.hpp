/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/dicom/exception/failed.hpp"
#include "io/dicom/reader/ie/information_entity.hpp"

#include <data/image.hpp>

namespace sight::io::dicom::reader::ie
{

/**
 * @brief image Information Entity class
 */
class IO_DICOM_CLASS_API image : public io::dicom::reader::ie::information_entity<data::image>
{
public:

    /**
     * @brief Constructor
     * @param[in] _dicom_series DicomSeries used to access computed tag values.
     * @param[in] _reader GDCM reader that must be enriched
     * @param[in] _instance DICOM instance used to share informations between modules
     * @param[in] _image image data
     * @param[in] _logger Logger
     * @param[in] _progress Progress callback
     * @param[in] _cancel Cancel requested callback
     */
    IO_DICOM_API image(
        const CSPTR(data::dicom_series)& _dicom_series,
        const SPTR(gdcm::Reader)& _reader,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const data::image::sptr& _image,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    IO_DICOM_API ~image() override;

    /**
     * @brief Read image Plane Module tags
     * @see PS 3.3 C.7.6.2
     */
    IO_DICOM_API virtual void read_image_plane_module();

    /**
     * @brief Read image Pixel Module tags
     * @see PS 3.3 C.7.6.3
     */
    IO_DICOM_API virtual void read_image_pixel_module();

    /**
     * @brief Read VOI LUT Module tags
     * @see PS 3.3 C.11.2
     */
    IO_DICOM_API virtual void read_voilut_module();

    /// Enable buffer rotation
    void set_buffer_rotation_enabled(bool _enabled)
    {
        m_enable_buffer_rotation = _enabled;
    }

protected:

    /**
     * @brief Read image buffer
     * @param[in] _dimensions image dimensions
     * @param[in] _bits_allocated Number of bits allocated before rescale
     * @param[in] _new_bits_allocated Number of bits allocated after rescale
     * @param[in] _perform_rescale Set to true when a rescale must be perfromed
     * @return Global raw buffer of the image.
     * @throw(io::dicom::exception::Failed)
     */
    char* read_image_buffer(
        const std::vector<unsigned int>& _dimensions,
        core::type _image_type,
        std::uint16_t _bits_allocated,
        std::uint16_t _new_bits_allocated,
        bool _perform_rescale
    );

    /**
     * @brief Correct image buffer according to image orientation
     * @param[in,out] _buffer image buffer
     * @param[in] _dimensions image dimensions
     * @param[in] _bits_allocated Number of bits allocated before rescale
     */
    char* correct_image_orientation(
        char* _buffer,
        std::vector<unsigned int>& _dimensions,
        std::uint16_t _bits_allocated
    );

    /// Enable buffer rotation
    bool m_enable_buffer_rotation {true};
};

} // namespace sight::io::dicom::reader::ie
