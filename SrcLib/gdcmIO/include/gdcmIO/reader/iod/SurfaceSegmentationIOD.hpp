/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2014.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IOD_SURFACESEGMENTATIONIOD_HPP__
#define __GDCMIO_READER_IOD_SURFACESEGMENTATIONIOD_HPP__

#include <gdcmSegment.h>

#include "gdcmIO/reader/iod/InformationObjectDefinition.hpp"

namespace fwData
{
    class Reconstruction;
}

namespace fwMedData
{
    class ModelSeries;
}

namespace gdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @class SurfaceSegmentationIOD
 * @brief SurfaceSegmentationIOD class used to read Surface Segmentation IODs
 */
class GDCMIO_CLASS_API SurfaceSegmentationIOD : public ::gdcmIO::reader::iod::InformationObjectDefinition
{

public :

    /// Constructor
   GDCMIO_API SurfaceSegmentationIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
           SPTR(::gdcmIO::container::DicomInstance) instance);

   /// Destructor
   GDCMIO_API virtual ~SurfaceSegmentationIOD();

   /**
    * @brief Read a DICOM File
    * @param[in,out] series Series that must be enriched
    */
  GDCMIO_API void read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed);

};

}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO

#endif /* __GDCMIO_READER_IOD_SURFACESEGMENTATIONIOD_HPP__ */
