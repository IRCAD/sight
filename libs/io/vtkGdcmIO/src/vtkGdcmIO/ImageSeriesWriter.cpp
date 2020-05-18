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

#include "vtkGdcmIO/ImageSeriesWriter.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/Dispatcher.hpp>

#include <fwVtkIO/helper/vtkLambdaCommand.hpp>
#include <fwVtkIO/vtk.hpp>

#include <gdcmDicts.h>
#include <gdcmFilenameGenerator.h>
#include <gdcmGlobal.h>
#include <time.h>
#include <vtkGDCMImageWriter.h>
#include <vtkImageData.h>
#include <vtkMedicalImageProperties.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>

#include <filesystem>
#include <iostream>

fwDataIOWriterRegisterMacro( ::vtkGdcmIO::ImageSeriesWriter )

namespace vtkGdcmIO
{

//------------------------------------------------------------------------------

void setValue(vtkMedicalImageProperties* medprop,
              const std::uint16_t group,
              const std::uint16_t element,
              const std::string& value)
{
    const gdcm::Global& g = gdcm::Global::GetInstance();
    const gdcm::Dicts& ds = g.GetDicts();

    const gdcm::Tag tag(group, element);
    const gdcm::DictEntry& dicEntry = ds.GetDictEntry( tag );
    medprop->AddUserDefinedValue( dicEntry.GetName(), value.c_str() );
}

//------------------------------------------------------------------------------

ImageSeriesWriter::ImageSeriesWriter(::fwDataIO::writer::IObjectWriter::Key) :
    ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this),
    m_job(::fwJobs::Observer::New("VTK Image writer")),
    m_compressionTypes(CompressionTypes::RAW)
{
}

//------------------------------------------------------------------------------

