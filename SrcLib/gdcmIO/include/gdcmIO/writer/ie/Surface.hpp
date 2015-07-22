/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_IE_SURFACE_HPP__
#define __GDCMIO_WRITER_IE_SURFACE_HPP__

#include <fwData/StructureTraitsDictionary.hpp>
#include <fwMedData/ModelSeries.hpp>

#include "gdcmIO/writer/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @class Surface
 * @brief Surface Information Entity class
 */
class GDCMIO_CLASS_API Surface : public ::gdcmIO::writer::ie::InformationEntity< ::fwMedData::ModelSeries >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] series Series data
     */
    GDCMIO_API Surface(SPTR(::gdcm::Writer)writer,
                       SPTR(::gdcmIO::container::DicomInstance)instance,
                       SPTR(::gdcmIO::container::DicomInstance)imageInstance,
                       ::fwMedData::ModelSeries::sptr series);

    /// Destructor
    GDCMIO_API virtual ~Surface();

    /**
     * @brief Write Surface Segmentation Module tags
     * @see PS 3.3 C.8.23.1
     */
    GDCMIO_API virtual void writeSurfaceSegmentationModule(unsigned int segmentationNumber);

    /**
     * @brief Write Surface Mesh Module tags
     * @see PS 3.3 C.27.1
     */
    GDCMIO_API virtual void writeSurfaceMeshModule(unsigned int segmentationNumber);

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    GDCMIO_API virtual void writeSOPCommonModule();

protected:

    /**
     * @brief Write Content Identification Macro
     * @see PS 3.3 C.8.23.1 and C.10.9
     */
    void writeContentIdentificationMacro(unsigned int segmentationNumber);

    /// Structure Dictionary
    ::fwData::StructureTraitsDictionary::sptr m_structureDictionary;

    /// DICOM Image Instance
    SPTR(::gdcmIO::container::DicomInstance) m_imageInstance;

};

} // namespace ie
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_IE_SURFACE_HPP__ */
