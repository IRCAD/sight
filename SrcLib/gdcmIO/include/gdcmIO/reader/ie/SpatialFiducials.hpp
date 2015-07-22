/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IE_SPATIALFIDUCIALS_HPP__
#define __GDCMIO_READER_IE_SPATIALFIDUCIALS_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/reader/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @class SpatialFiducials
 * @brief Spatial Fiducials Information Entity class
 */
class GDCMIO_CLASS_API SpatialFiducials : public ::gdcmIO::reader::ie::InformationEntity< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] series Series data
     */
    GDCMIO_API SpatialFiducials(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                                SPTR(::gdcm::Reader)reader,
                                SPTR(::gdcmIO::container::DicomInstance)instance,
                                ::fwData::Image::sptr series);

    /// Destructor
    GDCMIO_API virtual ~SpatialFiducials();

    /**
     * @brief Read image landmarks from dataset
     */
    void readLandmark(const ::gdcm::DataSet& fiducialDataset);

    /**
     * @brief Read image distances from dataset
     */
    void readDistance(const ::gdcm::DataSet& fiducialDataset);


};

} // namespace ie
} // namespace reader
} // namespace gdcmIO

#endif // __GDCMIO_READER_IE_SPATIALFIDUCIALS_HPP__
