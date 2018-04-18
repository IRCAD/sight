/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDcmtkIO/reader/rgblookup/ImageRGBLookupLazyStream.hpp"

#include "fwDcmtkIO/reader/rgblookup/ImageRGBLookupLazyReader.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

namespace fwDcmtkIO
{
namespace reader
{
namespace rgblookup
{

//------------------------------------------------------------------------------

bool filesStillExist( const ::fwMedData::DicomSeries::csptr& dicomSeries )
{
    ::boost::filesystem::path filePath;
    bool allFilesExists = true;

    for(::fwMedData::DicomSeries::DicomPathContainerType::const_iterator itPath =
            dicomSeries->getLocalDicomPaths().begin();
        allFilesExists && ( itPath != dicomSeries->getLocalDicomPaths().end() );
        ++itPath )
    {
        filePath        = itPath->second;
        allFilesExists &= ::boost::filesystem::exists(filePath);
    }
    return allFilesExists;
}

//------------------------------------------------------------------------------

ImageRGBLookupLazySource::ImageRGBLookupLazySource( ImageRGBLookupLazyInformation::sptr dcmInfo ) :
    m_dcmInfo( dcmInfo )
{
    SLM_ASSERT( "ImageRGBLookupLazySource needs at least one dicom file to read an image.",
                !dcmInfo->m_dicomSeries->getLocalDicomPaths().empty());

    m_frameSize       = m_dcmInfo->m_rows*m_dcmInfo->m_columns*m_dcmInfo->m_imageType.sizeOf()*3;
    m_currentFrame    = 0;
    m_currentPosition = 0;
    m_currentPath     = m_dcmInfo->m_dicomSeries->getLocalDicomPaths().begin();

    // Read instance
    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    const std::string filename = m_dcmInfo->m_dicomSeries->getLocalDicomPaths().begin()->second.string();
    status = fileFormat.loadFile(filename.c_str());
    FW_RAISE_IF("Unable to read the file: \""+filename+"\"", status.bad());
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
                                         m_dcmInfo->m_rows, m_dcmInfo->m_columns, m_currentPath->second,
                                         (Uint16*)m_redLookup,
                                         (Uint16*)m_greenLookup, (Uint16*)m_blueLookup,
                                         m_dcmInfo->m_pixelValueBitsAllocated));
    }
    else
    {
        m_frame = static_cast<char*>(::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyReader::createInstanceBuffer(
                                         m_dcmInfo->m_rows, m_dcmInfo->m_columns, m_currentPath->second,
                                         (Uint8*)m_redLookup,
                                         (Uint8*)m_greenLookup, (Uint8*)m_blueLookup,
                                         m_dcmInfo->m_pixelValueBitsAllocated));
    }
}

//------------------------------------------------------------------------------

std::streamsize ImageRGBLookupLazySource::read(char* stream, std::streamsize size)
{
    std::streamsize result = -1;

    // Be sure that all files exist when reading the first frame
    if(m_currentFrame == 0 && m_currentPosition == 0)
    {
        bool exist = filesStillExist(m_dcmInfo->m_dicomSeries);
        OSLM_ERROR_IF( "Error while reading dicom files : " <<
                       m_dcmInfo->m_dicomSeries->getLocalDicomPaths().begin()->second.string() << " ...", !exist );
    }

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
        ++m_currentPath;
        m_currentPosition = 0;

        // If there is more frame
        if(m_currentPath != m_dcmInfo->m_dicomSeries->getLocalDicomPaths().end())
        {
            // Copy extra bytes from the new frame
            if(extraBytes > 0)
            {
                if(m_dcmInfo->m_bitsAllocated == 16)
                {
                    m_frame =
                        static_cast<char*>(::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyReader::
                                           createInstanceBuffer(
                                               m_dcmInfo->m_rows, m_dcmInfo->m_columns, m_currentPath->second,
                                               (Uint16*)m_redLookup,
                                               (Uint16*)m_greenLookup, (Uint16*)m_blueLookup,
                                               m_dcmInfo->m_pixelValueBitsAllocated));
                }
                else
                {
                    m_frame =
                        static_cast<char*>(::fwDcmtkIO::reader::rgblookup::ImageRGBLookupLazyReader::
                                           createInstanceBuffer(
                                               m_dcmInfo->m_rows, m_dcmInfo->m_columns, m_currentPath->second,
                                               (Uint8*)m_redLookup,
                                               (Uint8*)m_greenLookup, (Uint8*)m_blueLookup,
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

        if(m_currentPath->second == m_dcmInfo->m_dicomSeries->getLocalDicomPaths().rbegin()->second &&
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