void ImageSeriesWriter::write()
{
    ::fwMedData::ImageSeries::csptr imgSeries = this->getConcreteObject();
    ::fwData::Image::sptr dataImage           = imgSeries->getImage();
    ::fwMedData::Patient::sptr patient        = imgSeries->getPatient();
    ::fwMedData::Study::sptr study            = imgSeries->getStudy();
    ::fwMedData::Equipment::sptr equipment    = imgSeries->getEquipment();

    // Vtk Conversion
    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( dataImage, vtkImage );

    std::filesystem::path outputDirectory = this->getFolder();
    FW_RAISE_IF("'" << outputDirectory << "' is not a directory.",
                !std::filesystem::is_directory( outputDirectory ) );

    // Generate filenames
    const std::string gdcmfile = ( outputDirectory / "image_" ).string() + "%01d.dcm";
    ::gdcm::FilenameGenerator fg;
    fg.SetPattern( gdcmfile.c_str() );
    int nfiles = vtkImage->GetDimensions()[2];
    fg.SetNumberOfFilenames( nfiles );

    bool isGenerated = fg.Generate();
    FW_RAISE_IF("Filename generation failed.", !isGenerated);

    auto filenames = vtkSmartPointer<vtkStringArray>::New();
    for(unsigned int i = 0; i < fg.GetNumberOfFilenames(); ++i)
    {
        filenames->InsertNextValue( fg.GetFilename(i) );
    }

    // Medical informations
    auto medprop = vtkSmartPointer<vtkMedicalImageProperties>::New();

    std::string modality = imgSeries->getModality();
    if(modality.empty())
    {
        modality = "OT";
    }
    medprop->SetModality(modality.c_str());
    setValue(medprop, 0x0020, 0x0011, imgSeries->getNumber());
    setValue(medprop, 0x0020, 0x0060, imgSeries->getLaterality());
    setValue(medprop, 0x0008, 0x0021, imgSeries->getDate());
    setValue(medprop, 0x0008, 0x0031, imgSeries->getTime());
    ::fwMedData::DicomValuesType performingPhysicians = imgSeries->getPerformingPhysiciansName();
    std::stringstream physicians;
    if (!performingPhysicians.empty())
    {
        ::fwMedData::DicomValuesType::const_iterator itr = performingPhysicians.begin();

        physicians << *itr++;
        for (; itr < performingPhysicians.end(); ++itr)
        {
            physicians << "\\" << *itr;
        }

    }
    setValue(medprop, 0x0008, 0x1050, physicians.str());
    setValue(medprop, 0x0018, 0x1030, imgSeries->getProtocolName());
    medprop->SetSeriesDescription(imgSeries->getDescription().c_str());
    setValue(medprop, 0x0018, 0x0015, imgSeries->getBodyPartExamined());
    setValue(medprop, 0x0018, 0x5100, imgSeries->getPatientPosition());
    setValue(medprop, 0x0010, 0x2210, imgSeries->getAnatomicalOrientationType());
    setValue(medprop, 0x0040, 0x0253, imgSeries->getPerformedProcedureStepID());
    setValue(medprop, 0x0040, 0x0244, imgSeries->getPerformedProcedureStepStartDate());
    setValue(medprop, 0x0040, 0x0245, imgSeries->getPerformedProcedureStepStartTime());
    setValue(medprop, 0x0040, 0x0250, imgSeries->getPerformedProcedureStepEndDate());
    setValue(medprop, 0x0040, 0x0251, imgSeries->getPerformedProcedureStepEndTime());
    setValue(medprop, 0x0040, 0x0254, imgSeries->getPerformedProcedureStepDescription());
    setValue(medprop, 0x0040, 0x0280, imgSeries->getPerformedProcedureComments());

    medprop->SetPatientName(patient->getName().c_str());
    medprop->SetPatientID(patient->getPatientId().c_str());
    medprop->SetPatientBirthDate(patient->getBirthdate().c_str());
    medprop->SetPatientSex(patient->getSex().c_str());

    medprop->SetStudyID(study->getStudyID().c_str());
    medprop->SetStudyDate(study->getDate().c_str());
    setValue(medprop, 0x0008, 0x0030, study->getTime());
    setValue(medprop, 0x0008, 0x0090, study->getReferringPhysicianName());
    setValue(medprop, 0x0008, 0x009C, study->getConsultingPhysicianName());
    medprop->SetStudyDescription(study->getDescription().c_str());
    medprop->SetPatientAge(study->getPatientAge().c_str());
    setValue(medprop, 0x0010, 0x1020, study->getPatientSize());
    setValue(medprop, 0x0010, 0x1030, study->getPatientWeight());
    setValue(medprop, 0x0010, 0x1022, study->getPatientBodyMassIndex());

    medprop->SetInstitutionName(equipment->getInstitutionName().c_str());

    setValue(medprop, 0x0018, 0x0010, imgSeries->getContrastAgent());
    setValue(medprop, 0x0018, 0x1040, imgSeries->getContrastRoute());
    setValue(medprop, 0x0018, 0x1041, imgSeries->getContrastVolume());
    setValue(medprop, 0x0018, 0x1042, imgSeries->getContrastStartTime());
    setValue(medprop, 0x0018, 0x1043, imgSeries->getContrastStopTime());
    setValue(medprop, 0x0018, 0x1044, imgSeries->getContrastTotalDose());
    setValue(medprop, 0x0018, 0x1046, imgSeries->getContrastFlowRate());
    setValue(medprop, 0x0018, 0x1047, imgSeries->getContrastFlowDuration());
    setValue(medprop, 0x0018, 0x1048, imgSeries->getContrastIngredient());
    setValue(medprop, 0x0018, 0x1049, imgSeries->getContrastIngredientConcentration());
    medprop->SetAcquisitionDate(imgSeries->getAcquisitionDate().c_str());
    medprop->SetAcquisitionTime(imgSeries->getAcquisitionTime().c_str());
    medprop->AddWindowLevelPreset(dataImage->getWindowWidth(), dataImage->getWindowCenter() );

    // Spacing
    // tagkey = "0028|0030";
    std::string value;
    const auto spacing = dataImage->getSpacing2();
    value  = ::fwTools::getString< double >(spacing[0]);
    value += '\\';
    value += ::fwTools::getString< double >(spacing[1]);
    setValue(medprop, 0x0028, 0x0030, value);

    std::string origin;
    const ::fwData::Image::Origin orginVec = dataImage->getOrigin2();
    origin  = ::fwTools::getString< ::fwData::Image::Origin::value_type >(orginVec[0]);
    origin += '\\';
    origin  = ::fwTools::getString< ::fwData::Image::Origin::value_type >(orginVec[1]);

    if(dataImage->getNumberOfDimensions() > 2)
    {
        // Thickness
        // tagkey = "0018|0050";
        value = ::fwTools::getString< double >(spacing[2]);
        medprop->SetSliceThickness(value.c_str());

        // InterSlice
        // tagkey = "0018|0088";
        value = ::fwTools::getString< double >(spacing[2]);
        setValue(medprop, 0x0018, 0x0088, value);

        origin += '\\';
        origin  = ::fwTools::getString< ::fwData::Image::Origin::value_type >(orginVec[2]);
    }

    setValue(medprop, 0x0020, 0x0032, origin);

    // Writing data
    //--------------------------------------------------------------
    vtkSmartPointer< vtkGDCMImageWriter > writer = vtkSmartPointer< vtkGDCMImageWriter >::New();

    //add progress observation
    writer->SetStudyUID(study->getInstanceUID().c_str());
    writer->SetSeriesUID(imgSeries->getInstanceUID().c_str());
    writer->SetInputData( vtkImage );
    writer->FileLowerLeftOn();
    writer->SetFileDimensionality( 2 ); // test the 3D to 2D writing mode
    writer->SetMedicalImageProperties( medprop );
    writer->SetFileNames( filenames );
    writer->SetCompressionType(static_cast<int>(m_compressionTypes));

    using namespace fwVtkIO::helper;
    vtkSmartPointer<vtkLambdaCommand> progressCallback;
    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback([this](vtkObject* caller, long unsigned int, void* )
        {
            auto filter = static_cast<vtkGDCMImageWriter*>(caller);
            m_job->doneWork( filter->GetProgress()*100 );
        });
    writer->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook( [&]()
        {
            writer->AbortExecuteOn();
        });

    writer->Write();

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string ImageSeriesWriter::extension()
{
    return "";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr ImageSeriesWriter::getJob() const
{
    return m_job;
}

} // namespace dicomIO
