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

#include "io/dicom/reader/ie/InformationEntity.hpp"

#include <data/image.hpp>

namespace sight::io::dicom::reader::ie
{

/**
 * @brief Spatial Fiducials Information Entity class
 */
class IO_DICOM_CLASS_API SpatialFiducials : public io::dicom::reader::ie::InformationEntity<data::image>
{
public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] series Series data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API SpatialFiducials(
        const CSPTR(data::dicom_series)& dicomSeries,
        const SPTR(gdcm::Reader)& reader,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const data::image::sptr& series,
        const core::log::logger::sptr& logger = nullptr,
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API ~SpatialFiducials() override;

    /**
     * @brief Read image landmarks from dataset
     */
    void readLandmark(const gdcm::DataSet& fiducialDataset);

    /**
     * @brief Read image distances from dataset
     */
    void readDistance(const gdcm::DataSet& fiducialDataset);
};

} // namespace sight::io::dicom::reader::ie
