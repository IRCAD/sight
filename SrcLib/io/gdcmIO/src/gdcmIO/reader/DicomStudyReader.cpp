/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmDirectory.h>
#include <gdcmScanner.h>
#include <gdcmAttribute.h>

#include <fwTools/fromIsoExtendedString.hpp>

#include "gdcmIO/reader/DicomStudyReader.hpp"
#include "gdcmIO/reader/DicomSeriesReader.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{

namespace reader
{

//------------------------------------------------------------------------------

DicomStudyReader::DicomStudyReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomStudyReader::~DicomStudyReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomStudyReader::readStudy()
{
    SLM_TRACE_FUNC();

    // List of tag use in this method
//    const ::gdcm::Tag sDateTag(0x0008,0x0020);  // Study Date
//    const ::gdcm::Tag sTimeTag(0x0008,0x0030);  // Study Time
//    const ::gdcm::Tag sModTag(0x0008,0x0060);   // Modality
//    const ::gdcm::Tag sINameTag(0x0008,0x0080); // Institution Name
//    const ::gdcm::Tag sDescTag(0x0008,0x1030);  // Study Description
//    const ::gdcm::Tag sSerDTag(0x0008,0x103e);  // Series Description
//    const ::gdcm::Tag sUIDTag(0x0020,0x000d);   // Study Instance UID
//    const ::gdcm::Tag sIDTag(0x0020,0x0010);    // Study ID

    // Init
    ::fwData::Study::sptr   study   = this->getConcreteObject();
    const ::gdcm::DataSet & a_ds    = this->getDataSet();

    // Study's date
    study->setDate(helper::GdcmData::getTagValue<0x0008,0x0020>(a_ds));
    OSLM_TRACE("Study's date : "<<study->getDate());

    // Study's time
    study->setTime(helper::GdcmData::getTagValue<0x0008,0x0030>(a_ds));
    OSLM_TRACE("Study's time : "<<study->getTime());

    // Study's modality
    study->setModality(::fwTools::toStringWithoutAccent( helper::GdcmData::getTagValue<0x0008,0x0060>(a_ds) ));
    OSLM_TRACE("Study's modality : "<<study->getModality());

    // Study's hospital
    study->setHospital(::fwTools::toStringWithoutAccent( helper::GdcmData::getTagValue<0x0008,0x0080>(a_ds) )); // Institution Name
    OSLM_TRACE("Study's hospital : "<<study->getHospital());

    // Study's description
    const std::string studyDesc = helper::GdcmData::getTagValue<0x0008,0x1030>(a_ds);
    study->setDescription(studyDesc);
    OSLM_TRACE("Study's description : "<<study->getDescription());

    // Study's UID
    study->setUID(helper::GdcmData::getTagValue<0x0020,0x000d>(a_ds));
    OSLM_TRACE("Study's UID : "<<study->getUID());

    // Study's description
    study->setAcquisitionZone( ::fwTools::toStringWithoutAccent( studyDesc ));
    OSLM_TRACE("Study's zone : "<<study->getAcquisitionZone());
}

//------------------------------------------------------------------------------

void DicomStudyReader::read() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Study::sptr       study       = this->getConcreteObject();
    SLM_ASSERT("::fwData::Study not set", study);

    //*****     Get all file names      *****//
    std::vector< std::string > studyFiles   = this->getFileNames(); // files which contain a common study
    OSLM_TRACE("Number of files for a study : " << studyFiles.size());

    //*****     Get group of file names for each series      *****//
    const ::gdcm::Tag   gTagSUID(0x0020,0x000e); // Series UID
    const ::gdcm::Tag   gTagImageType(0x0008,0x0008);  // ImageType
    gdcm::Scanner       gScanner;
    gScanner.AddTag(gTagSUID);
    gScanner.AddTag(gTagImageType);

    if( !gScanner.Scan( studyFiles ) )
    {
        throw ::fwTools::Failed("No series found");
    }

    ::gdcm::Directory::FilenamesType                    keys    = gScanner.GetKeys();
    ::gdcm::Directory::FilenamesType::const_iterator    it      = keys.begin();
    ::gdcm::Directory::FilenamesType::const_iterator    itEnd   = keys.end();

    // Create a map to associate each series with its files.
    std::map< std::string, std::vector< std::string > > seriesMap;    // Key : series UID ; Value : filenames found
    int secondaryCaptureCounter = 0;

    for(; it != itEnd; ++it)
    {
        const char * filename   = it->c_str();
        const char * seriesUID   = gScanner.GetValue(filename, gTagSUID);

        if (seriesUID != 0)
        {
            std::string seriesKey(seriesUID);
            // Treatment of secondary capture dicom file.
            if(gScanner.GetValue(filename, gTagImageType))
            {
                std::string imageType(gScanner.GetValue(filename, gTagImageType));
                std::string::size_type idx  = imageType.find("DERIVED\\SECONDARY");

                if( idx != std::string::npos)
                {
                    std::string::size_type endIdx  = imageType.find_first_not_of("DERIVED\\SECONDARY");
                    std::string optionalInfo = imageType.substr(endIdx);
                    std::ostringstream indice;
                    if(!optionalInfo.empty())
                    {
                        indice << optionalInfo;
                    }
                    else
                    {
                        // Tag as Secondary Capture
                        indice << "_SC_" << secondaryCaptureCounter;
                        secondaryCaptureCounter++;
                    }
                    seriesKey += indice.str();
                }
            }
            seriesMap[seriesKey].push_back( filename );
        }
        else
        {
            OSLM_ERROR ( "No study UID found in : " << filename );
        }
    }

    //*****     Read each patient (and so studies, series, ...)      *****//
    DicomSeriesReader  seriesReader;
    std::map< std::string, std::vector< std::string > >::iterator itMap     = seriesMap.begin();
    std::map< std::string, std::vector< std::string > >::iterator itMapEnd  = seriesMap.end();
    while (itMap != itMapEnd)
    {
        OSLM_TRACE ( "Series map key : " << itMap->first );
        if ( itMap->second.size() > 0 )
        {
            OSLM_TRACE ( "First series map value : " << *(itMap->second.begin()) );

            seriesReader.setFileNames(itMap->second);

            ::fwData::Acquisition::NewSptr series;
            seriesReader.setObject(series);
            try
            {
                // Read one series
                seriesReader.read();

                // Add a complete series
                study->addAcquisition(series);
            }
            catch (::fwTools::Failed & e)
            {
                OSLM_ERROR ("Reading error with series : " << itMap->first);
                // Acquisition skipped
            }
        }
        itMap++;
    }

    if(study->getNumberOfAcquisitions() == 0)
        throw ::fwTools::Failed("Study has no acquisition");

    //*****     Read study      *****//
    this->setReader( seriesReader.getReader() );
    this->readStudy();
}

}  // namespace reader

}  // namespace gdcmIO

