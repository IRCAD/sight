/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmScanner.h>

#include "gdcmIO/reader/DicomRTReader.hpp"
#include "gdcmIO/reader/DicomSurfaceReader.hpp"

namespace gdcmIO
{

namespace reader
{

//------------------------------------------------------------------------------

DicomRTReader::DicomRTReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomRTReader::~DicomRTReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomRTReader::read() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Acquisition::sptr series = this->getConcreteObject();
    SLM_ASSERT("fwData::Acquisition not instanced", series);

    //*****     Get all file names      *****//
    std::vector< std::string >  docRT   = this->getFileNames();
    OSLM_TRACE("Number of RT files : " << docRT.size());

    //*****     Get group of file names for each kind of RT document      *****//
    // NOTE : Currently, just search surface segmentation storage
    const ::gdcm::Tag   gTagSSQ(0x0066,0x0002); // Surface Sequence
    gdcm::Scanner       gScanner;
    gScanner.AddTag(gTagSSQ);

    if( !gScanner.Scan( docRT ) )
    {
        throw ::fwTools::Failed("No RT content found");
    }

    // Get Surface Segmentation Storage file names
    const std::vector< std::string > surfaceFileNames = getSurfaceStorageFileNames( gScanner );
    if (surfaceFileNames.size() > 0)
    {
        // Create a Surface Segmentation reader
        DicomSurfaceReader surfaceReader;
        surfaceReader.setObject( series );
        surfaceReader.setFileNames( surfaceFileNames );

        // Read surface(s) and add it to fwData::Reconstruction
        try
        {
            surfaceReader.read();
        }
        catch (::fwTools::Failed &e)
        {
            OSLM_ERROR("Surface reading failed: "<<e.what());
        }
    }
    else
    {
        throw ::fwTools::Failed("No surface segmentation storage found");
    }
}

//------------------------------------------------------------------------------

std::vector< std::string > DicomRTReader::getSurfaceStorageFileNames( ::gdcm::Scanner const & a_gScanner )
{
    std::vector< std::string > res;

    const ::gdcm::Tag gTagSSQ(0x0066,0x0002);   // Surface Sequence;

    ::gdcm::Directory::FilenamesType                    keys    = a_gScanner.GetKeys();   // Get file names
    ::gdcm::Directory::FilenamesType::const_iterator    it      = keys.begin();
    ::gdcm::Directory::FilenamesType::const_iterator    itEnd   = keys.end();
    // For each file with a surface sequence
    for(; it != itEnd; ++it)
    {
        const char * filename           = it->c_str();
        const char * surfaceSequence    = a_gScanner.GetValue(filename, gTagSSQ);
        if (surfaceSequence != 0)
        {
            res.push_back( filename );
        }
        else
        {
            OSLM_TRACE ( "No surface sequence found in : " << filename );
        }
    }

    return res;
}

}  // namespace reader

}  // namespace gdcmIO

