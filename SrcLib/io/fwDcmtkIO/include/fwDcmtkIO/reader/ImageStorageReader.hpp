/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwDcmtkIO/config.hpp"
#include "fwDcmtkIO/reader/IObjectReader.hpp"

#include <fwData/Image.hpp>

#include <fwTools/Type.hpp>

#include <dcmtk/dcmdata/dcdatset.h>

namespace fwDcmtkIO
{
namespace reader
{

/**
 * @brief Base class for Dicom instance reader.
 */
class FWDCMTKIO_CLASS_API ImageStorageReader : public ::fwDcmtkIO::reader::IObjectReader
{
public:
    typedef ::fwMedData::DicomSeries::DicomContainerType DicomContainerType;

    /// Constructor
    FWDCMTKIO_API ImageStorageReader();

    /// Destructor
    FWDCMTKIO_API virtual ~ImageStorageReader();

    /// Override
    FWDCMTKIO_API virtual ::fwMedData::Series::sptr read(const ::fwMedData::DicomSeries::csptr& series);

protected:
    /**
     * @brief Read an image using direct mode
     * @param[in] image Image
     * @param[in] instances Instances
     * @param[in] rows Rows
     * @param[in] columns Columns
     * @param[in] depth Depth
     * @param[in] rescaleSlope Rescale Slope
     * @param[in] rescaleIntercept Rescale Intercept
     * @param[in] pixelRepresentation Pixel representation
     * @param[in] imageType Image Type
     */
    FWDCMTKIO_API void directRead(const ::fwData::Image::sptr& image,
                                  DicomContainerType instances,
                                  unsigned short rows, unsigned short columns,
                                  int depth, double rescaleSlope,
                                  double rescaleIntercept,
                                  unsigned short pixelRepresentation,
                                  ::fwTools::Type imageType);

    /**
     * @brief Read an image using direct mode and perform a RGB lookup
     * @param[in] image Image
     * @param[in] dataset Dataset of the first instance
     * @param[in] instances Instances
     * @param[in] rows Rows
     * @param[in] columns Columns
     * @param[in] depth Depth
     * @param[in] bitsAllocated Number of bits allocated
     */
    FWDCMTKIO_API void directRGBLookupRead(const ::fwData::Image::sptr& image,
                                           DcmDataset& dataset,
                                           DicomContainerType instances,
                                           unsigned short rows,
                                           unsigned short columns, int depth,
                                           unsigned short bitsAllocated);

    /**
     * @brief Read an image using lazy mode
     * @param[in] image Image
     * @param[in] series Series
     * @param[in] rows Rows
     * @param[in] columns Columns
     * @param[in] depth Depth
     * @param[in] rescaleSlope Rescale Slope
     * @param[in] rescaleIntercept Rescale Intercept
     * @param[in] pixelRepresentation Pixel Representation
     * @param[in] imageType Image Type
     */
    FWDCMTKIO_API void lazyRead(const ::fwData::Image::sptr& image,
                                const ::fwMedData::DicomSeries::csptr& series,
                                unsigned short rows, unsigned short columns,
                                int depth, double rescaleSlope,
                                double rescaleIntercept,
                                unsigned short pixelRepresentation,
                                ::fwTools::Type imageType);

    /**
     * @brief Read an image using lazy mode and perform a RGB lookup
     * @param[in] image Image
     * @param[in] series Series
     * @param[in] dataset Dataset of the first instance
     * @param[in] instances Instances
     * @param[in] rows Rows
     * @param[in] columns Columns
     * @param[in] depth Depth
     * @param[in] bitsAllocated Bits allocated
     * @param[in] imageType Image type
     */
    FWDCMTKIO_API void lazyRGBLookupRead(const ::fwData::Image::sptr& image,
                                         const ::fwMedData::DicomSeries::csptr& series,
                                         DcmDataset& dataset,
                                         DicomContainerType instances, unsigned short rows,
                                         unsigned short columns,
                                         int depth, unsigned short bitsAllocated,
                                         ::fwTools::Type imageType);

};

} //reader
} //fwDcmtkIO
