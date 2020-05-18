/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
