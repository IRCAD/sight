/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGdcmIO/reader/iod/InformationObjectDefinition.hpp"

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @brief SpatialFiducialsIOD class used to read Spatial Fiducials IODs
 */
class FWGDCMIO_CLASS_API SpatialFiducialsIOD : public ::fwGdcmIO::reader::iod::InformationObjectDefinition
{
public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API SpatialFiducialsIOD(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                                     const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                     const ::fwLog::Logger::sptr& logger = nullptr,
                                     ProgressCallback progress           = nullptr,
                                     CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API ~SpatialFiducialsIOD();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     */
    FWGDCMIO_API void read(::fwMedData::Series::sptr series) throw(::fwGdcmIO::exception::Failed);

};

}  // namespace iod
}  // namespace reader
}  // namespace fwGdcmIO
