/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

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
     * @param[in] imageInstance Associated image instance
     * @param[in] destinationPath Destination path
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API SurfaceSegmentationIOD(const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                        const SPTR(::fwGdcmIO::container::DicomInstance)& imageInstance,
                                        const ::boost::filesystem::path& destinationPath,
                                        const ::fwLog::Logger::sptr& logger = ::fwLog::Logger::New(),
                                        ProgressCallback progress           = nullptr,
                                        CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~SurfaceSegmentationIOD();

    /// Write DICOM file
    FWGDCMIO_API virtual void write(const ::fwMedData::Series::csptr& series) override;

protected:
    /// Image instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_imageInstance;

};

} // namespace iod
} // namespace writer
} // namespace fwGdcmIO
