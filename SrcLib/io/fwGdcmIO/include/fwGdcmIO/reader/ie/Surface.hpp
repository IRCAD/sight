/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGdcmIO/helper/SegmentedPropertyRegistry.hpp"
#include "fwGdcmIO/reader/ie/InformationEntity.hpp"

#include <fwMedData/ModelSeries.hpp>

#include <filesystem>
#include <gdcmSurfaceReader.h>

namespace fwData
{
class Reconstruction;
}

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @brief Surface Information Entity class
 */
class FWGDCMIO_CLASS_API Surface : public ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::ModelSeries >
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
    FWGDCMIO_API Surface(const CSPTR(::fwMedData::DicomSeries)& dicomSeries,
                         const SPTR(::gdcm::Reader)& reader,
                         const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                         const ::fwMedData::ModelSeries::sptr& series,
                         const ::fwLog::Logger::sptr& logger = nullptr,
                         ProgressCallback progress           = nullptr,
                         CancelRequestedCallback cancel      = nullptr);

    /**
     * @brief Load Segmented Property Registry
     * @param[in] filepath Path to the registry CSV file
     * @return True on success
     */
    FWGDCMIO_API bool loadSegmentedPropertyRegistry(const std::filesystem::path& filepath);

    /// Destructor
    FWGDCMIO_API virtual ~Surface();

    /**
     * @brief Read Surface Segmentation and Surface Mesh Modules
     * @see PS 3.3 C.8.23.1 and PS 3.3 C.27.1
     */
    FWGDCMIO_API virtual void readSurfaceSegmentationAndSurfaceMeshModules();

protected:

    /**
     * @brief Read Surface Segmentation Module tags
     * @param[in,out] reconstruction Reconstruction data
     * @param[in] segment GDCM segment
     * @param[in] segmentItem GDCM segment item
     * @see PS 3.3 C.8.23.1
     */
    virtual void readSurfaceSegmentationModule(const SPTR(::fwData::Reconstruction)& reconstruction,
                                               const ::gdcm::SmartPointer< ::gdcm::Segment >& segment,
                                               const ::gdcm::Item& segmentItem);
    /**
     * @brief Read Surface Mesh Module tags
     * @param[in,out] reconstruction Reconstruction data
     * @param[in] surface GDCM surface
     * @see PS 3.3 C.27.1
     */
    FWGDCMIO_API virtual void readSurfaceMeshModule(const SPTR(::fwData::Reconstruction)& reconstruction, const ::gdcm::SmartPointer< ::gdcm::Surface >& surface
                                                    );
    /// Segment Property Registry
    ::fwGdcmIO::helper::SegmentedPropertyRegistry m_segmentedPropertyRegistry;

};

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
