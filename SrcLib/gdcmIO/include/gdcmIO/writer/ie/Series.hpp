/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_IE_SERIES_HPP__
#define __GDCMIO_WRITER_IE_SERIES_HPP__

#include <fwMedData/Series.hpp>

#include "gdcmIO/writer/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @class Series
 * @brief Series Information Entity class
 */
class GDCMIO_CLASS_API Series : public ::gdcmIO::writer::ie::InformationEntity< ::fwMedData::Series >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] series Series data
     */
    GDCMIO_API Series(SPTR(::gdcm::Writer) writer,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            ::fwMedData::Series::sptr series);

    /// Destructor
    GDCMIO_API virtual ~Series();

    /**
     * @brief Write General Series Module tags
     * @see PS 3.3 C.7.3.1
     */
    GDCMIO_API virtual void writeGeneralSeriesModule();

    /**
     * @brief Write Segmentation Series Module tags
     * @see PS 3.3 C.8.20.1
     */
    GDCMIO_API virtual void writeSegmentationSeriesModule();

    /**
     * @brief Write SR Document Series Module tags
     * @see PS 3.3 C.17.1
     */
    GDCMIO_API virtual void writeSRDocumentSeriesModule();

    /**
     * @brief Write Spatial Fiducials Series Module tags
     * @see PS 3.3 C.21.1
     */
    GDCMIO_API virtual void writeSpatialFiducialsSeriesModule();

};

} // namespace ie
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_IE_SERIES_HPP__ */
