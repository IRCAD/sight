/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDCMTKIO_READER_IMAGESTORAGEREADER_HPP__
#define __FWDCMTKIO_READER_IMAGESTORAGEREADER_HPP__

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
    typedef ::fwMedData::DicomSeries::DicomPathContainerType DicomPathContainerType;

    /// Constructor
    FWDCMTKIO_API ImageStorageReader();

    /// Destructor
    FWDCMTKIO_API virtual ~ImageStorageReader();

    /// Override
    FWDCMTKIO_API virtual ::fwMedData::Series::sptr read(::fwMedData::DicomSeries::sptr series);

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
    FWDCMTKIO_API void directRead(::fwData::Image::sptr image, DicomPathContainerType instances,
                                  unsigned short rows, unsigned short columns, int depth, double rescaleSlope,
                                  double rescaleIntercept,
                                  unsigned short pixelRepresentation, ::fwTools::Type imageType);

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
    FWDCMTKIO_API void directRGBLookupRead(::fwData::Image::sptr image, DcmDataset& dataset,
                                           DicomPathContainerType instances, unsigned short rows,
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
    FWDCMTKIO_API void lazyRead(::fwData::Image::sptr image, ::fwMedData::DicomSeries::sptr series,
                                unsigned short rows, unsigned short columns, int depth, double rescaleSlope,
                                double rescaleIntercept,
                                unsigned short pixelRepresentation, ::fwTools::Type imageType);

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
    FWDCMTKIO_API void lazyRGBLookupRead(::fwData::Image::sptr image, ::fwMedData::DicomSeries::sptr series,
                                         DcmDataset& dataset, DicomPathContainerType instances, unsigned short rows,
                                         unsigned short columns,
                                         int depth, unsigned short bitsAllocated, ::fwTools::Type imageType);




};

} //reader
} //fwDcmtkIO


#endif /* __FWDCMTKIO_READER_IMAGESTORAGEREADER_HPP__ */
