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

#include "io/dicom/reader/iod/information_object_definition.hpp"

#include <gdcmSegment.h>

namespace sight::data
{

class Reconstruction;

} // namespace sight::data

namespace sight::data
{

class model_series;

} // namespace sight::data

namespace sight::io::dicom::reader::iod
{

/**
 * @brief SurfaceSegmentationIOD class used to read Surface Segmentation IODs
 */
class SIGHT_IO_DICOM_CLASS_API surface_segmentation_iod : public io::dicom::reader::iod::information_object_definition
{
public:

    /**
     * @brief Constructor
     * @param[in] _dicom_series DicomSeries
     * @param[in] _instance Instance
     * @param[in] _logger Logger
     * @param[in] _progress Progress callback
     * @param[in] _cancel Cancel requested callback
     */
    SIGHT_IO_DICOM_API surface_segmentation_iod(
        const data::dicom_series::csptr& _dicom_series,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    SIGHT_IO_DICOM_API ~surface_segmentation_iod() override;

    /**
     * @brief Read a DICOM File
     * @param[in,out] _series Series that must be enriched
     * @throw io::dicom::exception::Failed
     */
    SIGHT_IO_DICOM_API void read(data::series::sptr _series) override;
};

} // namespace sight::io::dicom::reader::iod
