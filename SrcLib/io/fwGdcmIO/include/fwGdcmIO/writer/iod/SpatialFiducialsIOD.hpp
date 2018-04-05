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
 * @brief SpatialFiducialsIOD class used to write Spacial Fiducials DICOM files
 */
class FWGDCMIO_CLASS_API SpatialFiducialsIOD : public ::fwGdcmIO::writer::iod::InformationObjectDefinition
{

public:
    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] destinationPath Destination path
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API SpatialFiducialsIOD(const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                     const ::boost::filesystem::path& destinationPath,
                                     const ::fwLog::Logger::sptr& logger = nullptr,
                                     ProgressCallback progress           = nullptr,
                                     CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~SpatialFiducialsIOD();

    /// Write DICOM file
    FWGDCMIO_API virtual void write(const ::fwMedData::Series::csptr& series) override;

};

} // namespace iod
} // namespace writer
} // namespace fwGdcmIO
