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

#include "io/dicom/reader/iod/InformationObjectDefinition.hpp"

namespace sight::io::dicom::reader::iod
{

/**
 * @brief SpatialFiducialsIOD class used to read Spatial Fiducials IODs
 */
class IO_DICOM_CLASS_API SpatialFiducialsIOD : public io::dicom::reader::iod::InformationObjectDefinition
{
public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API SpatialFiducialsIOD(
        const data::dicom_series::csptr& _dicom_series,
        const SPTR(io::dicom::container::DicomInstance)& _instance,
        const core::log::logger::sptr& _logger = nullptr,
        ProgressCallback _progress             = nullptr,
        CancelRequestedCallback _cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API ~SpatialFiducialsIOD() override;

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     * @throw io::dicom::exception::Failed
     */
    IO_DICOM_API void read(data::series::sptr _series) override;
};

} // namespace sight::io::dicom::reader::iod
