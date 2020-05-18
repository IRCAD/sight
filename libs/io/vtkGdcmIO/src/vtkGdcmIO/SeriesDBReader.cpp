/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "vtkGdcmIO/SeriesDBReader.hpp"

#include "vtkGdcmIO/helper/GdcmHelper.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/fromIsoExtendedString.hpp>

#include <fwVtkIO/helper/vtkLambdaCommand.hpp>
#include <fwVtkIO/vtk.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmImageHelper.h>
#include <gdcmIPPSorter.h>
#include <gdcmReader.h>
#include <gdcmScanner.h>
#include <gdcmSorter.h>
#include <vtkGDCMImageReader.h>
#include <vtkImageData.h>
#include <vtkImageWriter.h>
#include <vtkMedicalImageProperties.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>

#include <exception>
#include <filesystem>

fwDataIOReaderRegisterMacro( ::vtkGdcmIO::SeriesDBReader )

namespace vtkGdcmIO
{

static const ::gdcm::Tag s_SERIES_MODALITY_TAG(0x0008, 0x0060);
static const ::gdcm::Tag s_SERIES_UID_TAG(0x0020, 0x000e);
static const ::gdcm::Tag s_SERIES_NUMBER_TAG(0x0020, 0x0011);
static const ::gdcm::Tag s_SERIES_LATERALITY_TAG(0x0020, 0x0060);
static const ::gdcm::Tag s_SERIES_DATE_TAG(0x0008, 0x0021);
static const ::gdcm::Tag s_SERIES_TIME_TAG(0x0008, 0x0031);
static const ::gdcm::Tag s_SERIES_PHYSICIAN_NAMES_TAG(0x0008, 0x1050);
static const ::gdcm::Tag s_SERIES_PROTOCOL_NAME_TAG(0x0018, 0x1030);
static const ::gdcm::Tag s_SERIES_DESCRIPTION_TAG(0x0008, 0x103e);
static const ::gdcm::Tag s_SERIES_BODY_PART_EXAMINED_TAG(0x0018, 0x0015);
static const ::gdcm::Tag s_SERIES_PATIENT_POSITION_TAG(0x0018, 0x5100);
static const ::gdcm::Tag s_SERIES_ANATOMICAL_ORIENTATION_TYPE_TAG(0x0010, 0x2210);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_STEP_ID_TAG(0x0040, 0x0253);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_STEP_START_DATE_TAG(0x0040, 0x0244);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_STEP_START_TIME_TAG(0x0040, 0x0245);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_STEP_END_DATE_TAG(0x0040, 0x0250);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_STEP_END_TIME_TAG(0x0040, 0x0251);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_STEP_DESCRIPTION_TAG(0x0040, 0x0254);
static const ::gdcm::Tag s_SERIES_PERFORMED_PROCEDURE_COMMENTS_TAG(0x0040, 0x0280);

static const ::gdcm::Tag s_PATIENT_NAME_TAG(0x0010, 0x0010);
static const ::gdcm::Tag s_PATIENT_ID_TAG(0x0010, 0x0020);
static const ::gdcm::Tag s_PATIENT_BIRTHDATE_TAG(0x0010, 0x0030);
static const ::gdcm::Tag s_PATIENT_SEX_TAG(0x0010, 0x0040);

static const ::gdcm::Tag s_STUDY_UID_TAG(0x0020, 0x000d);
static const ::gdcm::Tag s_STUDY_ID_TAG(0x0020, 0x0010);
static const ::gdcm::Tag s_STUDY_DATE_TAG(0x0008, 0x0020);
static const ::gdcm::Tag s_STUDY_TIME_TAG(0x0008, 0x0030);
static const ::gdcm::Tag s_STUDY_REFERRING_PHYSICIAN_NAME_TAG(0x0008, 0x0090);
static const ::gdcm::Tag s_STUDY_CONSULTING_PHYSICIAN_NAME_TAG(0x0008, 0x009C);
static const ::gdcm::Tag s_STUDY_DESCRIPTION_TAG(0x0008, 0x1030);
static const ::gdcm::Tag s_STUDY_PATIENT_AGE_TAG(0x0010, 0x1010);
static const ::gdcm::Tag s_STUDY_PATIENT_SIZE_TAG(0x0010, 0x1020);
static const ::gdcm::Tag s_STUDY_PATIENT_WEIGHT_TAG(0x0010, 0x1030);
static const ::gdcm::Tag s_STUDY_PATIENT_BODY_MASS_INDEX_TAG(0x0010, 0x1022);

static const ::gdcm::Tag s_EQUIPMENT_INSTITUTION_NAME_TAG(0x0008, 0x0080);

static const ::gdcm::Tag s_IMAGE_CONTRAST_AGENT_TAG(0x0018, 0x0010);
static const ::gdcm::Tag s_IMAGE_CONTRAST_ROUTE_TAG(0x0018, 0x1040);
static const ::gdcm::Tag s_IMAGE_CONTRAST_VOLUME_TAG(0x0018, 0x1041);
static const ::gdcm::Tag s_IMAGE_CONTRAST_START_TIME_TAG(0x0018, 0x1042);
static const ::gdcm::Tag s_IMAGE_CONTRAST_STOP_TIME_TAG(0x0018, 0x1043);
static const ::gdcm::Tag s_IMAGE_CONTRAST_TOTAL_DOSE_TAG(0x0018, 0x1044);
static const ::gdcm::Tag s_IMAGE_CONTRAST_FLOW_RATE_TAG(0x0018, 0x1046);
static const ::gdcm::Tag s_IMAGE_CONTRAST_FLOW_DURATION_TAG(0x0018, 0x1047);
static const ::gdcm::Tag s_IMAGE_CONTRAST_INGREDIENT_TAG(0x0018, 0x1048);
static const ::gdcm::Tag s_IMAGE_CONTRAST_INGREDIENT_CONCENTRATION_TAG(0x0018, 0x1049);
static const ::gdcm::Tag s_IMAGE_TYPE_TAG(0x0008, 0x0008);
static const ::gdcm::Tag s_IMAGE_ACQUISITION_DATE_TAG(0x0008, 0x0022);
static const ::gdcm::Tag s_IMAGE_ACQUISITION_TIME_TAG(0x0008, 0x0032);

//------------------------------------------------------------------------------

// Defines a custom sorter based on the InstanceNumber DICOM tag.
bool sortByInstanceNumber(const ::gdcm::DataSet& ds1, const ::gdcm::DataSet& ds2 )
{
    ::gdcm::Attribute<0x0020, 0x0013> at1;
    at1.Set( ds1 );
    ::gdcm::Attribute<0x0020, 0x0013> at2;
    at2.Set( ds2 );
    return at1 < at2;
}

//------------------------------------------------------------------------------

SeriesDBReader::SeriesDBReader(::fwDataIO::reader::IObjectReader::Key) :
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

void SeriesDBReader::read()
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();
    std::vector<std::string> filenames;
    if(::fwData::location::have < ::fwData::location::Folder, ::fwDataIO::reader::IObjectReader > (this))
    {
        helper::GdcmHelper::searchRecursivelyFiles(this->getFolder(), filenames);
    }
    else if(::fwData::location::have < ::fwData::location::MultiFiles, ::fwDataIO::reader::IObjectReader > (this))
    {
        for(std::filesystem::path file :  this->getFiles())
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

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr SeriesDBReader::createSeriesDB(const std::filesystem::path& _dicomDir)
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getConcreteObject();

    std::vector<std::string> filenames;
    helper::GdcmHelper::searchRecursivelyFiles(_dicomDir, filenames);

    this->addSeries(seriesDB, filenames);
    return seriesDB;
}

//----------------------------------------------------------------------------------------

void SeriesDBReader::addSeries( const ::fwMedData::SeriesDB::sptr& _seriesDB,
                                const std::vector< std::string >& _filenames)
{
    //gdcm::Trace::SetDebug( 1 );
    //gdcm::Trace::SetWarning( 1 );
    //gdcm::Trace::SetError( 1 );

    ::gdcm::Scanner scanner;
    scanner.AddTag(s_SERIES_MODALITY_TAG);
    scanner.AddTag(s_SERIES_UID_TAG);
    scanner.AddTag(s_SERIES_NUMBER_TAG);
    scanner.AddTag(s_SERIES_LATERALITY_TAG);
    scanner.AddTag(s_SERIES_DATE_TAG);
    scanner.AddTag(s_SERIES_TIME_TAG);
    scanner.AddTag(s_SERIES_PHYSICIAN_NAMES_TAG);
    scanner.AddTag(s_SERIES_PROTOCOL_NAME_TAG);
    scanner.AddTag(s_SERIES_DESCRIPTION_TAG);
    scanner.AddTag(s_SERIES_BODY_PART_EXAMINED_TAG);
    scanner.AddTag(s_SERIES_PATIENT_POSITION_TAG);
    scanner.AddTag(s_SERIES_ANATOMICAL_ORIENTATION_TYPE_TAG);
    scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_STEP_ID_TAG);
    scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_STEP_START_DATE_TAG);
    scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_STEP_START_TIME_TAG);
    scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_STEP_END_DATE_TAG);
    scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_STEP_END_TIME_TAG);
    scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_STEP_DESCRIPTION_TAG);
    scanner.AddTag(s_SERIES_PERFORMED_PROCEDURE_COMMENTS_TAG);

    scanner.AddTag(s_PATIENT_NAME_TAG);
    scanner.AddTag(s_PATIENT_ID_TAG);
    scanner.AddTag(s_PATIENT_BIRTHDATE_TAG);
    scanner.AddTag(s_PATIENT_SEX_TAG);

    scanner.AddTag(s_STUDY_UID_TAG);
    scanner.AddTag(s_STUDY_ID_TAG);
    scanner.AddTag(s_STUDY_DATE_TAG);
    scanner.AddTag(s_STUDY_TIME_TAG);
    scanner.AddTag(s_STUDY_REFERRING_PHYSICIAN_NAME_TAG);
    scanner.AddTag(s_STUDY_CONSULTING_PHYSICIAN_NAME_TAG);
    scanner.AddTag(s_STUDY_DESCRIPTION_TAG);
    scanner.AddTag(s_STUDY_PATIENT_AGE_TAG);
    scanner.AddTag(s_STUDY_PATIENT_SIZE_TAG);
    scanner.AddTag(s_STUDY_PATIENT_WEIGHT_TAG);
    scanner.AddTag(s_STUDY_PATIENT_BODY_MASS_INDEX_TAG);

    scanner.AddTag(s_EQUIPMENT_INSTITUTION_NAME_TAG);

    scanner.AddTag(s_IMAGE_CONTRAST_AGENT_TAG);
    scanner.AddTag(s_IMAGE_CONTRAST_ROUTE_TAG);
    scanner.AddTag(s_IMAGE_CONTRAST_VOLUME_TAG);
    scanner.AddTag(s_IMAGE_CONTRAST_START_TIME_TAG);
    scanner.AddTag(s_IMAGE_CONTRAST_STOP_TIME_TAG);
    scanner.AddTag(s_IMAGE_CONTRAST_TOTAL_DOSE_TAG);
    scanner.AddTag(s_IMAGE_CONTRAST_FLOW_RATE_TAG);
    scanner.AddTag(s_IMAGE_CONTRAST_FLOW_DURATION_TAG);
    scanner.AddTag(s_IMAGE_CONTRAST_INGREDIENT_TAG);
    scanner.AddTag(s_IMAGE_CONTRAST_INGREDIENT_CONCENTRATION_TAG);
    scanner.AddTag(s_IMAGE_TYPE_TAG);
    scanner.AddTag(s_IMAGE_ACQUISITION_DATE_TAG);
    scanner.AddTag(s_IMAGE_ACQUISITION_TIME_TAG);

    try
    {
        const bool isScanned = scanner.Scan(_filenames);
        if(!isScanned)
        {
            SLM_ERROR("Scanner failed to read files");
            return;
        }

        typedef std::map< std::string, std::vector< std::string > > MapSeriesType;
        MapSeriesType mapSeries;

        const ::gdcm::Directory::FilenamesType keys = scanner.GetKeys();
        for(const std::string& filename : keys)
        {
            SLM_ASSERT("'"+filename+"' is not a key in the mapping table", scanner.IsKey(filename.c_str()));

            const std::string seriesUID = helper::GdcmHelper::getValue(scanner, filename, s_SERIES_UID_TAG);
            if(!seriesUID.empty())
            {
                std::string fileSetId = seriesUID;

                const std::string imageAcquisitionDate = helper::GdcmHelper::getValue(scanner, filename,
                                                                                      s_IMAGE_ACQUISITION_DATE_TAG);
                if(!imageAcquisitionDate.empty())
                {
                    fileSetId += "_";
                    fileSetId += imageAcquisitionDate;
                }

                const std::string imageType = helper::GdcmHelper::getValue(scanner, filename, s_IMAGE_TYPE_TAG);
                if(!imageType.empty())
                {
                    // Treatment of secondary capture dicom file.
                    SLM_TRACE("Image Type : " + std::string(imageType));
                    fileSetId += "_";
                    fileSetId += imageType;
                }
                mapSeries[fileSetId].push_back(filename);
            }
            else
            {
                SLM_ERROR("Unique identifier is require for all series and was not found in the file: " + filename);
            }
        }

        for(const auto& elt : mapSeries)
        {
            SLM_TRACE( "Processing: '" + elt.first + "' file set.");
            const MapSeriesType::mapped_type& files = elt.second;
            if ( !files.empty() )
            {
                // The modality attribute is mandatory for all DICOM,
                // so we can read it to check if the type is supported by this reader.
                const std::string seriesModality =
                    helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_MODALITY_TAG);
                SLM_ASSERT("The modality is required for all series", !seriesModality.empty());
                if(seriesModality == "SR" || seriesModality == "AU" || seriesModality == "DOC")
                {
                    SLM_WARN(
                        "The modality " + seriesModality +
                        " can't be read with this reader and will be not added to the seriesDB`");
                    continue;
                }

                if(seriesModality != "CT" && seriesModality != "MR" && seriesModality != "US")
                {
                    SLM_WARN("The modality " + seriesModality + " is maybe not support be this reader`");
                }

                ::fwMedData::ImageSeries::sptr series  = ::fwMedData::ImageSeries::New();
                ::fwMedData::Patient::sptr patient     = series->getPatient();
                ::fwMedData::Study::sptr study         = series->getStudy();
                ::fwMedData::Equipment::sptr equipment = series->getEquipment();

                _seriesDB->getContainer().push_back(series);

                ::gdcm::IPPSorter ippSorter;
                ippSorter.SetComputeZSpacing( true );
                ippSorter.SetZSpacingTolerance( 1e-3 );
                bool isSorted = ippSorter.Sort( files );
                std::vector<std::string> sorted;
                double zspacing = 0.;
                if(isSorted)
                {
                    sorted   = ippSorter.GetFilenames();
                    zspacing = ippSorter.GetZSpacing();
                    OSLM_TRACE("Found z-spacing:" << ippSorter.GetZSpacing());
                }
                else
                {
                    //  Else an error has been encountered.
                    //  We fall back to a more trivial sorting based on the InstanceNumber DICOM tag.
                    SLM_WARN("IPP Sorting failed. Falling back to Instance Number sorting.");
                    ::gdcm::Sorter sorter;
                    sorter.SetSortFunction(sortByInstanceNumber);
                    isSorted = sorter.StableSort(_filenames);
                    if(isSorted)
                    {
                        // If the custom sorted returns true, it worked
                        // and the filenames are sorted by InstanceNumber (ASC).
                        sorted = sorter.GetFilenames();
                    }
                    else
                    {
                        // There is nothing more we can do to sort DICOM files.
                        SLM_ERROR("Failed to sort '"+elt.first+"'");
                    }
                }

                vtkSmartPointer< vtkGDCMImageReader > reader = vtkSmartPointer< vtkGDCMImageReader >::New();
                vtkSmartPointer< vtkStringArray > fileArray  = vtkSmartPointer< vtkStringArray >::New();
                reader->FileLowerLeftOn();
                fileArray->Initialize();
                if(isSorted)
                {
                    SLM_TRACE("Success to sort '" + elt.first+"'");
                    if (!zspacing && sorted.size() > 1)
                    {
                        SLM_TRACE( "Guessing zspacing ..." );
                        if (!sorted.empty())
                        {
                            ::gdcm::Reader localReader1;
                            ::gdcm::Reader localReader2;
                            const std::string& f1 = *(sorted.begin());
                            const std::string& f2 = *(sorted.begin() + 1);
                            SLM_TRACE( "Search spacing in: '" + f1 +"'");
                            SLM_TRACE( "Search spacing in: '" + f2 +"'");

                            localReader1.SetFileName( f1.c_str() );
                            localReader2.SetFileName( f2.c_str() );
                            const bool canRead = localReader1.Read() && localReader2.Read();
                            if(canRead)
                            {
                                const std::vector<double> vOrigin1 =
                                    ::gdcm::ImageHelper::GetOriginValue(localReader1.GetFile());
                                const std::vector<double> vOrigin2 =
                                    ::gdcm::ImageHelper::GetOriginValue(localReader2.GetFile());
                                zspacing = vOrigin2[2] - vOrigin1[2];
                                OSLM_TRACE(
                                    "Found z-spacing:" << zspacing << " from : << " << vOrigin2[2] << " | " <<
                                        vOrigin1[2]);
                            }
                            SLM_ERROR_IF("Cannot read: '" + f1 + "' or: '" + f2 +"'", !canRead);
                        }
                    }
                }

                for(const std::string& file : sorted)
                {
                    SLM_TRACE("Add '" + file + "' to vtkGdcmReader");
                    fileArray->InsertNextValue(file.c_str());
                }

                ::fwData::Image::sptr pDataImage = ::fwData::Image::New();
                bool res = false;
                if (fileArray->GetNumberOfValues() > 0)
                {
                    reader->SetFileNames( fileArray );
                    try
                    {
                        SLM_TRACE("Read Series: '" + elt.first + "'");

                        //add progress observation
                        vtkSmartPointer< ::fwVtkIO::helper::vtkLambdaCommand > progressCallback =
                            vtkSmartPointer< ::fwVtkIO::helper::vtkLambdaCommand >::New();
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
                        catch(std::exception& e)
                        {
                            OSLM_ERROR("VTKImage to fwData::Image failed : "<<e.what());
                        }
                    }
                    catch (std::exception& e)
                    {
                        OSLM_ERROR( "Error during conversion : " << e.what() );
                    }
                    catch (...)
                    {
                        SLM_ERROR( "Unexpected error during conversion" );
                    }
                    m_job->finish();
                }

                if (res)
                {
                    SLM_ASSERT("No file to read", !files.empty());

                    vtkMedicalImageProperties* medprop = reader->GetMedicalImageProperties();

                    const std::string seriesUID
                        = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_UID_TAG);
                    const std::string seriesNumber
                        = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_NUMBER_TAG);
                    const std::string seriesLaterality
                        = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_LATERALITY_TAG);
                    const std::string seriesDate = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_DATE_TAG);
                    const std::string seriesTime = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_TIME_TAG);
                    const ::gdcm::Scanner::ValuesType physicianNames
                        = scanner.GetValues( s_SERIES_PHYSICIAN_NAMES_TAG);
                    ::fwMedData::DicomValuesType seriesPhysicianNames;
                    for(const std::string& name :  physicianNames)
                    {
                        ::fwMedData::DicomValuesType result;
                        ::boost::split( result, name, ::boost::is_any_of("\\"));
                        seriesPhysicianNames.reserve(seriesPhysicianNames.size() + result.size());
                        seriesPhysicianNames.insert(seriesPhysicianNames.end(), result.begin(), result.end());
                    }
                    const std::string seriesProtocolName
                        = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_PROTOCOL_NAME_TAG);
                    const std::string seriesDescription
                        = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_DESCRIPTION_TAG);
                    const std::string seriesBodypartExamined
                        = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_PROTOCOL_NAME_TAG);
                    const std::string seriesPatientPosition
                        = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_PATIENT_POSITION_TAG);
                    const std::string seriesAnatomicalOrientationType
                        = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_ANATOMICAL_ORIENTATION_TYPE_TAG);
                    const std::string seriesPerformedProcedureStepID
                        = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_PERFORMED_PROCEDURE_STEP_ID_TAG);
                    const std::string seriesPerformedProcedureStepStartDate
                        = helper::GdcmHelper::getValue(scanner, files[0],
                                                       s_SERIES_PERFORMED_PROCEDURE_STEP_START_DATE_TAG);
                    const std::string seriesPerformedProcedureStepStartTime
                        = helper::GdcmHelper::getValue(scanner, files[0],
                                                       s_SERIES_PERFORMED_PROCEDURE_STEP_START_TIME_TAG);
                    const std::string seriesPerformedProcedureStepEndDate
                        = helper::GdcmHelper::getValue(scanner, files[0],
                                                       s_SERIES_PERFORMED_PROCEDURE_STEP_END_DATE_TAG);
                    const std::string seriesPerformedProcedureStepEndTime
                        = helper::GdcmHelper::getValue(scanner, files[0],
                                                       s_SERIES_PERFORMED_PROCEDURE_STEP_END_TIME_TAG);
                    const std::string seriesPerformedProcedureStepDescription
                        = helper::GdcmHelper::getValue(scanner, files[0],
                                                       s_SERIES_PERFORMED_PROCEDURE_STEP_DESCRIPTION_TAG);
                    const std::string seriesPerformedProcedureComments
                        = helper::GdcmHelper::getValue(scanner, files[0], s_SERIES_PERFORMED_PROCEDURE_COMMENTS_TAG);

                    const std::string patientName
                        = helper::GdcmHelper::getValue(scanner, files[0], s_PATIENT_NAME_TAG);
                    const std::string patientId
                        = helper::GdcmHelper::getValue(scanner, files[0], s_PATIENT_ID_TAG);
                    const std::string patientBirthdate
                        = helper::GdcmHelper::getValue(scanner, files[0], s_PATIENT_BIRTHDATE_TAG);
                    const std::string patientSex
                        = helper::GdcmHelper::getValue(scanner, files[0], s_PATIENT_SEX_TAG);

                    const std::string studyUID
                        = helper::GdcmHelper::getValue(scanner, files[0], s_STUDY_UID_TAG);
                    const std::string studyID
                        = helper::GdcmHelper::getValue(scanner, files[0], s_STUDY_ID_TAG);
                    const std::string studyDate
                        = helper::GdcmHelper::getValue(scanner, files[0], s_STUDY_DATE_TAG);
                    const std::string studyTime
                        = helper::GdcmHelper::getValue(scanner, files[0], s_STUDY_TIME_TAG);
                    const std::string studyReferringPhysicianName
                        = helper::GdcmHelper::getValue(scanner, files[0], s_STUDY_REFERRING_PHYSICIAN_NAME_TAG);
                    const std::string studyConsultingPhysicianName
                        = helper::GdcmHelper::getValue(scanner, files[0], s_STUDY_CONSULTING_PHYSICIAN_NAME_TAG);
                    const std::string studyDescription
                        = helper::GdcmHelper::getValue(scanner, files[0], s_STUDY_DESCRIPTION_TAG);
                    const std::string studyPatientAge
                        = helper::GdcmHelper::getValue(scanner, files[0], s_STUDY_PATIENT_AGE_TAG);
                    const std::string studyPatientSize
                        = helper::GdcmHelper::getValue(scanner, files[0], s_STUDY_PATIENT_SIZE_TAG);
                    const std::string studyPatientWeight
                        = helper::GdcmHelper::getValue(scanner, files[0], s_STUDY_PATIENT_WEIGHT_TAG);
                    const std::string studyPatientBodyMassIndex
                        = helper::GdcmHelper::getValue(scanner, files[0], s_STUDY_PATIENT_BODY_MASS_INDEX_TAG);

                    const std::string equipmentInstitutionName
                        = helper::GdcmHelper::getValue(scanner, files[0], s_EQUIPMENT_INSTITUTION_NAME_TAG);

                    const std::string imageContrastAgent
                        = helper::GdcmHelper::getValue(scanner, files[0], s_IMAGE_CONTRAST_AGENT_TAG);
                    const std::string imageContrastRoute
                        = helper::GdcmHelper::getValue(scanner, files[0], s_IMAGE_CONTRAST_ROUTE_TAG);
                    const std::string imageContrastVolume
                        = helper::GdcmHelper::getValue(scanner, files[0], s_IMAGE_CONTRAST_VOLUME_TAG);
                    const std::string imageContrastStartTime
                        = helper::GdcmHelper::getValue(scanner, files[0], s_IMAGE_CONTRAST_START_TIME_TAG);
                    const std::string imageContrastStopTime
                        = helper::GdcmHelper::getValue(scanner, files[0], s_IMAGE_CONTRAST_STOP_TIME_TAG);
                    const std::string imageContrastTotalDose
                        = helper::GdcmHelper::getValue(scanner, files[0], s_IMAGE_CONTRAST_TOTAL_DOSE_TAG);
                    const std::string imageContrastFlowRate
                        = helper::GdcmHelper::getValue(scanner, files[0], s_IMAGE_CONTRAST_FLOW_RATE_TAG);
                    const std::string imageContrastFlowDuration
                        = helper::GdcmHelper::getValue(scanner, files[0], s_IMAGE_CONTRAST_FLOW_DURATION_TAG);
                    const std::string imageContrastIngredient
                        = helper::GdcmHelper::getValue(scanner, files[0], s_IMAGE_CONTRAST_INGREDIENT_TAG);
                    const std::string imageContrastIngredientConcentration
                        = helper::GdcmHelper::getValue(scanner, files[0],
                                                       s_IMAGE_CONTRAST_INGREDIENT_CONCENTRATION_TAG);
                    const std::string imageAcquisitionDate
                        = helper::GdcmHelper::getValue(scanner, files[0], s_IMAGE_ACQUISITION_DATE_TAG);
                    const std::string imageAcquisitionTime
                        = helper::GdcmHelper::getValue(scanner, files[0], s_IMAGE_ACQUISITION_TIME_TAG);

                    const double thickness = medprop->GetSliceThicknessAsDouble();//"0018|0050"
                    double center          = 0.0;
                    double width           = 0.0;
                    if (medprop->GetNumberOfWindowLevelPresets())//FIXME : Multiple preset !!!
                    {
                        medprop->GetNthWindowLevelPreset(0, &width, &center); //0028|1050,1051
                    }

                    // Image must have 3 dimensions
                    if(pDataImage->getNumberOfDimensions() == 2)
                    {
                        ::fwData::Image::Size imgSize = pDataImage->getSize2();
                        imgSize[2]                    = 1;
                        pDataImage->setSize2(imgSize);

                        ::fwData::Image::Origin imgOrigin = pDataImage->getOrigin2();
                        imgOrigin[2]                      = 0.;
                        pDataImage->setOrigin2(imgOrigin);
                    }

                    ::fwData::Image::Spacing vPixelSpacing = pDataImage->getSpacing2();
                    // assume z-spacing = 1 if not guessed
                    vPixelSpacing[2] = zspacing ? zspacing : (thickness ? thickness : 1.);
                    pDataImage->setSpacing2(vPixelSpacing);
                    pDataImage->setWindowCenter(center);
                    pDataImage->setWindowWidth(width);

                    series->setModality(seriesModality);
                    SLM_ASSERT("No series UID", !seriesUID.empty());
                    series->setInstanceUID(seriesUID);
                    series->setNumber(seriesNumber);
                    series->setLaterality(seriesLaterality);
                    series->setDate(seriesDate);
                    series->setTime(seriesTime);
                    series->setPerformingPhysiciansName(seriesPhysicianNames);
                    series->setProtocolName(seriesProtocolName);
                    series->setDescription(seriesDescription);
                    series->setBodyPartExamined(seriesBodypartExamined);
                    series->setPatientPosition(seriesPatientPosition);
                    series->setAnatomicalOrientationType(seriesAnatomicalOrientationType);
                    series->setPerformedProcedureStepID(seriesPerformedProcedureStepID);
                    series->setPerformedProcedureStepStartDate(seriesPerformedProcedureStepStartDate);
                    series->setPerformedProcedureStepStartTime(seriesPerformedProcedureStepStartTime);
                    series->setPerformedProcedureStepEndDate(seriesPerformedProcedureStepEndDate);
                    series->setPerformedProcedureStepEndTime(seriesPerformedProcedureStepEndTime);
                    series->setPerformedProcedureStepDescription(seriesPerformedProcedureStepDescription);
                    series->setPerformedProcedureComments(seriesPerformedProcedureComments);

                    patient->setName(patientName);
                    patient->setPatientId(patientId);
                    patient->setBirthdate(patientBirthdate);
                    patient->setSex(patientSex);

                    SLM_ASSERT("No study UID", !studyUID.empty());
                    study->setInstanceUID(studyUID);
                    study->setStudyID(studyID);
                    study->setDate(studyDate);
                    study->setTime(studyTime);
                    study->setReferringPhysicianName(studyReferringPhysicianName);
                    study->setConsultingPhysicianName(studyConsultingPhysicianName);
                    study->setDescription(studyDescription);
                    study->setPatientAge(studyPatientAge);
                    study->setPatientSize(studyPatientSize);
                    study->setPatientWeight(studyPatientWeight);
                    study->setPatientBodyMassIndex(studyPatientBodyMassIndex);

                    equipment->setInstitutionName(equipmentInstitutionName);

                    series->setImage(pDataImage);
                    series->setContrastAgent(imageContrastAgent);
                    series->setContrastRoute(imageContrastRoute);
                    series->setContrastVolume(imageContrastVolume);
                    series->setContrastStartTime(imageContrastStartTime);
                    series->setContrastStopTime(imageContrastStopTime);
                    series->setContrastTotalDose(imageContrastTotalDose);
                    series->setContrastFlowRate(imageContrastFlowRate);
                    series->setContrastFlowDuration(imageContrastFlowDuration);
                    series->setContrastIngredient(imageContrastIngredient);
                    series->setContrastIngredientConcentration(imageContrastIngredientConcentration);
                    series->setAcquisitionDate(imageAcquisitionDate);
                    series->setAcquisitionTime(imageAcquisitionTime);
                }
            }
        }
    }
    catch (std::exception& e)
    {
        OSLM_ERROR( "Try with another reader or retry with this reader on a specific subfolder : " << e.what() );
        for(const auto filename : _filenames)
        {
            SLM_ERROR("file error : " + filename );
        }
    }
}

} //namespace vtkGdcmIO
