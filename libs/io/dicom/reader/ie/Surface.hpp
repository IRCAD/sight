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

#include "io/dicom/helper/SegmentedPropertyRegistry.hpp"
#include "io/dicom/reader/ie/InformationEntity.hpp"

#include <data/ModelSeries.hpp>

#include <gdcmSurfaceReader.h>

#include <filesystem>

namespace sight::data
{

class Reconstruction;

}

namespace sight::io::dicom
{

namespace reader
{

namespace ie
{

/**
 * @brief Surface Information Entity class
 */
class IO_DICOM_CLASS_API Surface : public io::dicom::reader::ie::InformationEntity<data::ModelSeries>
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
    IO_DICOM_API Surface(
        const CSPTR(data::DicomSeries)& dicomSeries,
        const SPTR(::gdcm::Reader)& reader,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const data::ModelSeries::sptr& series,
        const core::log::Logger::sptr& logger = nullptr,
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /**
     * @brief Load Segmented Property Registry
     * @param[in] filepath Path to the registry CSV file
     * @return True on success
     */
    IO_DICOM_API bool loadSegmentedPropertyRegistry(const std::filesystem::path& filepath);

    /// Destructor
    IO_DICOM_API virtual ~Surface();

    /**
     * @brief Read Surface Segmentation and Surface Mesh Modules
     * @see PS 3.3 C.8.23.1 and PS 3.3 C.27.1
     */
    IO_DICOM_API virtual void readSurfaceSegmentationAndSurfaceMeshModules();

protected:

    /**
     * @brief Read Surface Segmentation Module tags
     * @param[in,out] reconstruction Reconstruction data
     * @param[in] segment GDCM segment
     * @param[in] segmentItem GDCM segment item
     * @see PS 3.3 C.8.23.1
     */
    virtual void readSurfaceSegmentationModule(
        const SPTR(data::Reconstruction)& reconstruction,
        const ::gdcm::SmartPointer< ::gdcm::Segment>& segment,
        const ::gdcm::Item& segmentItem
    );
    /**
     * @brief Read Surface Mesh Module tags
     * @param[in,out] reconstruction Reconstruction data
     * @param[in] surface GDCM surface
     * @see PS 3.3 C.27.1
     */
    IO_DICOM_API virtual void readSurfaceMeshModule(
        const SPTR(data::Reconstruction)& reconstruction,
        const ::gdcm::SmartPointer< ::gdcm::Surface>& surface
    );
    /// Segment Property Registry
    io::dicom::helper::SegmentedPropertyRegistry m_segmentedPropertyRegistry;
};

} // namespace ie

} // namespace reader

} // namespace sight::io::dicom
