/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_READER_IMAGESTORAGEREADER_HPP__
#define __FWDICOMIOEXT_DCMTK_READER_IMAGESTORAGEREADER_HPP__

#include <fwTools/Type.hpp>

#include <fwData/Image.hpp>

#include "fwDicomIOExt/config.hpp"
#include "fwDicomIOExt/dcmtk/reader/IObjectReader.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace reader
{

/**
 * @class IObjectReader
 * @brief Base class for Dicom instance reader.
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API ImageStorageReader : public ::fwDicomIOExt::dcmtk::reader::IObjectReader
{
public:
    typedef ::fwDicomData::DicomSeries::DicomPathContainerType DicomPathContainerType;

    /// Constructor
    FWDICOMIOEXT_API ImageStorageReader();

    /// Destructor
    FWDICOMIOEXT_API virtual ~ImageStorageReader();

    /// Override
    FWDICOMIOEXT_API virtual ::fwMedData::Series::sptr read(::fwDicomData::DicomSeries::sptr series);

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
    FWDICOMIOEXT_API void directRead(::fwData::Image::sptr image, DicomPathContainerType instances,
            unsigned short rows, unsigned short columns, int depth, double rescaleSlope, double rescaleIntercept,
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
    FWDICOMIOEXT_API void directRGBLookupRead(::fwData::Image::sptr image, DcmDataset& dataset,
            DicomPathContainerType instances, unsigned short rows, unsigned short columns, int depth,
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
    FWDICOMIOEXT_API void lazyRead(::fwData::Image::sptr image, ::fwDicomData::DicomSeries::sptr series,
            unsigned short rows, unsigned short columns, int depth, double rescaleSlope, double rescaleIntercept,
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
    FWDICOMIOEXT_API void lazyRGBLookupRead(::fwData::Image::sptr image, ::fwDicomData::DicomSeries::sptr series,
            DcmDataset& dataset, DicomPathContainerType instances, unsigned short rows, unsigned short columns,
            int depth, unsigned short bitsAllocated, ::fwTools::Type imageType);




};

} //dcmtk
} //reader
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_DCMTK_READER_IMAGESTORAGEREADER_HPP__ */
