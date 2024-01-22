/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/dicom/reader/ie/information_entity.hpp"

#include <data/image.hpp>

namespace sight::io::dicom::reader::ie
{

/**
 * @brief Spatial Fiducials Information Entity class
 */
class IO_DICOM_CLASS_API spatial_fiducials : public io::dicom::reader::ie::information_entity<data::image>
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
    IO_DICOM_API spatial_fiducials(
        const CSPTR(data::dicom_series)& _dicom_series,
        const SPTR(gdcm::Reader)& _reader,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const data::image::sptr& _series,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    IO_DICOM_API ~spatial_fiducials() override;

    /**
     * @brief Read image landmarks from dataset
     */
    void read_landmark(const gdcm::DataSet& _fiducial_dataset);

    /**
     * @brief Read image distances from dataset
     */
    void read_distance(const gdcm::DataSet& _fiducial_dataset);
};

} // namespace sight::io::dicom::reader::ie
