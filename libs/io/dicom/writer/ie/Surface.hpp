/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "io/dicom/writer/ie/InformationEntity.hpp"

#include <data/ModelSeries.hpp>

#include <gdcmSegment.h>

#include <filesystem>

namespace sight::io::dicom
{

namespace writer
{

namespace ie
{

/**
 * @brief Surface Information Entity class
 */
class IO_DICOM_CLASS_API Surface : public io::dicom::writer::ie::InformationEntity<data::ModelSeries>
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
    IO_DICOM_API Surface(
        const SPTR(gdcm::Writer)& writer,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const SPTR(io::dicom::container::DicomInstance)& imageInstance,
        const data::ModelSeries::csptr& series,
        const core::log::Logger::sptr& logger = nullptr,
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API virtual ~Surface();

    /**
     * @brief Load Segmented Property Registry
     * @param[in] filepath Path to the registry CSV file
     * @return True on success
     */
    IO_DICOM_API bool loadSegmentedPropertyRegistry(const std::filesystem::path& filepath);

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    IO_DICOM_API virtual void writeSOPCommonModule();

    /**
     * @brief Write Surface Segmentation Module tags
     * @see PS 3.3 C.8.23.1 & PS 3.3 C.27.1
     */
    IO_DICOM_API virtual void writeSurfaceSegmentationAndSurfaceMeshModules();

protected:

    /**
     * @brief Write Segment Item into Segment Sequence
     * @see PS 3.3 C.8.23.1
     * @param[in] reconstruction Reconstruction data
     * @param[in] segmentItem GDCM segment item
     * @param[in] segment GDCM segment
     * @param[in] segmentNumber Segment number
     */
    IO_DICOM_API virtual void writeSegmentSequence(
        const CSPTR(data::Reconstruction)& reconstruction,
        gdcm::Item& segmentItem,
        const gdcm::SmartPointer<gdcm::Segment>& segment,
        unsigned short segmentNumber
    );
    /**
     * @brief Write Surface Item into Surface Sequence
     * @see PS 3.3 C.27.1
     * @param[in] reconstruction Reconstruction data
     * @param[in] surfaceItem GDCM surface item
     * @param[in] surface GDCM surface
     * @param[in] segmentNumber Associated segment number
     */
    IO_DICOM_API virtual void writeSurfaceSequence(
        const CSPTR(data::Reconstruction)& reconstruction,
        gdcm::Item& surfaceItem,
        const gdcm::SmartPointer<gdcm::Surface>& surface,
        unsigned short segmentNumber
    );

    /// Structure Dictionary
    io::dicom::helper::SegmentedPropertyRegistry m_segmentedPropertyRegistry;

    /// DICOM Image Instance
    SPTR(io::dicom::container::DicomInstance) m_imageInstance;
};

} // namespace ie

} // namespace writer

} // namespace sight::io::dicom
