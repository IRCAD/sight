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

#include <data/Series.hpp>

namespace sight::io::dicom
{

namespace writer
{

namespace ie
{

/**
 * @brief Series Information Entity class
 */
class IO_DICOM_CLASS_API Series : public io::dicom::writer::ie::InformationEntity<data::Series>
{
public:

    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] series Series data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API Series(
        const SPTR(::gdcm::Writer)& writer,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const data::Series::csptr& series,
        const core::log::Logger::sptr& logger = nullptr,
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API virtual ~Series();

    /**
     * @brief Write General Series Module tags
     * @see PS 3.3 C.7.3.1
     */
    IO_DICOM_API virtual void writeGeneralSeriesModule();

    /**
     * @brief Write Segmentation Series Module tags
     * @see PS 3.3 C.8.20.1
     */
    IO_DICOM_API virtual void writeSegmentationSeriesModule();

    /**
     * @brief Write SR Document Series Module tags
     * @see PS 3.3 C.17.1
     */
    IO_DICOM_API virtual void writeSRDocumentSeriesModule();

    /**
     * @brief Write Spatial Fiducials Series Module tags
     * @see PS 3.3 C.21.1
     */
    IO_DICOM_API virtual void writeSpatialFiducialsSeriesModule();
};

} // namespace ie

} // namespace writer

} // namespace sight::io::dicom
