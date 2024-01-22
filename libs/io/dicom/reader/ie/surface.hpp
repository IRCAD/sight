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
#include "io/dicom/reader/ie/information_entity.hpp"

#include <data/model_series.hpp>

#include <gdcmSurfaceReader.h>

#include <filesystem>

namespace sight::data
{

class Reconstruction;

} // namespace sight::data

namespace sight::io::dicom::reader::ie
{

/**
 * @brief Surface Information Entity class
 */
class IO_DICOM_CLASS_API surface : public io::dicom::reader::ie::information_entity<data::model_series>
{
public:

    /**
     * @brief Constructor
     * @param[in] _dicom_series DicomSeries used to access computed tag values.
     * @param[in] _reader GDCM reader that must be enriched
     * @param[in] _instance DICOM instance used to share informations between modules
     * @param[in] _series Series data
     * @param[in] _logger Logger
     * @param[in] _progress Progress callback
     * @param[in] _cancel Cancel requested callback
     */
    IO_DICOM_API surface(
        const CSPTR(data::dicom_series)& _dicom_series,
        const SPTR(gdcm::Reader)& _reader,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const data::model_series::sptr& _series,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /**
     * @brief Load Segmented Property Registry
     * @param[in] _filepath Path to the registry CSV file
     * @return True on success
     */
    IO_DICOM_API bool load_segmented_property_registry(const std::filesystem::path& _filepath);

    /// Destructor
    IO_DICOM_API ~surface() override;

    /**
     * @brief Read Surface Segmentation and Surface Mesh Modules
     * @see PS 3.3 C.8.23.1 and PS 3.3 C.27.1
     */
    IO_DICOM_API virtual void read_surface_segmentation_and_surface_mesh_modules();

protected:

    /**
     * @brief Read Surface Segmentation Module tags
     * @param[in,out] _reconstruction Reconstruction data
     * @param[in] _segment GDCM segment
     * @param[in] _segment_item GDCM segment item
     * @see PS 3.3 C.8.23.1
     */
    virtual void read_surface_segmentation_module(
        const SPTR(data::reconstruction)& _reconstruction,
        const gdcm::SmartPointer<gdcm::Segment>& _segment,
        const gdcm::Item& _segment_item
    );
    /**
     * @brief Read Surface Mesh Module tags
     * @param[in,out] _reconstruction Reconstruction data
     * @param[in] _surface GDCM surface
     * @see PS 3.3 C.27.1
     */
    IO_DICOM_API virtual void read_surface_mesh_module(
        const SPTR(data::reconstruction)& _reconstruction,
        const gdcm::SmartPointer<gdcm::Surface>& _surface
    );
    /// Segment Property Registry
    io::dicom::helper::segmented_property_registry m_segmented_property_registry;
};

} // namespace sight::io::dicom::reader::ie
