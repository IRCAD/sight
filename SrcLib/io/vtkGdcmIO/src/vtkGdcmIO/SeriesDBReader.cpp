/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "vtkGdcmIO/SeriesDBReader.hpp"
#include "vtkGdcmIO/helper/GdcmHelper.hpp"


#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/fromIsoExtendedString.hpp>

#include <fwVtkIO/vtk.hpp>
#include <fwVtkIO/helper/vtkLambdaCommand.hpp>

#include <vtkImageWriter.h>
#include <vtkGDCMImageReader.h>
#include <vtkImageData.h>
#include <vtkStringArray.h>
#include <vtkMedicalImageProperties.h>
#include <vtkSmartPointer.h>

#include <gdcmImageHelper.h>
#include <gdcmIPPSorter.h>
#include <gdcmScanner.h>
#include <gdcmReader.h>
#include <gdcmIPPSorter.h>


#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <exception>

fwDataIOReaderRegisterMacro( ::vtkGdcmIO::SeriesDBReader );

namespace vtkGdcmIO
{

//------------------------------------------------------------------------------

SeriesDBReader::SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableFolder< IObjectReader >(this),
    ::fwData::location::enableMultiFiles< IObjectReader >(this),
    m_job(::fwJobs::Observer::New("SeriesDB reader"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

SeriesDBReader::~SeriesDBReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr SeriesDBReader::createSeriesDB( const ::boost::filesystem::path &dicomDir )
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();

    std::vector<std::string> filenames;
    ::vtkGdcmIO::helper::DicomSearch::searchRecursivelyFiles(dicomDir, filenames);

    this->addSeries( seriesDB, filenames);
    return seriesDB;
}

//------------------------------------------------------------------------------

void SeriesDBReader::addSeries( const ::fwMedData::SeriesDB::sptr &seriesDB,
                                const std::vector< std::string > &filenames)
{
    //gdcm::Trace::SetDebug( 1 );
    //gdcm::Trace::SetWarning( 1 );
    //gdcm::Trace::SetError( 1 );

    gdcm::Scanner scanner;
    const gdcm::Tag seriesUIDTag(0x0020,0x000e);
    const gdcm::Tag seriesDateTag(0x0008,0x0021);
    const gdcm::Tag seriesTimeTag(0x0008,0x0031);
    const gdcm::Tag seriesTypeTag(0x0008,0x0060);
    const gdcm::Tag seriesDescriptionTag(0x0008,0x103e);
    const gdcm::Tag seriesPhysicianNamesTag(0x0008,0x1050);

    const gdcm::Tag equipmentInstitutionNameTag(0x0008,0x0080);

    const gdcm::Tag patientNameTag(0x0010,0x0010);
    const gdcm::Tag patientIDTag(0x0010,0x0020);
    const gdcm::Tag patientBirthdateTag(0x0010,0x0030);
    const gdcm::Tag patientSexTag(0x0010,0x0040);
    const gdcm::Tag studyUIDTag(0x0020,0x000d);
    const gdcm::Tag studyDateTag(0x0008,0x0020);
    const gdcm::Tag studyTimeTag(0x0008,0x0030);
    const gdcm::Tag studyReferingPhysicianNameTag(0x0008,0x0090);
    const gdcm::Tag studyDescriptionTag(0x0008,0x1030);
    const gdcm::Tag studyPatientAgeTag(0x0010,0x1010);

    const gdcm::Tag imageTypeTag(0x0008,0x0008);
    const gdcm::Tag acquisitionDateTag(0x0008,0x0022);
    const gdcm::Tag acquisitionTimeTag(0x0008,0x0032);

    scanner.AddTag( seriesUIDTag );
    scanner.AddTag( seriesDateTag );
    scanner.AddTag( seriesTimeTag );
    scanner.AddTag( seriesTypeTag );
    scanner.AddTag( seriesDescriptionTag );
    scanner.AddTag( seriesPhysicianNamesTag );
    scanner.AddTag( equipmentInstitutionNameTag );
    scanner.AddTag( studyUIDTag );
    scanner.AddTag( patientNameTag );
    scanner.AddTag( patientIDTag );
    scanner.AddTag( patientBirthdateTag );
    scanner.AddTag( patientSexTag );
    scanner.AddTag( studyUIDTag );
    scanner.AddTag( studyDateTag );
    scanner.AddTag( studyTimeTag );
    scanner.AddTag( studyReferingPhysicianNameTag );
    scanner.AddTag( studyDescriptionTag );
    scanner.AddTag( studyPatientAgeTag );
    scanner.AddTag( imageTypeTag );
    scanner.AddTag( acquisitionDateTag );
    scanner.AddTag( acquisitionTimeTag );


    try
    {
        bool b = scanner.Scan( filenames );
        if( !b )
        {
            SLM_ERROR("Scanner failed");
            return;
        }
        gdcm::Directory::FilenamesType keys               = scanner.GetKeys();
        gdcm::Directory::FilenamesType::const_iterator it = keys.begin();

        typedef std::map< std::string, std::vector< std::string > > MapSeriesType;
        MapSeriesType mapSeries;

        for(; it != keys.end(); ++it)
        {
            const char *filename = it->c_str();
            assert( scanner.IsKey( filename ) );

            const char *seriesUID = scanner.GetValue( filename, seriesUIDTag );
            const char *acqDate   = scanner.GetValue( filename, acquisitionDateTag );

            if (seriesUID)
            {
                std::string fileSetId = seriesUID;

                if (acqDate)
                {
                    fileSetId += "_";
                    fileSetId += acqDate;
                }

                const char *imageTypeStr = scanner.GetValue(filename, imageTypeTag);
                if(imageTypeStr)
                {
                    // Treatment of secondary capture dicom file.
                    std::string imageType(imageTypeStr);
                    SLM_TRACE("Image Type : " + imageType);

                    fileSetId += "_";
                    fileSetId += imageTypeStr;
                }
                mapSeries[fileSetId].push_back(filename);

            }
            else
            {
                OSLM_ERROR ( "Error in vtkGdcmIO : No serie name found in : " << filename );
            }

        }

        MapSeriesType::const_iterator iter    = mapSeries.begin();
        MapSeriesType::const_iterator iterEnd = mapSeries.end();

        while (iter != iterEnd)
        {
            std::string seriesInstanceUID;
            std::string studyInstanceUID;

            ::fwMedData::ImageSeries::sptr series  = ::fwMedData::ImageSeries::New();
            ::fwMedData::Patient::sptr patient     = series->getPatient();
            ::fwMedData::Study::sptr study         = series->getStudy();
            ::fwMedData::Equipment::sptr equipment = series->getEquipment();

            seriesDB->getContainer().push_back(series);

            OSLM_TRACE ( " Processing : " << iter->first << " file set.");
            const MapSeriesType::mapped_type &files = iter->second;
            if ( !files.empty() )
            {
                vtkSmartPointer< vtkStringArray > fileArray  = vtkSmartPointer< vtkStringArray >::New();
                vtkSmartPointer< vtkGDCMImageReader > reader = vtkSmartPointer< vtkGDCMImageReader >::New();
                reader->FileLowerLeftOn();
                gdcm::IPPSorter s;
                s.SetComputeZSpacing( true );
                s.SetZSpacingTolerance( 1e-3 );
                b = s.Sort( files );
                double zspacing = 0;
                // int nbSorter = 0;
                fileArray->Initialize();

                MapSeriesType::mapped_type::const_iterator filesIt  = files.begin();
                MapSeriesType::mapped_type::const_iterator filesEnd = files.end();

                OSLM_WARN_IF( "Failed to sort:" << iter->first, !b );
                OSLM_TRACE  ( "Found z-spacing:" << s.GetZSpacing());

                if(b)
                {
                    const std::vector<std::string> & sorted = s.GetFilenames();
                    OSLM_TRACE ( "Success to sort" << iter->first );
                    zspacing = s.GetZSpacing();
                    if (!zspacing && s.GetFilenames().size() > 1)
                    {
                        SLM_TRACE ( "Guessing zspacing ..." );
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
                            bool canRead = localReader1.Read() && localReader2.Read();
                            if( canRead )
                            {
                                std::vector<double> vOrigin1 =
                                    gdcm::ImageHelper::GetOriginValue(localReader1.GetFile());
                                std::vector<double> vOrigin2 =
                                    gdcm::ImageHelper::GetOriginValue(localReader2.GetFile());
                                zspacing = vOrigin2[2] - vOrigin1[2];
                                OSLM_TRACE (
                                    "Found z-spacing:" << zspacing << " from : << " << vOrigin2[2] << " | " <<
                                    vOrigin1[2]);
                            }

                            OSLM_ERROR_IF( "Cannot read :" << f1 << " or : " << f2, !canRead );
                        }
                        OSLM_DEBUG_IF ( "Failed to find z-spacing:" << s.GetZSpacing(), !zspacing);
                        OSLM_DEBUG_IF ( "Guessed z-spacing:" << s.GetZSpacing() << " -> " << zspacing, zspacing);
                    }

                    filesIt  = sorted.begin();
                    filesEnd = sorted.end();
                }

                for(; filesIt != filesEnd; ++filesIt)
                {
                    const std::string &f = *filesIt;
                    SLM_TRACE("Add '" + f + "' to vtkGdcmReader");
                    fileArray->InsertNextValue( f.c_str() );
                }

                ::fwData::Image::sptr pDataImage = ::fwData::Image::New();

                bool res = false;
                if (fileArray->GetNumberOfValues() > 0)
                {
                    reader->SetFileNames( fileArray );
                    try
                    {
                        SLM_TRACE("Read Series: " + iter->first);

                        using namespace fwVtkIO::helper;
                        //add progress observation
                        vtkSmartPointer<vtkLambdaCommand> progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
                        progressCallback->SetCallback([this](vtkObject* caller, long unsigned int, void* )
                            {
                                auto filter = static_cast<vtkGDCMImageReader*>(caller);
                                m_job->doneWork( filter->GetProgress()*100 );
                            });
                        reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

                        m_job->addSimpleCancelHook( [&]()
                            {
                                reader->AbortExecuteOn();
                            } );

                        reader->Update();
                        reader->UpdateInformation();
                        reader->PropagateUpdateExtent();
                        try
                        {
                            ::fwVtkIO::fromVTKImage(reader->GetOutput(), pDataImage);
                            res = true;
                        }
                        catch(std::exception &e)
                        {
                            OSLM_ERROR("VTKImage to fwData::Image failed : "<<e.what());
                        }
                    }
                    catch (std::exception &e)
                    {
                        OSLM_ERROR ( "Error during conversion : " << e.what() );
                    }
                    catch (...)
                    {
                        OSLM_ERROR ( "Unexpected error during conversion" );
                    }
                    m_job->finish();
                }

                if (res)
                {

                    SLM_ASSERT("No file to read", !files.empty());

                    // Read medical info
                    vtkMedicalImageProperties * medprop = reader->GetMedicalImageProperties();

                    std::string patientName      = medprop->GetPatientName(); //"0010|0010"
                    std::string patientId        = medprop->GetPatientID();
                    std::string patientBirthdate = medprop->GetPatientBirthDate(); //"0010|0030"
                    std::string patientSex       = medprop->GetPatientSex(); //"0010|0040"

                    gdcm::Scanner::ValuesType gdcmPhysicianNames = scanner.GetValues( seriesPhysicianNamesTag );
                    const char * seriesUIDStr                    = scanner.GetValue( files[0].c_str(), seriesUIDTag );
                    const char * seriesTimeStr                   = scanner.GetValue( files[0].c_str(), seriesTimeTag );
                    const char * seriesDateStr                   = scanner.GetValue( files[0].c_str(), seriesDateTag );
                    std::string seriesModality                   = medprop->GetModality(); //"0008|0060"
                    std::string seriesDescription                = medprop->GetSeriesDescription();
                    std::string seriesDate                       = ( seriesDateStr ? seriesDateStr : "" );
                    std::string seriesTime                       = ( seriesTimeStr ? seriesTimeStr : "" );

                    ::fwMedData::DicomValuesType seriesPhysicianNames;
                    for(const std::string &str :  gdcmPhysicianNames)
                    {
                        ::fwMedData::DicomValuesType result;
                        ::boost::split( result, str, ::boost::is_any_of("\\"));
                        seriesPhysicianNames.reserve(seriesPhysicianNames.size() + result.size());
                        seriesPhysicianNames.insert(seriesPhysicianNames.end(), result.begin(), result.end());
                    }

                    const char * studyUIDStr = scanner.GetValue( files[0].c_str(), studyUIDTag );
                    const char * studyReferingPhysicianNameStr
                        = scanner.GetValue( files[0].c_str(), studyReferingPhysicianNameTag );
                    std::string studyDate        = medprop->GetStudyDate();
                    std::string studyTime        = medprop->GetStudyTime();
                    std::string studyDescription = medprop->GetStudyDescription();  //"0008|1030"
                    std::string studyPatientAge  = medprop->GetPatientAge();
                    std::string studyReferingPhysicianName
                        = ( studyReferingPhysicianNameStr ? studyReferingPhysicianNameStr : "" );

                    std::string equipementInstitution = medprop->GetInstitutionName(); //"0008|0080"


                    double thickness = medprop->GetSliceThicknessAsDouble();//"0018|0050"
                    double center    = 0.0;
                    double width     = 0.0;
                    if (medprop->GetNumberOfWindowLevelPresets())//FIXME : Multiple preset !!!
                    {
                        medprop->GetNthWindowLevelPreset(0,&width,&center); //0028|1050,1051
                    }

                    // Image must have 3 dimensions
                    if(pDataImage->getNumberOfDimensions()  == 2)
                    {
                        ::fwData::Image::SizeType imgSize = pDataImage->getSize();
                        imgSize.resize(3);
                        imgSize[2] = 1;
                        pDataImage->setSize(imgSize);

                        ::fwData::Image::OriginType imgOrigin = pDataImage->getOrigin();
                        imgOrigin.resize(3);
                        imgOrigin[2] = 0.;
                        pDataImage->setOrigin(imgOrigin);
                    }


                    ::fwData::Image::SpacingType vPixelSpacing = pDataImage->getSpacing();
                    vPixelSpacing.resize(3);
                    // assume z-spacing = 1 if not guessed
                    vPixelSpacing[2] = zspacing ? zspacing : (thickness ? thickness : 1.);
                    pDataImage->setSpacing(vPixelSpacing);
                    pDataImage->setWindowCenter(center);
                    pDataImage->setWindowWidth(width);

                    // Get the serie instance UID.
                    SLM_ASSERT("No series UID", seriesUIDStr);
                    series->setInstanceUID(( seriesUIDStr ? seriesUIDStr : "UNKNOWN-UID" ));
                    series->setModality( seriesModality );
                    series->setDescription( seriesDescription );
                    series->setDate( seriesDate );
                    series->setTime( seriesTime );
                    series->setPerformingPhysiciansName( seriesPhysicianNames );
                    series->setImage(pDataImage);

                    SLM_ASSERT("No study UID",  studyUIDStr);
                    study->setInstanceUID(( studyUIDStr ? studyUIDStr : "UNKNOWN-UID" ));
                    study->setDate(studyDate);
                    study->setTime(studyTime);
                    study->setDescription(studyDescription);
                    study->setPatientAge(studyPatientAge);
                    study->setReferringPhysicianName(studyReferingPhysicianName);

                    patient->setName(patientName);
                    patient->setPatientId(patientId);
                    patient->setBirthdate(patientBirthdate);
                    patient->setSex(patientSex);

                    equipment->setInstitutionName(equipementInstitution);

                } // if res
            } // if !files.empty()
            ++iter;
        } // while (iter != iterEnd)
    } // try
    catch (std::exception& e)
    {
        OSLM_ERROR ( "Try with another reader or retry with this reader on a specific subfolder : " << e.what() );
        std::vector< std::string >::const_iterator it = filenames.begin();
        for(; it != filenames.end(); ++it)
        {
            OSLM_ERROR ("file error : " << *it );
        }
    }
}

//------------------------------------------------------------------------------

void SeriesDBReader::read()
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    std::vector<std::string> filenames;
    if(::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this))
    {
        ::vtkGdcmIO::helper::DicomSearch::searchRecursivelyFiles(this->getFolder(), filenames);
    }
    else if(::fwData::location::have < ::fwData::location::MultiFiles, ::fwDataIO::reader::IObjectReader > (this))
    {
        for(::boost::filesystem::path file :  this->getFiles())
        {
            filenames.push_back(file.string());
        }
    }
    this->addSeries( seriesDB, filenames);
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr SeriesDBReader::getJob() const
{
    return m_job;
}

} //namespace vtkGdcmIO


