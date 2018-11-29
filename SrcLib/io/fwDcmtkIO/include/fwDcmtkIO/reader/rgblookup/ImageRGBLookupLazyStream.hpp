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

#include <fwCore/base.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <fwMemory/stream/in/IFactory.hpp>

#include <fwTools/Type.hpp>

#include <boost/iostreams/stream.hpp>

#include <vector>

namespace fwDcmtkIO
{
namespace reader
{
namespace rgblookup
{

//------------------------------------------------------------------------------

/**
 * @brief Basic class used to store informations in order to read DICOM images
 */
class FWDCMTKIO_CLASS_API ImageRGBLookupLazyInformation
{
public:
    typedef SPTR ( ImageRGBLookupLazyInformation ) sptr;

    /// Dicom series used to get the paths of the instances
    ::fwMedData::DicomSeries::csptr m_dicomSeries;

    /// Number of rows in the image
    unsigned int m_rows;

    /// Number of columns in the image
    unsigned int m_columns;

    /// Depth of the image
    unsigned int m_depth;

    /// Bits allocated
    unsigned short m_bitsAllocated;

    /// Pixel value bits allocated
    unsigned short m_pixelValueBitsAllocated;

    /// Image type
    ::fwTools::Type m_imageType;

};

//------------------------------------------------------------------------------

/**
 * @brief Class used to build a std::istream using the ::boost::iostreams api
 */
class FWDCMTKIO_CLASS_API ImageRGBLookupLazySource
{
public:
    typedef char char_type;
    typedef ::boost::iostreams::source_tag category;

    /**
     * @brief Constructor
     * @param[in] dcmInfo DICOM information
     */
    ImageRGBLookupLazySource( ImageRGBLookupLazyInformation::sptr dcmInfo );

    /**
     * @brief Method to read n bytes in dicom buffer and write it in s.
     * @param[in] size Number of bytes
     * @param[out] stream Destination buffer
     */
    std::streamsize read(char* stream, std::streamsize size);

private:
    /// DICOM information
    ImageRGBLookupLazyInformation::sptr m_dcmInfo;

    /// Frame size
    size_t m_frameSize;

    /// Current frame
    int m_currentFrame;

    /// Current position in the frame
    size_t m_currentPosition;

    /// Current frame
    char* m_frame;

    /// Current path
    ::fwMedData::DicomSeries::DicomContainerType::const_iterator m_currentDicom;

    /// Red lookup table
    void* m_redLookup;

    /// Green lookup table
    void* m_greenLookup;

    /// Blue lookup table
    void* m_blueLookup;

};

//------------------------------------------------------------------------------

/**
 * @brief Class used to perform a lazy reading of dicom images with fw4spl system
 */
class FWDCMTKIO_CLASS_API ImageRGBLookupLazyStream : public ::fwMemory::stream::in::IFactory
{

public:

    /**
     * @brief Constructor. Builds the ::fwMemory::stream::in::IFactory with few dicom information
     * @param[in] dcmInfo DICOM information
     */
    ImageRGBLookupLazyStream( ImageRGBLookupLazyInformation::sptr dcmInfo );

protected:

    /// Returns the istream on dicom image buffer
    SPTR(std::istream) get();

    /// To conserve dicom information
    ImageRGBLookupLazyInformation::sptr m_dcmInfo;
};

//------------------------------------------------------------------------------

} //rgblookup
} //reader
} //fwDcmtkIO
