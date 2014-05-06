/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IOD_SPATIALFIDUCIALSIOD_HPP__
#define __GDCMIO_READER_IOD_SPATIALFIDUCIALSIOD_HPP__

#include "gdcmIO/reader/iod/InformationObjectDefinition.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @class SpatialFiducialsIOD
 * @brief SpatialFiducialsIOD class used to read Spatial Fiducials IODs
 */
class GDCMIO_CLASS_API SpatialFiducialsIOD : public ::gdcmIO::reader::iod::InformationObjectDefinition
{
public :

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     */
    GDCMIO_API SpatialFiducialsIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
            SPTR(::gdcmIO::container::DicomInstance) instance);


    /// Destructor
    GDCMIO_API ~SpatialFiducialsIOD();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     */
    GDCMIO_API void read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed);

};

}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO

#endif /* __GDCMIO_READER_IOD_SPATIALFIDUCIALSIOD_HPP__ */
