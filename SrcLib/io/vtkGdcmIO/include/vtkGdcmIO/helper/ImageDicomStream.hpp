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

#include "vtkGdcmIO/config.hpp"

#include <fwCore/base.hpp>

#include <fwMemory/stream/in/IFactory.hpp>

#include <boost/iostreams/stream.hpp>

#include <vtkGDCMImageReader.h>
#include <vtkSmartPointer.h>

#include <vector>

namespace vtkGdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

/// Basic class to store some information to read an dicom image
class ImageDicomInfo
{

public:

    typedef std::vector< std::string > SeriesFilesType;

    typedef SPTR ( ImageDicomInfo ) sptr;

    /// Size of image buffer in bytes
    size_t m_buffSizeInBytes;

    /// Dicom files that represents an image series
    SeriesFilesType m_seriesFiles;
};

//------------------------------------------------------------------------------

/// Class used to build an std::istream thanks to ::boost::iostreams api
class ImageDicomSource
{
public:

    typedef char char_type;

    typedef ::boost::iostreams::source_tag category;

    /// Constructor
    ImageDicomSource( ImageDicomInfo::sptr dcmInfo );

    /// Method to read n bytes in dicom buffer and write it in s. On the first call, readImage() method is called.
    std::streamsize read(char* s, std::streamsize n);

private:

    /**
     * @brief Helper to read with vtk and gdcm a dicom image, sets m_reader, m_inputReader and m_success
     * if reading process is ok.
     */
    bool readImage();

    /// Data to store information
    ImageDicomInfo::sptr m_dcmInfo;

    /// to conserve the position in m_inputReader of bytes already read
    size_t m_pos;

    /// reader used to read dicom image buffer, it will reset when reading process will finished
    vtkSmartPointer< vtkGDCMImageReader > m_reader;

    /// direct pointer to raw dicom image buffer
    char* m_inputReader;

    /// to conserve if reading process is a succes
    bool m_success;
};

//------------------------------------------------------------------------------

/// Class to perform a lazy reading on dicom image with Sight system
class ImageDicomStream : public ::fwMemory::stream::in::IFactory
{

public:

    /// Builds the ::fwMemory::stream::in::IFactory with few dicom information
    ImageDicomStream( ImageDicomInfo::sptr dcmInfo );

protected:

    /// Returns the istream on dicom image buffer
    SPTR(std::istream) get();

    /// To conserve dicom information
    ImageDicomInfo::sptr m_dcmInfo;
};

//------------------------------------------------------------------------------

} // namespace helper
} // namespace vtkGdcmIO
