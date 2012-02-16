/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <stdio.h>     /* for printf */
#include <stdint.h>
#include <stdlib.h>    /* for exit */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sstream>
#include <iterator>
#include <exception>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Factory.hpp>
#include <fwTools/dateAndTime.hpp>
#include <fwTools/fromIsoExtendedString.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Image.hpp>

#include <vtkImageWriter.h>

#include <vtkGDCMImageReader.h>
#include <vtkImageData.h>
#include <vtkStringArray.h>
#include <vtkImageChangeInformation.h>
#include <vtkMedicalImageProperties.h>
#include <vtkImageMapToColors.h>
#include <vtkSmartPointer.h>

#include <gdcmImageHelper.h>
#include <gdcmIPPSorter.h>
#include <gdcmFilename.h>
#include <gdcmTesting.h>
#include <gdcmSystem.h>
#include <gdcmTrace.h>
#include <gdcmDirectory.h>
#include <gdcmScanner.h>
#include <gdcmReader.h>
#include <gdcmIPPSorter.h>
#include <gdcmFilenameGenerator.h>
#include <gdcmAttribute.h>
#include <gdcmFile.h>

#include <vtkIO/vtk.hpp>
#include <vtkIO/helper/ProgressVtkToFw.hpp>

#include "vtkGdcmIO/DicomPatientDBReader.hpp"
#include "vtkGdcmIO/helper/GdcmHelper.hpp"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif


// Sort image using Instance Number:
bool mysort(gdcm::DataSet const & ds1, gdcm::DataSet const & ds2 )
{
    gdcm::Attribute<0x0020,0x0013> at1; // Instance Number
    at1.Set( ds1 );
    gdcm::Attribute<0x0020,0x0013> at2;
    at2.Set( ds2 );
    return at1 < at2;
}

