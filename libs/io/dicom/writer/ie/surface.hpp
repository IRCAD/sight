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

#include "io/dicom/helper/segmented_property_registry.hpp"
#include "io/dicom/writer/ie/information_entity.hpp"

#include <data/model_series.hpp>

#include <gdcmSegment.h>

#include <filesystem>

namespace sight::io::dicom::writer::ie
{

/**
 * @brief Surface Information Entity class
 */
class SIGHT_IO_DICOM_CLASS_API surface : public io::dicom::writer::ie::information_entity<data::model_series>
{
public:

    /**
     * @brief Constructor
     * @param[in] _writer GDCM writer that must be enriched
     * @param[in] _instance DICOM instance used to share information between modules
     * @param[in] _series Series data
     * @param[in] _logger Logger
     * @param[in] _progress Progress callback
     * @param[in] _cancel Cancel requested callback
     */
    SIGHT_IO_DICOM_API surface(
        const SPTR(gdcm::Writer)& _writer,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        SPTR(io::dicom::container::dicom_instance)_image_instance,
        const data::model_series::csptr& _series,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    SIGHT_IO_DICOM_API ~surface() override;

    /**
     * @brief Load Segmented Property Registry
     * @param[in] _filepath Path to the registry CSV file
     * @return True on success
     */
    SIGHT_IO_DICOM_API bool load_segmented_property_registry(const std::filesystem::path& _filepath);

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    SIGHT_IO_DICOM_API virtual void write_sop_common_module();

    /**
     * @brief Write Surface Segmentation Module tags
     * @see PS 3.3 C.8.23.1 & PS 3.3 C.27.1
     */
    SIGHT_IO_DICOM_API virtual void write_surface_segmentation_and_surface_mesh_modules();

protected:

    /**
     * @brief Write Segment Item into Segment Sequence
     * @see PS 3.3 C.8.23.1
     * @param[in] _reconstruction Reconstruction data
     * @param[in] _segment_item GDCM segment item
     * @param[in] _segment GDCM segment
     * @param[in] _segment_number Segment number
     */
    SIGHT_IO_DICOM_API virtual void write_segment_sequence(
        const CSPTR(data::reconstruction)& _reconstruction,
        gdcm::Item& _segment_item,
        const gdcm::SmartPointer<gdcm::Segment>& _segment,
        std::uint16_t _segment_number
    );
    /**
     * @brief Write Surface Item into Surface Sequence
     * @see PS 3.3 C.27.1
     * @param[in] _reconstruction Reconstruction data
     * @param[in] _surface_item GDCM surface item
     * @param[in] _surface GDCM surface
     * @param[in] _segment_number Associated segment number
     */
    SIGHT_IO_DICOM_API virtual void write_surface_sequence(
        const CSPTR(data::reconstruction)& _reconstruction,
        gdcm::Item& _surface_item,
        const gdcm::SmartPointer<gdcm::Surface>& _surface,
        std::uint16_t _segment_number
    );

    /// Structure Dictionary
    io::dicom::helper::segmented_property_registry m_segmented_property_registry;

    /// DICOM Image Instance
    SPTR(io::dicom::container::dicom_instance) m_image_instance;
};

} // namespace sight::io::dicom::writer::ie
