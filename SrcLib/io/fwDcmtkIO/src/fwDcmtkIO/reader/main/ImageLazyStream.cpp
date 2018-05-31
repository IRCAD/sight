/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDcmtkIO/reader/main/ImageLazyStream.hpp"

#include "fwDcmtkIO/reader/main/ImageLazyReader.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

namespace fwDcmtkIO
{
namespace reader
{
namespace main
{

//------------------------------------------------------------------------------

ImageLazySource::ImageLazySource( ImageLazyInformation::sptr dcmInfo ) :
    m_dcmInfo( dcmInfo )
{
    SLM_ASSERT( "ImageLazySource needs at least one dicom file to read an image.",
                !dcmInfo->m_dicomSeries->getDicomContainer().empty());

    m_frameSize       = m_dcmInfo->m_rows * m_dcmInfo->m_columns * m_dcmInfo->m_imageType.sizeOf();
    m_currentFrame    = 0;
    m_currentPosition = 0;
    m_currentDicom    = m_dcmInfo->m_dicomSeries->getDicomContainer().begin();

    m_frame = static_cast<char*>(::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer(m_dcmInfo->m_rows,
                                                                                                  m_dcmInfo->m_columns,
                                                                                                  m_currentDicom->second,
                                                                                                  m_dcmInfo->
                                                                                                  m_rescaleSlope,
                                                                                                  m_dcmInfo->
                                                                                                  m_rescaleIntercept,
                                                                                                  m_dcmInfo->
                                                                                                  m_pixelRepresentation,
                                                                                                  m_dcmInfo->m_imageType));
}

//------------------------------------------------------------------------------

std::streamsize ImageLazySource::read(char* s, std::streamsize n)
{
    std::streamsize result = -1;

    // Load new frame
    if(m_currentPosition + n > m_frameSize)
    {
        // Copy remaining bytes
        const size_t remainingBytes = m_frameSize - m_currentPosition;
        const size_t extraBytes     = n - remainingBytes;

        if(remainingBytes > 0)
        {
            memcpy(s, m_frame + m_currentPosition, remainingBytes);
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
                m_frame = static_cast<char*>(::fwDcmtkIO::reader::main::ImageLazyReader::createInstanceBuffer(
                                                 m_dcmInfo->m_rows, m_dcmInfo->m_columns, m_currentDicom->second,
                                                 m_dcmInfo->m_rescaleSlope, m_dcmInfo->m_rescaleIntercept,
                                                 m_dcmInfo->m_pixelRepresentation, m_dcmInfo->m_imageType));

                memcpy(s, m_frame + m_currentPosition, extraBytes);
                m_currentPosition += extraBytes;
                result             = remainingBytes + extraBytes;
            }
        }
        else
        {
            OSLM_TRACE("Reading process over.");
        }

    }
    // Copy bytes from the loaded frame
    else
    {
        memcpy(s, m_frame + m_currentPosition, n);
        m_currentPosition += n;
        result             = n;

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

ImageLazyStream::ImageLazyStream( ImageLazyInformation::sptr dcmInfo ) :
    m_dcmInfo( dcmInfo )
{
}

//------------------------------------------------------------------------------

SPTR(std::istream) ImageLazyStream::get()
{
    SPTR(::boost::iostreams::stream<ImageLazySource>) is
        = std::make_shared< ::boost::iostreams::stream<ImageLazySource> >( m_dcmInfo );
    return is;
}

//------------------------------------------------------------------------------

} //main
} //reader
} //fwDcmtkIO