namespace vtkGdcmIO
{

//------------------------------------------------------------------------------

DicomPatientDBReader::DicomPatientDBReader() :
    ::fwData::location::enableFolder< IObjectReader >(this),
    ::fwData::location::enableMultiFiles< IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomPatientDBReader::~DicomPatientDBReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr DicomPatientDBReader::createPatientDB( const ::boost::filesystem::path dicomDir )
{
    SLM_TRACE_FUNC();
    ::fwData::PatientDB::sptr patientDB = this->getConcreteObject();

    std::vector<std::string> filenames;
    ::vtkGdcmIO::helper::DicomSearch::searchRecursivelyFiles(dicomDir, filenames);

    this->addPatients( patientDB , filenames);
    return patientDB;
}

//------------------------------------------------------------------------------

void DicomPatientDBReader::addPatients( ::fwData::PatientDB::sptr patientDB, std::vector<std::string> filenames)
{
    //gdcm::Trace::SetDebug( 1 );
    //gdcm::Trace::SetWarning( 1 );
    //gdcm::Trace::SetError( 1 );

    gdcm::Scanner scanner;
    const gdcm::Tag t1(0x0020,0x000d); // Study Instance UID
    const gdcm::Tag t2(0x0020,0x000e); // Series Instance UID
    const gdcm::Tag t3(0x0020,0x0011); // Series Number
    const gdcm::Tag t4(0x0020,0x0032); // Image Position (Patient)
    const gdcm::Tag t5(0x0020,0x0037); //
    const gdcm::Tag t6(0x0020,0x0052); //
    const gdcm::Tag t7(0x0018,0x0024); // Sequence Name
    const gdcm::Tag t8(0x0018,0x0050); // Slice Thickness
    const gdcm::Tag t9(0x0028,0x0010); // Rows
    const gdcm::Tag t10(0x0028,0x0011); // Columns
    const gdcm::Tag t11(0x0008,0x0021); //
    const gdcm::Tag t12(0x0008,0x0030); // Study Time
    const gdcm::Tag t13(0x0008,0x0031); // Series Time
    const gdcm::Tag t14(0x0008,0x0032); // Acquisition Time

    const gdcm::Tag imageTypeTag(0x0008,0x0008); // ImageType
    const gdcm::Tag bitsAllocatedTag(0x0028,0x0100); // BitsAllocated

    scanner.AddTag( t1 );
    scanner.AddTag( t2 );
    scanner.AddTag( t3 );
    scanner.AddTag( t4 );
    scanner.AddTag( t5 );
    scanner.AddTag( t6 );
    scanner.AddTag( t7 );
    scanner.AddTag( t8 );
    scanner.AddTag( t9 );
    scanner.AddTag( t10 );
    scanner.AddTag( t11 );
    scanner.AddTag( t12 );
    scanner.AddTag( t13 );
    scanner.AddTag( t14 );    
    scanner.AddTag(imageTypeTag);
    scanner.AddTag(bitsAllocatedTag);

    //const gdcm::Tag &reftag = t2;

    ::fwData::Patient::sptr patient;
    ::fwData::Study::sptr study;

    try
    {
        std::string seriesInstanceUID("");
        std::string studyInstanceUID("");
        bool b = scanner.Scan( filenames );
        if( !b )
        {
            SLM_ERROR("Scanner failed");
            return ;
        }
        gdcm::Directory::FilenamesType keys = scanner.GetKeys();
        gdcm::Directory::FilenamesType::const_iterator it = keys.begin();

        std::map< std::string, std::vector< std::string > > mapSeries;
        int secondaryCaptureCounter = 0;

        for(; it != keys.end() /*&& i < 2*/; ++it)
        {
            const char *filename = it->c_str();
            assert( scanner.IsKey( filename ) );

            const char *value1 =  scanner.GetValue( filename, t2);
            const char *value2 =  scanner.GetValue( filename, t14);
            if (value1)
            {
                std::string stdValue = value1;
                if (value2)
                {
                    stdValue += value2;
                }

                if(scanner.GetValue(filename, imageTypeTag))
                {
                    // Treatment of secondary capture dicom file.
                    std::string imageType(scanner.GetValue(filename, imageTypeTag));
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
                        stdValue += indice.str();
                    }
                }
                mapSeries[stdValue.c_str()].push_back(filename);

            }
            else
            {
                OSLM_ERROR ( "Error in vtkGdcmIO : No serie name found in : " << filename );
            }

        }

        std::map< std::string, std::vector< std::string > >::iterator iter = mapSeries.begin();
        std::map< std::string, std::vector< std::string > >::iterator iterEnd = mapSeries.end();

        while (iter != iterEnd)
        {
            OSLM_TRACE ( " first : " << iter->first );
            if ( iter->second.size() != 0 )
            {
                OSLM_TRACE ( " second : " << *(iter->second.begin()) );
                vtkSmartPointer< vtkStringArray > files = vtkSmartPointer< vtkStringArray >::New();
                vtkSmartPointer< vtkGDCMImageReader > reader = vtkSmartPointer< vtkGDCMImageReader >::New();
                reader->FileLowerLeftOn();
                gdcm::IPPSorter s;
                s.SetComputeZSpacing( true );
                s.SetZSpacingTolerance( 1e-3 );
                b = s.Sort( iter->second );
                double zspacing = 0;
                int nbSorter = 0;
                files->Initialize();

                std::vector< std::string > listOfFiles = iter->second;
                int nb = listOfFiles.size();
                if( !b )
                {
                    SLM_WARN ( "Failed to sort:" );
                    std::vector< std::string >::const_iterator it = iter->second.begin();
                    for( ; it != iter->second.end(); ++it)
                    {
                        const std::string &f = *it;
                        files->InsertNextValue( f.c_str() );
                    }
                }
                else
                {
                    SLM_TRACE ( "Success to sort" );
                    zspacing = s.GetZSpacing();
                    if (!zspacing && s.GetFilenames().size() > 1)
                    {
                        SLM_TRACE ( "New sort (more soft)" );
                        const std::vector<std::string> & sorted = s.GetFilenames();
                        if (!sorted.empty())
                        {
                            gdcm::Reader localReader1;
                            gdcm::Reader localReader2;
                            const std::string &f1 = *(sorted.begin());
                            const std::string &f2 = *(sorted.begin() + 1);
                            OSLM_TRACE ( "Search spacing in : " << f1.c_str());
                            OSLM_TRACE ( "Search spacing in : " << f2.c_str());

                            localReader1.SetFileName( f1.c_str() );
                            localReader2.SetFileName( f2.c_str() );
                            if( localReader1.Read() && localReader2.Read() )
                            {
                                std::vector<double> vOrigin1 = gdcm::ImageHelper::GetOriginValue(localReader1.GetFile());
                                std::vector<double> vOrigin2 = gdcm::ImageHelper::GetOriginValue(localReader2.GetFile());
                                zspacing = vOrigin2[2] - vOrigin1[2];
                                OSLM_TRACE ( "Found z-spacing:" << zspacing << " from : << " << vOrigin2[2] << " | " << vOrigin1[2]);
                            }
                            else
                            {
                                OSLM_ERROR ( "Cannot read :" << f1 << " or : " << f2 );
                            }
                        }

                        if (!zspacing)
                        {
                            OSLM_DEBUG ( "Failed to find z-spacing:" << s.GetZSpacing());
                        }
                        else
                        {
                            nbSorter = 2;
                            OSLM_DEBUG ( "Re-Sort and fix z-spacing:" << s.GetZSpacing());
                        }
                    }
                    else
                    {
                        nbSorter = 1;
                        OSLM_TRACE ( "Found z-spacing:" << s.GetZSpacing());
                    }
                    const std::vector<std::string> & sorted = s.GetFilenames(); //FIXME
                    std::vector< std::string >::const_iterator it = sorted.begin();
                    for( ; it != sorted.end(); ++it)
                    {
                        const std::string &f = *it;
                        files->InsertNextValue( f.c_str() );
                    }
                }

                // Get the serie instance UID.
                const std::vector<std::string> & filesList = iter->second;
                seriesInstanceUID="";
                studyInstanceUID="";
                if(!filesList.empty())
                {
                    const std::string &file = *(filesList.begin());
                    seriesInstanceUID.assign(scanner.GetValue( file.c_str(), t2));
                    studyInstanceUID.assign(scanner.GetValue( file.c_str(), t1));
                }
                ::fwData::Image::NewSptr pDataImage;
                bool res = false;
                if (files->GetNumberOfValues() > 0)
                {
                    try
                    {
                        SLM_TRACE ( "Read all files" );
                        reader->SetFileNames( files );
                        // Memory management
                        // Clean memory if possible
                        // Reserve 1 image (to allow a copy) 512 * 512 * size Z * sizeof(signed short)
                        //bool bMem = xxx->reserveMemory( 512 * 512 * files->GetNumberOfValues() * 2 );
                        bool bMem = true;
                        if ( bMem )
                        {
                            //add progress observation
                            ::vtkIO::Progressor progress(reader, this->getSptr(), "Serie " + iter->first);
                            reader->Update();
                            try
                            {
                                ::vtkIO::fromVTKImage(reader->GetOutput(), pDataImage);
                                res = true;
                            }
                            catch(std::exception &e)
                            {
                                OSLM_ERROR("VTKImage to fwData::Image failed "<<e.what());
                            }
                        }
                    }
                    catch (std::exception &e)
                    {
                        OSLM_ERROR ( "Error during conversion : " << e.what() );
                    }
                    catch (...)
                    {
                        OSLM_ERROR ( "Error during conversion" );
                    }
                }

                if (res)
                {
                    // Read medical info
                    vtkMedicalImageProperties * medprop = reader->GetMedicalImageProperties();
                    std::string nameStr = medprop->GetPatientName(); //"0010|0010"
                    std::string sexStr = medprop->GetPatientSex(); //"0010|0040"
                    bool sex = (sexStr[0] == 'F')?false:true;
                    std::string modality    = medprop->GetModality(); //"0008|0060"
//                    std::string zone = (std::string(medprop->GetStudyDescription()) + " - ") + medprop->GetSeriesDescription(); //"0008|1030"
                    std::string serieDescription = medprop->GetSeriesDescription();
                    std::string zone = std::string(medprop->GetStudyDescription()) ; //"0008|1030"
                    std::string studyID = std::string(medprop->GetStudyID()); // "0020,0010"
                    std::string studyTime =  std::string(medprop->GetStudyTime());
                    std::string studyDate = std::string(medprop->GetStudyDate());
                    std::string studyDescription = std::string(medprop->GetStudyDescription());
                    std::string patientID  = medprop->GetPatientID();//"0010|0020"
                    ::boost::algorithm::trim(patientID);
                    std::string birthdateStr= medprop->GetPatientBirthDate(); //"0010|0030"
                    ::boost::posix_time::ptime birthdate = ::fwTools::strToBoostDateAndTime(birthdateStr);
                    std::string hospital = medprop->GetInstitutionName(); //"0008|0080"

                    std::string acqDateStr  = medprop->GetAcquisitionDate(); //"0008|0022"
                    std::string acqTimeStr  = medprop->GetAcquisitionTime(); //"0008|0032"
                    ::boost::posix_time::ptime acqDate = ::fwTools::strToBoostDateAndTime(acqDateStr,acqTimeStr);

                    //std::string spacing             = "0028|0030"
                    double thickness = medprop->GetSliceThicknessAsDouble();//"0018|0050"
                    //std::string interSliceStr= "0018|0088"
                    //std::vector<double> vRescale = gdcm::ImageHelper::GetRescaleInterceptSlopeValue(localReader.GetFile());
                    double rescale=0.0;//(vRescale.size()>0)?vRescale[0]:0.0;
                    double center=0.0;
                    double width=0.0;
                    if (medprop->GetNumberOfWindowLevelPresets())//FIXME : Multiple preset !!!
                        medprop->GetNthWindowLevelPreset(0,&width,&center); //0028|1050,1051

                    // remove accent
                    nameStr = ::fwTools::toStringWithoutAccent(nameStr);
                    modality = ::fwTools::toStringWithoutAccent(modality);
                    zone = ::fwTools::toStringWithoutAccent(zone);
                    serieDescription = ::fwTools::toStringWithoutAccent(serieDescription);
                    patientID = ::fwTools::toStringWithoutAccent(patientID);
                    hospital = ::fwTools::toStringWithoutAccent(hospital);

                    ::fwData::Acquisition::NewSptr acq;
                    // Check the existence of the the study and the patient.
                    bool bIsNewStudy =false;
                    bool bIsNewPatient =false;
                    if(patientDB->getPatientSize() !=0 )
                    {
                        //Looking for an existing patient.
                        ::fwData::PatientDB::PatientIterator itrOnPatient;
                        std::pair< ::fwData::PatientDB::PatientIterator, ::fwData::PatientDB::PatientIterator > patients = patientDB->getPatients();
                        for(itrOnPatient = patients.first; itrOnPatient != patients.second; ++itrOnPatient)
                        {
                            std::string id = (*itrOnPatient)->getIDDicom();
                            if (patientID == id)
                            {
                                patient =*itrOnPatient;
                                std::pair< ::fwData::Patient::StudyIterator, ::fwData::Patient::StudyIterator > studies = patient->getStudies();
                                // Looking for an existing study
                                ::fwData::Patient::StudyIterator itrOnStudy;
                                for(itrOnStudy = studies.first; itrOnStudy != studies.second; ++itrOnStudy)
                                {
                                    if((*itrOnStudy)->getUID() == studyInstanceUID)
                                    {
                                        study = *itrOnStudy;
                                        break;
                                    }
                                    else
                                    {
                                        study = ::fwData::Study::New();
                                        bIsNewStudy = true;
                                    }
                                }
                                break;
                            }
                            else
                            {
                                bIsNewPatient = true;
                                patient = ::fwData::Patient::New();
                                study = ::fwData::Study::New();
                            }
                        }
                    }
                    else
                    {
                        bIsNewStudy = true;
                        bIsNewPatient = true;
                        patient = ::fwData::Patient::New();
                        study = ::fwData::Study::New();
                    }

                    ::fwData::Image::SpacingType vPixelSpacing = pDataImage->getSpacing();
                    if (zspacing > 0)
                    {
                        vPixelSpacing[2] = zspacing;
                    }
                    pDataImage->setSpacing(vPixelSpacing);

                    ::fwData::Image::SizeType imgSize = pDataImage->getSize();
                    if (imgSize[2] == 0.0)
                    {
                        imgSize[2] = 1.0;
                        width = 4096;
                    }


                    // Name & firstname
                    std::string name = "";
                    std::string firstname = "";
                    this->extractIdentity(nameStr, name, firstname);

                    // Set field
                    pDataImage->setWindowCenter(center);
                    pDataImage->setWindowWidth(width);
                    // Not managed by fwData::Image new API
                    //  pDataImage->setRescaleIntercept(/*rescale*/0.0);

                    acq->setUID(seriesInstanceUID);
                    acq->setCRefCreationDate(acqDate);
                    acq->setDescription(serieDescription);
                    acq->setSliceThickness(thickness);
                    // Keep the path and file name fo the Dicom file associated with acquisition.
                    std::vector< std::string >::const_iterator itrOnfiles = iter->second.begin();
                    for( ; itrOnfiles != iter->second.end(); ++itrOnfiles)
                    {
                        acq->addDicomFileUrl(*itrOnfiles);
                    }
                    //acq->setCRefImageType(imageTypeStr);
                    if (bIsNewStudy)
                    {
                        study->setCRefModality(modality);
                        study->setCRefHospital(hospital);
                        study->setCRefAcquisitionZone(zone);
                        study->setUID(studyInstanceUID);
                        study->setTime(studyTime);
                        study->setDate(studyDate);
                        study->setDescription(studyDescription);
                    }
                    if(bIsNewPatient)
                    {

                        patient->setCRefFirstname(firstname);
                        patient->setCRefName(name);
                        patient->setCRefIDDicom(patientID);
                        patient->setCRefBirthdate(birthdate);
                        patient->setCRefIsMale(sex);
                    }                    //--

                    acq->setImage(pDataImage);
                    study->addAcquisition(acq);
                    if (bIsNewStudy)
                    {
                        patient->addStudy(study);
                    }
                    if(bIsNewPatient)
                    {
                        patientDB->addPatient( patient );
                    }
                } // if res == true
            } // if nb files > 0
            iter++;
        } // While all data
    } // try
    catch (std::exception& e)
    {
        OSLM_ERROR ( "Try with another reader or retry with this reader on a specific subfolder : " << e.what() );
        std::vector< std::string >::const_iterator it = filenames.begin();
        for( ; it != filenames.end(); ++it)
        {
            OSLM_ERROR ("file error : " << *it );
        }
    }
}

