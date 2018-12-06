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

#include "fwDcmtkIO/reader/rgblookup/ImageRGBLookupLazyStream.hpp"

#include "fwDcmtkIO/reader/rgblookup/ImageRGBLookupLazyReader.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#include <dcmtk/dcmdata/dcistrmb.h>

namespace fwDcmtkIO
{
namespace reader
{
namespace rgblookup
{

//------------------------------------------------------------------------------

ImageRGBLookupLazySource::ImageRGBLookupLazySource( ImageRGBLookupLazyInformation::sptr dcmInfo ) :
    m_dcmInfo( dcmInfo )
{
    SLM_ASSERT( "ImageRGBLookupLazySource needs at least one dicom item to read an image.",
                !dcmInfo->m_dicomSeries->getDicomContainer().empty());

    m_frameSize       = m_dcmInfo->m_rows * m_dcmInfo->m_columns * m_dcmInfo->m_imageType.sizeOf() * 3;
    m_currentFrame    = 0;
    m_currentPosition = 0;
    m_currentDicom    = m_dcmInfo->m_dicomSeries->getDicomContainer().begin();

    // Read instance
    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    // Open first instance
    const auto firstItem                           = m_dcmInfo->m_dicomSeries->getDicomContainer().begin();
    const ::fwMemory::BufferObject::sptr bufferObj = firstItem->second;
    const size_t buffSize                          = bufferObj->getSize();
    const std::string dicomPath                    = bufferObj->getStreamInfo().fsFile.string();
    ::fwMemory::BufferObject::Lock lock(bufferObj);
    char* buffer = static_cast< char* >( lock.getBuffer() );

    DcmInputBufferStream is;
    is.setBuffer(buffer, offile_off_t(buffSize));
    is.setEos();

    fileFormat.transferInit();
    if (!fileFormat.read(is).good())
    {
        FW_RAISE("Unable to read Dicom file '"<< dicomPath <<"' "<<
                 "(slice: '" << firstItem->first << "')");
    }

    fileFormat.loadAllDataIntoMemory();
    fileFormat.transferEnd();

    dataset = fileFormat.getDataset();

    if(m_dcmInfo->m_bitsAllocated == 16)
    {
        const Uint16* redLookup;
        const Uint16* greenLookup;
        const Uint16* blueLookup;
        // Those values are freed by the dataset destructor
        dataset->findAndGetUint16Array(DCM_RedPaletteColorLookupTableData, redLookup);
        dataset->findAndGetUint16Array(DCM_GreenPaletteColorLookupTableData, greenLookup);
        dataset->findAndGetUint16Array(DCM_BluePaletteColorLookupTableData, blueLookup);
        m_redLookup   = (void*) redLookup;
        m_greenLookup = (void*) greenLookup;
        m_blueLookup  = (void*) blueLookup;
    }
    else
    {
        const Uint8* redLookup;
        const Uint8* greenLookup;
        const Uint8* blueLookup;
        // Those values are freed by the dataset destructor
        dataset->findAndGetUint8Array(DCM_RedPaletteColorLookupTableData, redLookup);
        dataset->findAndGetUint8Array(DCM_GreenPaletteColorLookupTableData, greenLookup);
        dataset->findAndGetUint8Array(DCM_BluePaletteColorLookupTableData, blueLookup);
        m_redLookup   = (void*) redLookup;
        m_greenLookup = (void*) greenLookup;
        m_blueLookup  = (void*) blueLookup;
    }

    if(m_dcmInfo->m_bitsAllocated == 16)
    {
        m_frame = static_cast<char*>(::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyReader::createInstanceBuffer(
                                         m_dcmInfo->m_rows, m_dcmInfo->m_columns,
                                         m_currentDicom->second,
                                         (Uint16*)m_redLookup,
                                         (Uint16*)m_greenLookup, (Uint16*)m_blueLookup,
                                         m_dcmInfo->m_pixelValueBitsAllocated));
    }
    else
    {
        m_frame = static_cast<char*>(::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyReader::createInstanceBuffer(
                                         m_dcmInfo->m_rows, m_dcmInfo->m_columns,
                                         m_currentDicom->second,
                                         (Uint8*)m_redLookup,
                                         (Uint8*)m_greenLookup, (Uint8*)m_blueLookup,
                                         m_dcmInfo->m_pixelValueBitsAllocated));
    }
}

//------------------------------------------------------------------------------

std::streamsize ImageRGBLookupLazySource::read(char* stream, std::streamsize size)
{
    std::streamsize result = -1;

    // Load new frame
    if(m_currentPosition + size > m_frameSize)
    {
        // Copy remaining bytes
        const size_t remainingBytes = m_frameSize - m_currentPosition;
        const size_t extraBytes     = size - remainingBytes;

        if(remainingBytes > 0)
        {
            memcpy(stream, m_frame + m_currentPosition, remainingBytes);
            result = remainingBytes;
        }

        // Change frame
        delete m_frame;
        ++m_currentFrame;
        ++m_currentDicom;
        m_currentPosition = 0;

        // If there is more frame
        if(m_currentDicom != m_dcmInfo->m_dicomSeries->getDicomContainer().end())
        {
            // Copy extra bytes from the new frame
            if(extraBytes > 0)
            {
                if(m_dcmInfo->m_bitsAllocated == 16)
                {
                    m_frame =
                        static_cast<char*>(::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyReader::
                                           createInstanceBuffer(
                                               m_dcmInfo->m_rows, m_dcmInfo->m_columns,
                                               m_currentDicom->second,
                                               (Uint16*)m_redLookup,
                                               (Uint16*)m_greenLookup,
                                               (Uint16*)m_blueLookup,
                                               m_dcmInfo->m_pixelValueBitsAllocated));
                }
                else
                {
                    m_frame =
                        static_cast<char*>(::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyReader::
                                           createInstanceBuffer(
                                               m_dcmInfo->m_rows, m_dcmInfo->m_columns,
                                               m_currentDicom->second,
                                               (Uint8*)m_redLookup,
                                               (Uint8*)m_greenLookup,
                                               (Uint8*)m_blueLookup,
                                               m_dcmInfo->m_pixelValueBitsAllocated));
                }

                memcpy(stream, m_frame + m_currentPosition, extraBytes);
                m_currentPosition += extraBytes;
                result             = remainingBytes + extraBytes;
            }
        }
        else
        {
            SLM_TRACE("Reading process over.");
        }
    }
    // Copy bytes from the loaded frame
    else
    {
        memcpy(stream, m_frame + m_currentPosition, size);
        m_currentPosition += size;
        result             = size;

        if(m_currentDicom->second == m_dcmInfo->m_dicomSeries->getDicomContainer().rbegin()->second &&
           m_currentPosition == m_frameSize)
        {
            SLM_TRACE("Reading process over.");
            delete m_frame;
        }
    }
    return result;
}

//------------------------------------------------------------------------------

ImageRGBLookupLazyStream::ImageRGBLookupLazyStream( ImageRGBLookupLazyInformation::sptr dcmInfo ) :
    m_dcmInfo( dcmInfo )
{
}

//------------------------------------------------------------------------------

SPTR(std::istream) ImageRGBLookupLazyStream::get()
{
    SPTR(::boost::iostreams::stream<ImageRGBLookupLazySource>) is
        = std::make_shared< ::boost::iostreams::stream<ImageRGBLookupLazySource> >( m_dcmInfo );
    return is;
}

//------------------------------------------------------------------------------

} //rgblookup
} //reader
} //fwDcmtkIO
