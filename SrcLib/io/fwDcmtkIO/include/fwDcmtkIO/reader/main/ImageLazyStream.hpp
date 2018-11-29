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
namespace main
{

//------------------------------------------------------------------------------

/**
 * @brief Basic class used to store informations in order to read DICOM images
 */
class FWDCMTKIO_CLASS_API ImageLazyInformation
{
public:
    typedef SPTR ( ImageLazyInformation ) sptr;

    /// Dicom series used to get the paths of the instances
    ::fwMedData::DicomSeries::csptr m_dicomSeries;

    /// Number of rows in the image
    unsigned int m_rows;

    /// Number of columns in the image
    unsigned int m_columns;

    /// Depth of the image
    unsigned int m_depth;

    /// Rescale slope
    double m_rescaleSlope;

    /// Rescale intercept
    double m_rescaleIntercept;

    /// Pixel representation
    unsigned short m_pixelRepresentation;

    /// Image type
    ::fwTools::Type m_imageType;

};

//------------------------------------------------------------------------------

/**
 * @brief Class used to build a std::istream using the ::boost::iostreams api
 */
class FWDCMTKIO_CLASS_API ImageLazySource
{
public:
    typedef char char_type;
    typedef ::boost::iostreams::source_tag category;

    /**
     * @brief Constructor
     * @param[in] dcmInfo DICOM information
     */
    ImageLazySource( ImageLazyInformation::sptr dcmInfo );

    /**
     * @brief Method to read n bytes in dicom buffer and write it in s.
     * @param[in] n Number of bytes
     * @param[out] s Destination buffer
     */
    std::streamsize read(char* s, std::streamsize n);

private:
    /// DICOM information
    ImageLazyInformation::sptr m_dcmInfo;

    /// Frame size
    size_t m_frameSize;

    /// Current frame
    int m_currentFrame;

    /// Current position in the frame
    size_t m_currentPosition;

    /// Current frame
    char* m_frame;

    /// Current Dicom item
    ::fwMedData::DicomSeries::DicomContainerType::const_iterator m_currentDicom;
};

//------------------------------------------------------------------------------

/**
 * @brief Class used to perform a lazy reading of dicom images with Sight system
 */
class FWDCMTKIO_CLASS_API ImageLazyStream : public ::fwMemory::stream::in::IFactory
{

public:

    /**
     * @brief Constructor. Builds the ::fwMemory::stream::in::IFactory with few dicom information
     * @param[in] dcmInfo DICOM information
     */
    ImageLazyStream( ImageLazyInformation::sptr dcmInfo );

protected:

    /// Returns the istream on dicom image buffer
    SPTR(std::istream) get();

    /// To conserve dicom information
    ImageLazyInformation::sptr m_dcmInfo;
};

//------------------------------------------------------------------------------

} //main
} //reader
} //fwDcmtkIO
