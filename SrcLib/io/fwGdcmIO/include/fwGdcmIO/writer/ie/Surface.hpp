/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IE_SURFACE_HPP__
#define __FWGDCMIO_WRITER_IE_SURFACE_HPP__

#include "fwGdcmIO/writer/ie/InformationEntity.hpp"
#include "fwGdcmIO/helper/SegmentedPropertyRegistry.hpp"

#include <fwMedData/ModelSeries.hpp>
#include <gdcmSegment.h>

#include <boost/filesystem/path.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @brief Surface Information Entity class
 */
class FWGDCMIO_CLASS_API Surface : public ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::ModelSeries >
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
    FWGDCMIO_API Surface(const SPTR(::gdcm::Writer)& writer,
                         const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                         const SPTR(::fwGdcmIO::container::DicomInstance)& imageInstance,
                         const ::fwMedData::ModelSeries::sptr& series,
                         const ::fwLog::Logger::sptr& logger = nullptr,
                         ProgressCallback progress = nullptr,
                         CancelRequestedCallback cancel = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~Surface();

    /**
     * @brief Load Segmented Property Registry
     * @param[in] filepath Path to the registry CSV file
     * @return True on success
     */
    FWGDCMIO_API bool loadSegmentedPropertyRegistry(const ::boost::filesystem::path& filepath);

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    FWGDCMIO_API virtual void writeSOPCommonModule();

    /**
     * @brief Write Surface Segmentation Module tags
     * @see PS 3.3 C.8.23.1 & PS 3.3 C.27.1
     */
    FWGDCMIO_API virtual void writeSurfaceSegmentationAndSurfaceMeshModules();


protected:

    /**
     * @brief Write Segment Item into Segment Sequence
     * @see PS 3.3 C.8.23.1
     * @param[in] reconstruction Reconstruction data
     * @param[in] segmentItem GDCM segment item
     * @param[in] segment GDCM segment
     * @param[in] segmentNumber Segment number
     */
    FWGDCMIO_API virtual void writeSegmentSequence(const CSPTR(::fwData::Reconstruction)& reconstruction,
                                                   ::gdcm::Item& segmentItem,
                                                   const ::gdcm::SmartPointer< ::gdcm::Segment >& segment,
                                                   unsigned short segmentNumber);
    /**
     * @brief Write Surface Item into Surface Sequence
     * @see PS 3.3 C.27.1
     * @param[in] reconstruction Reconstruction data
     * @param[in] surfaceItem GDCM surface item
     * @param[in] surface GDCM surface
     * @param[in] segmentNumber Associated segment number
     */
    FWGDCMIO_API virtual void writeSurfaceSequence(const CSPTR(::fwData::Reconstruction)& reconstruction,
                                                   ::gdcm::Item& surfaceItem,
                                                   const ::gdcm::SmartPointer< ::gdcm::Surface >& surface,
                                                   unsigned short segmentNumber);

    /// Structure Dictionary
    ::fwGdcmIO::helper::SegmentedPropertyRegistry m_segmentedPropertyRegistry;

    /// DICOM Image Instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_imageInstance;

};

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_WRITER_IE_SURFACE_HPP__ */
