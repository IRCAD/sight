/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IOD_SURFACESEGMENTATIONIOD_HPP__
#define __FWGDCMIO_READER_IOD_SURFACESEGMENTATIONIOD_HPP__

#include "fwGdcmIO/reader/iod/InformationObjectDefinition.hpp"

#include <gdcmSegment.h>

namespace fwData
{
class Reconstruction;
}

namespace fwMedData
{
class ModelSeries;
}

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @brief SurfaceSegmentationIOD class used to read Surface Segmentation IODs
 */
class FWGDCMIO_CLASS_API SurfaceSegmentationIOD : public ::fwGdcmIO::reader::iod::InformationObjectDefinition
{

public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     * @param[in] logger Logger
     * @param[in] callback Progress callback
     * @param[in] cancelled cancel information
     */
    FWGDCMIO_API SurfaceSegmentationIOD(::fwMedData::DicomSeries::sptr dicomSeries,
                                        SPTR(::fwGdcmIO::container::DicomInstance)instance,
                                        ::fwLog::Logger::sptr logger,
                                        ProgressCallback& callback, bool& cancelled);

    /// Destructor
    FWGDCMIO_API virtual ~SurfaceSegmentationIOD();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     */
    FWGDCMIO_API void read(::fwMedData::Series::sptr series) throw(::fwGdcmIO::exception::Failed);

};

}  // namespace iod
}  // namespace reader
}  // namespace fwGdcmIO

#endif /* __FWGDCMIO_READER_IOD_SURFACESEGMENTATIONIOD_HPP__ */
