/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// GDCM
#include <gdcmDirectory.h>
#include <gdcmScanner.h>
#include <gdcmAttribute.h>

// fwTools
#include <fwTools/ClassRegistrar.hpp>

#include "gdcmIO/reader/DicomPatientDBReader.hpp"
#include "gdcmIO/reader/DicomPatientReader.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader, ::gdcmIO::reader::DicomPatientDBReader, ::gdcmIO::reader::DicomPatientDBReader );

namespace gdcmIO
{

namespace reader
{

//------------------------------------------------------------------------------

DicomPatientDBReader::DicomPatientDBReader() :
    ::fwData::location::enableFolder< IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomPatientDBReader::~DicomPatientDBReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientDBReader::read() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::PatientDB::sptr   patientDB   = this->getConcreteObject();
    SLM_ASSERT("::fwData::PatientDB not instanced", patientDB);

    //*****     Get all file names      *****//
    const std::string &         rootPath    = this->getFolder().string();
    ::gdcm::Directory           dir;
    const unsigned int          nbFiles     = dir.Load(rootPath, true); // Get all files from specified folder and its sub folders
    OSLM_TRACE("Number of files load : " << nbFiles);

    // TODO: (optional) Filter filenames with extension : .dcm, .dicom, nothing

    //*****     Get group of file names for each patient      *****//
    const ::gdcm::Tag gTagPN(0x0010,0x0010);    // Patient Name
    const ::gdcm::Tag gTagPBD(0x0010,0x0030);   // Patient Birth Date
//    const ::gdcm::Tag pIdTag(0x0010,0x0020);    // Patient ID             // Scan ID too (useful or not for anonymous patient)?
    gdcm::Scanner gScanner;
    gScanner.AddTag(gTagPN);
    gScanner.AddTag(gTagPBD);

    // Scan all files
    if( !gScanner.Scan( dir.GetFilenames() ) )
    {
        throw ::fwTools::Failed("No patient found");
    }

    //  Get all DICOM file names
    ::gdcm::Directory::FilenamesType                    keys    = gScanner.GetKeys();
    ::gdcm::Directory::FilenamesType::const_iterator    it      = keys.begin();
    ::gdcm::Directory::FilenamesType::const_iterator    itEnd   = keys.end();

    // Create a map to associate each patient with its files.
    std::map< std::string, std::vector< std::string > > patientMap;    // Key : patientName (+ patientBirthDate) ; Value : file name found
    std::string                                         mapKey;
    for(; it != itEnd; ++it)    // For each DICOM file
    {
        const char * filename           = it->c_str();
        const char * patientName        = gScanner.GetValue(filename, gTagPN);
        const char * patientBirthDate   = gScanner.GetValue(filename, gTagPBD);
        if (patientName != 0)
        {
            mapKey = patientName;
            if (patientBirthDate != 0)
            {
                mapKey += patientBirthDate;
            }
            patientMap[mapKey.c_str()].push_back( filename );
        }
        else
        {
            OSLM_ERROR ( "No patient name found in : " << filename );
        }
    }

    //*****     Read each patient (and so studies, series, ...)      *****//
    DicomPatientReader                                              patientReader;
    std::map< std::string, std::vector< std::string > >::iterator   itMap     = patientMap.begin();
    std::map< std::string, std::vector< std::string > >::iterator   itMapEnd  = patientMap.end();
    while (itMap != itMapEnd)
    {
        OSLM_TRACE ( "Patient map key : " << itMap->first );
        if ( itMap->second.size() > 0 )
        {
            OSLM_TRACE ( "First patient map value : " << *(itMap->second.begin()) );

            patientReader.setFileNames(itMap->second);

            ::fwData::Patient::NewSptr patient;
            patientReader.setObject(patient);
            try
            {
                // Read one patient
                patientReader.read();

                // Add a complete patient
                patientDB->addPatient(patient);
            }
            catch (::fwTools::Failed & e)
            {
                OSLM_ERROR ("Read error with patient : " << itMap->first);
            }
        }
        itMap++;
    }

    if (patientDB->getPatientSize() == 0)
    {
        std::stringstream ss;
        ss << "Read error : no DICOM file or does not contain image";
#ifndef __MACOSX__
        OSLM_ERROR(ss.str());
#else
        throw ::fwTools::Failed(ss.str());
#endif
    }
}

}  // namespace reader

}  // namespace gdcmIO

