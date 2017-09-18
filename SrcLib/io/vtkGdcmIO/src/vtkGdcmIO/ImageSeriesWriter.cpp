/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>

#include <fwVtkIO/helper/vtkLambdaCommand.hpp>
#include <fwVtkIO/vtk.hpp>

#include <boost/filesystem.hpp>

#include <gdcmDicts.h>
#include <gdcmFilenameGenerator.h>
#include <gdcmGlobal.h>
#include <time.h>
#include <vtkGDCMImageWriter.h>
#include <vtkImageData.h>
#include <vtkMedicalImageProperties.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>

#include <iostream>

fwDataIOWriterRegisterMacro( ::vtkGdcmIO::ImageSeriesWriter );

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

ImageSeriesWriter::ImageSeriesWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this),
    m_job(::fwJobs::Observer::New("VTK Image writer")),
    m_compressionTypes(CompressionTypes::RAW)
{
}

//------------------------------------------------------------------------------

void ImageSeriesWriter::write()
{
    ::fwMedData::ImageSeries::sptr imgSeries = this->getConcreteObject();
    ::fwData::Image::sptr dataImage          = imgSeries->getImage();
    ::fwMedData::Patient::sptr patient       = imgSeries->getPatient();
    ::fwMedData::Study::sptr study           = imgSeries->getStudy();
    ::fwMedData::Equipment::sptr equipment   = imgSeries->getEquipment();

    // Vtk Conversion
    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( dataImage, vtkImage );

    ::boost::filesystem::path outputDirectory = this->getFolder();
    FW_RAISE_IF("'" << outputDirectory << "' is not a directory.",
                !::boost::filesystem::is_directory( outputDirectory ) );

    // Generate filenames
    const std::string gdcmfile = ( outputDirectory / "image_" ).string() + "%01d.dcm";
    ::gdcm::FilenameGenerator fg;
    fg.SetPattern( gdcmfile.c_str() );
    int nfiles = vtkImage->GetDimensions()[2];
    fg.SetNumberOfFilenames( nfiles );

    bool isGenerated = fg.Generate();
    FW_RAISE_IF("Filename generation failed.", !isGenerated);

    vtkStringArray* filenames = vtkStringArray::New();
    for(unsigned int i = 0; i < fg.GetNumberOfFilenames(); ++i)
    {
        filenames->InsertNextValue( fg.GetFilename(i) );
    }

    // Medical informations
    vtkMedicalImageProperties* medprop = vtkMedicalImageProperties::New();

    // Patient name
    // tagkey = "0010|0010";
    medprop->SetPatientName(patient->getName().c_str());

    // Patient sex
    // tagkey = "0010|0040";
    medprop->SetPatientSex(patient->getSex().c_str());

    // Modality
    // tagkey = "0008|0060";
    std::string modality = imgSeries->getModality();
    if(modality.empty())
    {
        modality = "OT";
    }
    medprop->SetModality(modality.c_str());

    // Zone
    // tagkey = "0008|1030";
    medprop->SetStudyDescription(study->getDescription().c_str());
    medprop->SetSeriesDescription(imgSeries->getDescription().c_str());

    // ID
    // tagkey =  "0010|0020";
    medprop->SetPatientID(patient->getPatientId().c_str());

    // Birthdate
    // tagkey = "0010|0030";
    medprop->SetPatientBirthDate(patient->getBirthdate().c_str());

    // Hospital
    // tagkey = "0008|0080";
    medprop->SetInstitutionName(equipment->getInstitutionName().c_str());

    // Patient age
    // Format: nnnD, nnW, nnnM or nnnY (eventually nnD, nnW, nnY)
    //         with D (day), M (month), W (week), Y (year)
    // For ex: DICOM (0010,1010) = 031Y
    medprop->SetPatientAge(study->getPatientAge().c_str());

    // Study Date
    // tagkey = "0008|0020";
    medprop->SetStudyDate(study->getDate().c_str());

    // Study Time
    // tagkey = "0008|0030";
//    medprop->SetStudyTime(study->getTime().c_str()); // Do not work (write current time)
    setValue(medprop, 0x0008, 0x0030, study->getTime());

    //Series Date
    setValue(medprop, 0x0008, 0x0021, imgSeries->getDate());

    // Series Time
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

    setValue(medprop, 0x0008, 0x0090, study->getReferringPhysicianName());

    // Center and Width
    // Center = tagkey = "0028|1050";
    // Width = "0028|1051";
    medprop->AddWindowLevelPreset( dataImage->getWindowWidth(), dataImage->getWindowCenter() );

    // Spacing
    // tagkey = "0028|0030";
    std::string value;
    std::vector<double> spacing = dataImage->getSpacing();
    value  = ::fwTools::getString< double >(spacing[0]);
    value += '\\';
    value += ::fwTools::getString< double >(spacing[1]);
    setValue(medprop, 0x0028, 0x0030, value);

    std::string origin;
    ::fwData::Image::OriginType orginVec = dataImage->getOrigin();
    origin                               = ::fwTools::getString< ::fwData::Image::OriginType::value_type >(orginVec[0]);
    origin                              += '\\';
    origin                               = ::fwTools::getString< ::fwData::Image::OriginType::value_type >(orginVec[1]);

    if (dataImage->getNumberOfDimensions() > 2)
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
        origin  = ::fwTools::getString< ::fwData::Image::OriginType::value_type >(orginVec[2]);
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

    filenames->Delete();
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
