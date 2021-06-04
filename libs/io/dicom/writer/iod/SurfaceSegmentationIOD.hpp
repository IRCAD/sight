/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/dicom/config.hpp"
#include "io/dicom/container/DicomInstance.hpp"
#include "io/dicom/writer/iod/InformationObjectDefinition.hpp"

#include <filesystem>

namespace sight::io::dicom
{

namespace writer
{

namespace iod
{

/**
 * @brief SurfaceSegmentationIOD class used to write Surface Segmentation DICOM files
 */
class IO_DICOM_CLASS_API SurfaceSegmentationIOD : public io::dicom::writer::iod::InformationObjectDefinition
{
public:

    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] imageInstance Associated image instance
     * @param[in] destinationPath Destination path
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API SurfaceSegmentationIOD(
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const SPTR(io::dicom::container::DicomInstance)& imageInstance,
        const std::filesystem::path& destinationPath,
        const core::log::Logger::sptr& logger = core::log::Logger::New(),
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API virtual ~SurfaceSegmentationIOD();

    /// Write DICOM file
    IO_DICOM_API void write(const data::Series::csptr& series) override;

protected:

    /// Image instance
    SPTR(io::dicom::container::DicomInstance) m_imageInstance;
};

} // namespace iod

} // namespace writer

} // namespace sight::io::dicom