//------------------------------------------------------------------------------

void DicomPatientDBReader::extractIdentity(const std::string patientName, std::string& name, std::string& firstname)
{
    name = "";
    firstname = "";
    std::string::size_type sizeBase = std::string::npos;
    std::string::size_type sizeIndex = patientName.find('^');
    if (sizeIndex == sizeBase)
    {
        name = patientName;
        firstname = "";
        assert( name.length() == patientName.length() );
    }
    else
    {
        name = patientName.substr(0, sizeIndex);
        firstname = patientName.substr(sizeIndex+1, patientName.size());
        assert( name.length() + firstname.length() + 1 == patientName.length() );
    }
}

//------------------------------------------------------------------------------

void DicomPatientDBReader::read()
{
    SLM_TRACE_FUNC();
    ::fwData::PatientDB::sptr patientDB = this->getConcreteObject();
    std::vector<std::string> filenames;
    if(::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this))
    {
        ::vtkGdcmIO::helper::DicomSearch::searchRecursivelyFiles(this->getFolder(), filenames);
    }
    else if(::fwData::location::have < ::fwData::location::MultiFiles, ::fwDataIO::reader::IObjectReader > (this))
    {
        BOOST_FOREACH(::boost::filesystem::path file, this->getFiles())
        {
            filenames.push_back(file.string());
        }
    }
    this->addPatients( patientDB , filenames);
}

} //namespace vtkGdcmIO


