/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IE_SPATIALFIDUCIALS_HPP__
#define __FWGDCMIO_READER_IE_SPATIALFIDUCIALS_HPP__

#include "fwGdcmIO/reader/ie/InformationEntity.hpp"

#include <fwData/Image.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @brief Spatial Fiducials Information Entity class
 */
class FWGDCMIO_CLASS_API SpatialFiducials : public ::fwGdcmIO::reader::ie::InformationEntity< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] series Series data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API SpatialFiducials(const SPTR(::fwMedData::DicomSeries)& dicomSeries,
                                  const SPTR(::gdcm::Reader)& reader,
                                  const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                  const ::fwData::Image::sptr& series,
                                  const ::fwLog::Logger::sptr& logger = nullptr,
                                  ProgressCallback progress = nullptr,
                                  CancelRequestedCallback cancel = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~SpatialFiducials();

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
} // namespace fwGdcmIO

#endif // __FWGDCMIO_READER_IE_SPATIALFIDUCIALS_HPP__
