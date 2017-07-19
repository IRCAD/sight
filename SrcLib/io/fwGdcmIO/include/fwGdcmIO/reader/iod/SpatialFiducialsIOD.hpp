/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IOD_SPATIALFIDUCIALSIOD_HPP__
#define __FWGDCMIO_READER_IOD_SPATIALFIDUCIALSIOD_HPP__

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
     * @param[in] callback Progress callback
     * @param[in] cancelled cancel information
     */
    FWGDCMIO_API SpatialFiducialsIOD(::fwMedData::DicomSeries::sptr dicomSeries,
                                     SPTR(::fwGdcmIO::container::DicomInstance)instance, ::fwLog::Logger::sptr logger,
                                     ProgressCallback& callback, bool& cancelled);


    /// Destructor
    FWGDCMIO_API ~SpatialFiducialsIOD();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     */
    FWGDCMIO_API void read(::fwMedData::Series::sptr series);

};

}  // namespace iod
}  // namespace reader
}  // namespace fwGdcmIO

#endif /* __FWGDCMIO_READER_IOD_SPATIALFIDUCIALSIOD_HPP__ */
