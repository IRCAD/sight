/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_IOD_SURFACESEGMENTATIONIOD_HPP__
#define __FWGDCMIO_WRITER_IOD_SURFACESEGMENTATIONIOD_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"
#include "fwGdcmIO/writer/iod/InformationObjectDefinition.hpp"

#include <boost/filesystem/path.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace iod
{

/**
 * @brief SurfaceSegmentationIOD class used to write Surface Segmentation DICOM files
 */
class FWGDCMIO_CLASS_API SurfaceSegmentationIOD : public ::fwGdcmIO::writer::iod::InformationObjectDefinition
{

public:
    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share information between modules
     */
    FWGDCMIO_API SurfaceSegmentationIOD(SPTR(::fwGdcmIO::container::DicomInstance)instance,
                                        SPTR(::fwGdcmIO::container::DicomInstance)imageInstance,
                                        ::boost::filesystem::path folderPath);

    /// Destructor
    FWGDCMIO_API virtual ~SurfaceSegmentationIOD();

    /// Write DICOM file
    FWGDCMIO_API virtual void write(::fwMedData::Series::sptr series);

protected:
    /// Image instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_imageInstance;

};

} // namespace iod
} // namespace writer
} // namespace fwGdcmIO

#endif // __FWGDCMIO_WRITER_IOD_SURFACESEGMENTATIONIOD_HPP__
