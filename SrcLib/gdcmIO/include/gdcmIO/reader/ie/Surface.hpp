/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IE_SURFACE_HPP__
#define __GDCMIO_READER_IE_SURFACE_HPP__

#include <fwData/StructureTraitsDictionary.hpp>
#include <fwMedData/ModelSeries.hpp>

#include "gdcmIO/reader/ie/InformationEntity.hpp"

namespace fwData
{
    class Reconstruction;
}

namespace gdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @class Surface
 * @brief Surface Information Entity class
 */
class GDCMIO_CLASS_API Surface : public ::gdcmIO::reader::ie::InformationEntity< ::fwMedData::ModelSeries >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] series Series data
     */
    GDCMIO_API Surface(SPTR(::fwDicomData::DicomSeries) dicomSeries,
            SPTR(::gdcm::Reader) reader,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            ::fwMedData::ModelSeries::sptr series);

    /// Destructor
    GDCMIO_API virtual ~Surface();

    /**
     * @brief Read Surface Segmentation Module tags
     * @param[in] segment GDCM segment
     * @see PS 3.3 C.8.23.1
     */
    GDCMIO_API virtual void readSurfaceSegmentationModule(::gdcm::SmartPointer< ::gdcm::Segment > segment);

protected:
    /**
     * @brief Read Surface Mesh Module tags
     * @param[in] segment GDCM surface
     * @param[in] reconstruction Reconstruction data
     * @see PS 3.3 C.27.1
     */
    GDCMIO_API virtual void readSurfaceMeshModule(::gdcm::SmartPointer< ::gdcm::Surface > surface,
            SPTR(::fwData::Reconstruction) reconstruction);

};

} // namespace ie
} // namespace reader
} // namespace gdcmIO

#endif /* __GDCMIO_READER_IE_SURFACE_HPP__ */
