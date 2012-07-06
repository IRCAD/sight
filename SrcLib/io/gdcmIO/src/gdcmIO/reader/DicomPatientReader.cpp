/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/trim.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/fromIsoExtendedString.hpp>

#include <gdcmDirectory.h>
#include <gdcmScanner.h>
#include <gdcmAttribute.h>

#include "gdcmIO/reader/DicomPatientReader.hpp"
#include "gdcmIO/reader/DicomStudyReader.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{

namespace reader
{

//------------------------------------------------------------------------------

DicomPatientReader::DicomPatientReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomPatientReader::~DicomPatientReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientReader::readPatient()
{
    SLM_TRACE_FUNC();

    // List of tag use in this method
//    const ::gdcm::Tag pNameTag(0x0010,0x0010);  // Patient's name
//    const ::gdcm::Tag pIdTag(0x0010,0x0020);    // Patient's ID
//    const ::gdcm::Tag pBdTag(0x0010,0x0030);    // Patient's birth date
//    const ::gdcm::Tag pSexTag(0x0010,0x0040);   // Patient's sex

    ::fwData::Patient::sptr patient = this->getConcreteObject();

    const ::gdcm::DataSet & gDsRoot = this->getDataSet();

    // Patient's name & first name
    {
        // Patient's Name
        std::string dicomName = ::fwTools::toStringWithoutAccent( helper::GdcmData::getTagValue<0x0010,0x0010>(gDsRoot) );
        std::string name = "", firstname = "";

        std::string::size_type sizeBase     = std::string::npos;
        std::string::size_type sizeIndex    = dicomName.find('^'); // name and first name are separated by '^'
        if (sizeIndex == sizeBase)
        {// If no separator
            name        = dicomName;
            firstname   = "";
            assert( name.length() == dicomName.length() );
        }
        else
        {
            name        = dicomName.substr(0, sizeIndex);
            firstname   = dicomName.substr(sizeIndex+1, dicomName.size());
            assert( name.length() + firstname.length() + 1 == dicomName.length() );
        }

        patient->setFirstname(firstname);
        patient->setName(name);
        OSLM_TRACE("Patient's name & first name : "<<name<<" "<<firstname);
    }

    // Patient's ID
    std::string patientID = helper::GdcmData::getTagValue<0x0010,0x0020>(gDsRoot);
    ::boost::algorithm::trim(patientID);    // Delete binary space padding
    patient->setIDDicom( ::fwTools::toStringWithoutAccent(patientID) );
    OSLM_TRACE("Patient's ID : "<<patient->getIDDicom());

    // Patient's birth date
    patient->setBirthdate(::fwTools::strToBoostDateAndTime( helper::GdcmData::getTagValue<0x0010,0x0030>(gDsRoot) ) );
    OSLM_TRACE("Patient's birth date : "<<patient->getBirthdate());

    // Patient's sex
    patient->setIsMale( (helper::GdcmData::getTagValue<0x0010,0x0040>(gDsRoot)[0] == 'F')?false:true);
    OSLM_TRACE("Patient is male : "<<patient->getIsMale());
}

//------------------------------------------------------------------------------

void DicomPatientReader::read() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Patient::sptr     patient         = this->getConcreteObject();
    SLM_ASSERT("::fwData::Patient not set", patient);

    //*****     Get all file names      *****//
    std::vector< std::string >  patientFiles    = this->getFileNames(); // files which contain a common patient
    OSLM_TRACE("Number of files for a patient : " << patientFiles.size());

    //*****     Get group of file names for each study      *****//
    const ::gdcm::Tag   gTagSUID(0x0020,0x000d); // Study UID
    gdcm::Scanner       gScanner;
    gScanner.AddTag(gTagSUID);

    // Scan patient files
    if( !gScanner.Scan( patientFiles ) )
    {
        throw ::fwTools::Failed("No study found");
    }

    ::gdcm::Directory::FilenamesType                    keys    = gScanner.GetKeys();
    ::gdcm::Directory::FilenamesType::const_iterator    it      = keys.begin();
    ::gdcm::Directory::FilenamesType::const_iterator    itEnd   = keys.end();

    // Create a map to associate each study with its files.
    std::map< std::string, std::vector< std::string > > studyMap;    // Key : studyUID ; Value : filenames found
    for(; it != itEnd; ++it)
    {
        const char * filename   = it->c_str();
        const char * studyUID   = gScanner.GetValue(filename, gTagSUID);
        if (studyUID != 0)
        {
            studyMap[studyUID].push_back( filename );
        }
        else
        {
            OSLM_ERROR ( "No study UID found in : " << filename );
        }
    }

    //*****     Read and add each study (and so series, ...)      *****//
    DicomStudyReader        studyReader;
    std::map< std::string, std::vector< std::string > >::iterator itMap     = studyMap.begin();
    std::map< std::string, std::vector< std::string > >::iterator itMapEnd  = studyMap.end();
    while (itMap != itMapEnd)
    {
        OSLM_TRACE ( "Study map key : " << itMap->first );
        if ( itMap->second.size() > 0 )
        {
            OSLM_TRACE ( "First study map value : " << *(itMap->second.begin()) );

            studyReader.setFileNames(itMap->second);

            ::fwData::Study::NewSptr study;
            studyReader.setObject(study);
            try
            {
                // Read one study
                studyReader.read();

                // Add a complete study
                patient->addStudy(study);
            }
            catch (::fwTools::Failed & e)
            {
                OSLM_ERROR ("Read error with study : " << itMap->first);
                // Study skipped
            }
        }
        itMap++;
    }

    if(patient->getNumberOfStudies() == 0)
        throw ::fwTools::Failed("Patient has no study");

    //*****     Read patient      *****//
    this->setReader( studyReader.getReader() ); // Get reader instantiated by DicomImageReader
    this->readPatient();
}

}  // namespace reader

}  // namespace gdcmIO

