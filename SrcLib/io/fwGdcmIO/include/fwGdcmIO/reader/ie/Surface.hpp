/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IE_SURFACE_HPP__
#define __FWGDCMIO_READER_IE_SURFACE_HPP__

#include "fwGdcmIO/reader/ie/InformationEntity.hpp"

#include <fwData/StructureTraitsDictionary.hpp>
#include <fwMedData/ModelSeries.hpp>

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
     * @param[in] callback Progress callback
     * @param[in] cancelled cancel information
     */
    FWGDCMIO_API Surface(SPTR(::fwMedData::DicomSeries)dicomSeries,
                         SPTR(::gdcm::Reader)reader,
                         SPTR(::fwGdcmIO::container::DicomInstance)instance,
                         ::fwMedData::ModelSeries::sptr series,
                         ::fwLog::Logger::sptr logger,
                         const ProgressCallback& callback,
                         const bool& cancelled);

    /// Destructor
    FWGDCMIO_API virtual ~Surface();

    /**
     * @brief Read Surface Segmentation Module tags
     * @param[in] segment GDCM segment
     * @see PS 3.3 C.8.23.1
     */
    FWGDCMIO_API virtual void readSurfaceSegmentationModule(::gdcm::SmartPointer< ::gdcm::Segment > segment);

protected:
    /**
     * @brief Read Surface Mesh Module tags
     * @param[in] segment GDCM surface
     * @param[in] reconstruction Reconstruction data
     * @see PS 3.3 C.27.1
     */
    FWGDCMIO_API virtual void readSurfaceMeshModule(::gdcm::SmartPointer< ::gdcm::Surface > surface,
                                                    SPTR(::fwData::Reconstruction) reconstruction);

};

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_READER_IE_SURFACE_HPP__ */
