/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <gdcmDirectory.h>
#include <gdcmScanner.h>
#include <gdcmAttribute.h>

#include <fwMedData/ImageSeries.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include "gdcmIO/reader/SeriesDB.hpp"
#include "gdcmIO/reader/dicom/DicomSeries.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

fwDataIOReaderRegisterMacro( ::gdcmIO::reader::SeriesDB );

namespace gdcmIO
{

namespace reader
{

//------------------------------------------------------------------------------

SeriesDB::SeriesDB(::fwDataIO::reader::IObjectReader::Key key) :
            ::fwData::location::enableFolder< IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SeriesDB::read() throw(::gdcmIO::exception::Failed)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();

    MapSeriesType mapSeries;
    this->sortSeriesFiles(this->getFolder().string(), mapSeries);


    BOOST_FOREACH(const MapSeriesType::value_type elt, mapSeries)
    {
        try
        {
            ::gdcmIO::reader::dicom::DicomSeries seriesReader;
            seriesReader.setFileNames(elt.second);
            seriesReader.setObject(seriesDB);
            // Read one series
            seriesReader.read();
        }
        catch (::gdcmIO::exception::Failed & e)
        {
            OSLM_ERROR("Read error with patient : " << elt.first);
        }
    }
}

//------------------------------------------------------------------------------

void SeriesDB::sortSeriesFiles(const std::string& folder, MapSeriesType& mapSeries)
{
    const gdcm::Tag seriesUIDTag(0x0020,0x000e);
    const gdcm::Tag acquisitionDateTag(0x0008,0x0022);
    const gdcm::Tag imageTypeTag(0x0008,0x0008);

    ::gdcm::Directory dir;
    dir.Load(folder, true); // Get all files from specified folder and its sub folders

    gdcm::Scanner scanner;
    scanner.AddTag( seriesUIDTag );
    scanner.AddTag( acquisitionDateTag );
    scanner.AddTag( imageTypeTag );
    try
    {
         //Scan all files
        FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("No Series found"), !scanner.Scan( dir.GetFilenames() ) );
        const gdcm::Directory::FilenamesType& keys = scanner.GetKeys();

        BOOST_FOREACH(const gdcm::Directory::FilenamesType::value_type& elt, keys)
        {
            const char *filename = elt.c_str();
            OSLM_ASSERT("'"<<filename<<"' isn't a key in the Mapping table.", scanner.IsKey( filename ) );

            const char *seriesUID =  scanner.GetValue( filename, seriesUIDTag );
            const char *acqDate   =  scanner.GetValue( filename, acquisitionDateTag );

            if (seriesUID)
            {
                std::string fileSetId = seriesUID;

                if (acqDate)
                {
                    fileSetId += "_";
                    fileSetId += acqDate;
                }

//                // Problem on sorting SR files
//                const char *imageTypeStr = scanner.GetValue(filename, imageTypeTag);
//                if(imageTypeStr)
//                {
//                    // Treatment of secondary capture dicom file.
//                    std::string imageType(imageTypeStr);
//                    OSLM_TRACE("Image Type : " << imageType);
//
//                    fileSetId += "_";
//                    fileSetId += imageTypeStr;
//                }
                mapSeries[fileSetId].push_back(filename);
            }
            else
            {
                OSLM_ERROR( "No series name found in : " << filename );
            }

        }
    }
    catch (std::exception& e)
    {
        OSLM_ERROR("Error : " << e.what() );
    }
}

}  // namespace reader

}  // namespace gdcmIO

