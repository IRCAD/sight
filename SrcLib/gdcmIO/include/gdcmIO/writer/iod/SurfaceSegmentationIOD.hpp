/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_IOD_SURFACESEGMENTATIONIOD_HPP__
#define __GDCMIO_WRITER_IOD_SURFACESEGMENTATIONIOD_HPP__

#include "gdcmIO/container/DicomInstance.hpp"
#include "gdcmIO/writer/iod/InformationObjectDefinition.hpp"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace writer
{
namespace iod
{

/**
 * @class SurfaceSegmentationIOD
 * @brief SurfaceSegmentationIOD class used to write Surface Segmentation DICOM files
 */
class GDCMIO_CLASS_API SurfaceSegmentationIOD : public ::gdcmIO::writer::iod::InformationObjectDefinition
{

public:
    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share information between modules
     */
    GDCMIO_API SurfaceSegmentationIOD(SPTR(::gdcmIO::container::DicomInstance)instance,
                                      SPTR(::gdcmIO::container::DicomInstance)imageInstance,
                                      ::boost::filesystem::path folderPath);

    /// Destructor
    GDCMIO_API virtual ~SurfaceSegmentationIOD();

    /// Write DICOM file
    GDCMIO_API virtual void write(::fwMedData::Series::sptr series);

protected:
    /// Image instance
    SPTR(::gdcmIO::container::DicomInstance) m_imageInstance;

};

} // namespace iod
} // namespace writer
} // namespace gdcmIO

#endif // __GDCMIO_WRITER_IOD_SURFACESEGMENTATIONIOD_HPP__
