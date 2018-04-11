/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGdcmIO/helper/SegmentedPropertyRegistry.hpp"
#include "fwGdcmIO/reader/ie/InformationEntity.hpp"

#include <fwMedData/ModelSeries.hpp>

#include <boost/filesystem/path.hpp>

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
    FWGDCMIO_API bool loadSegmentedPropertyRegistry(const ::boost::filesystem::path& filepath);

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
