/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IE_SURFACE_HPP__
#define __FWGDCMIO_WRITER_IE_SURFACE_HPP__

#include "fwGdcmIO/writer/ie/InformationEntity.hpp"
#include <fwData/StructureTraitsDictionary.hpp>
#include "fwGdcmIO/helper/SegmentedPropertyRegistry.hpp"

#include <fwMedData/ModelSeries.hpp>

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
     * @see PS 3.3 C.8.23.1
     */
    FWGDCMIO_API virtual void writeSurfaceSegmentationModule(unsigned short segmentationNumber);

    /**
     * @brief Write Surface Mesh Module tags
     * @see PS 3.3 C.27.1
     */
    FWGDCMIO_API virtual void writeSurfaceMeshModule(unsigned short segmentationNumber);


protected:

    /**
     * @brief Write Content Identification Macro
     * @see PS 3.3 C.8.23.1 and C.10.9
     */
    void writeContentIdentificationMacro(unsigned int segmentationNumber);

    /// Structure Dictionary
    ::fwData::StructureTraitsDictionary::sptr m_structureDictionary;

    /// Structure Dictionary
    ::fwGdcmIO::helper::SegmentedPropertyRegistry m_segmentedPropertyRegistry;

    /// DICOM Image Instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_imageInstance;

};

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_WRITER_IE_SURFACE_HPP__ */
