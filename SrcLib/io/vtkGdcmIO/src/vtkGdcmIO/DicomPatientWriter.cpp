/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>

#include <iostream>
#include <time.h>

// Vtk
#include <vtkImageData.h>
#include <vtkStringArray.h>
#include <vtkSmartPointer.h>

// Gdcm
#include <vtkGDCMImageWriter.h>
#include <vtkMedicalImageProperties.h>
#include <gdcmFilenameGenerator.h>
#include <gdcmDicts.h>
#include <gdcmGlobal.h>

// SpyLog
#include <fwCore/base.hpp>

// fwTools
#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/Dispatcher.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/dateAndTime.hpp>


#include "vtkGdcmIO/DicomPatientWriter.hpp"
#include "vtkIO/vtk.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::vtkGdcmIO::DicomPatientWriter, ::vtkGdcmIO::DicomPatientWriter );


namespace vtkGdcmIO
{
//------------------------------------------------------------------------------

DicomPatientWriter::DicomPatientWriter()
: ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomPatientWriter::~DicomPatientWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

struct VTKDicomSaverFunctor
{

    struct Parameter
    {
        ::boost::filesystem::path   m_filename;
        ::fwData::Patient::sptr         m_dataPatient;
        DicomPatientWriter *        m_fwWriter;
    };

    template<class PIXELTYPE>
    void operator()( const  Parameter &param )
    {
        srand( time( NULL ) );
        int nbSerie=0;
        // Study selection
        ::fwData::Patient::StudyIterator studyIter;
        for(studyIter = param.m_dataPatient->getStudies().first ; studyIter != param.m_dataPatient->getStudies().second ; studyIter++)
        {
            ::fwData::Study::sptr pStudy = (*studyIter);
            // Acquisition selection
            ::fwData::Study::AcquisitionIterator acquisitionIter ;
            for(acquisitionIter = pStudy->getAcquisitions().first ; acquisitionIter != pStudy->getAcquisitions().second ; acquisitionIter++)
            {
                nbSerie++;
                int idSerie = int( double( rand() ) / ( double( RAND_MAX) + 1 ) * 1000000);

                ::fwData::Acquisition::sptr pAcquisition = (* acquisitionIter);
                ::fwData::Image::sptr dataImage = pAcquisition->getImage();

                // Vtk Conversion
                vtkSmartPointer< vtkImageData > myVtkImage = vtkSmartPointer< vtkImageData >::New();
                ::vtkIO::toVTKImage( dataImage, myVtkImage );

                // Manage filenames and directories
                std::string tmpStr = "";
                tmpStr += "image_" + ::fwTools::getString< int >(idSerie);
                ::boost::filesystem::path outputDirectory = param.m_filename / tmpStr;

                if ( ! ::boost::filesystem::exists( outputDirectory ) )
                {
                    ::boost::filesystem::create_directory( outputDirectory );
                }
                assert( ::boost::filesystem::is_directory( outputDirectory ) );

                std::string gdcmfile = ( outputDirectory / "image_" ).string();

                // Generate filenames
                std::ostringstream os;
                os << gdcmfile;
                os << "%01d";
                gdcm::FilenameGenerator fg;
                fg.SetPattern( os.str().c_str() );
                unsigned int nfiles = myVtkImage->GetDimensions()[2];
                fg.SetNumberOfFilenames( nfiles );
                bool b = fg.Generate();
                if( ! b )
                {
                    OSLM_FATAL("FilenameGenerator::Generate() failed");
                    assert( false && "FilenameGenerator::Generate() failed" );
                }
                if( ! fg.GetNumberOfFilenames() )
                {
                    OSLM_FATAL("FilenameGenerator::Generate() failed somehow...");
                    assert( false && "FilenameGenerator::Generate() failed somehow..." );
                }
                vtkStringArray *filenames = vtkStringArray::New();
                for(unsigned int i = 0; i < fg.GetNumberOfFilenames(); ++i)
                {
                    filenames->InsertNextValue( fg.GetFilename(i) );
                    OSLM_INFO( fg.GetFilename(i) );
                }
                assert( filenames->GetNumberOfValues() == (int)fg.GetNumberOfFilenames() );


                // Medical informations
                //--------------------------------------------------------------
                vtkMedicalImageProperties * medprop = vtkMedicalImageProperties::New();
                const gdcm::Global & g = gdcm::Global::GetInstance();
                const gdcm::Dicts & ds = g.GetDicts();

                std::string tagkey, value;

                // Patient name
                // tagkey = "0010|0010";
                if (param.m_dataPatient->getFirstname().empty())
                    value = param.m_dataPatient->getName();
                else
                    value = param.m_dataPatient->getName() + "^" + param.m_dataPatient->getFirstname();
                medprop->SetPatientName(value.c_str());


                // Patient sex
                // tagkey = "0010|0040";
                value = (param.m_dataPatient->getIsMale()?"M":"F");
                medprop->SetPatientSex(value.c_str());

                // Modality
                // tagkey = "0008|0060";
                value = pStudy->getModality();
                if(value.empty())
                    value = "CT";
                medprop->SetModality(value.c_str());

                // Zone
                // tagkey = "0008|1030";
                value = pStudy->getAcquisitionZone();
                medprop->SetStudyDescription(value.c_str());
                medprop->SetSeriesDescription("");


                // ID
                // tagkey =  "0010|0020";
                value = param.m_dataPatient->getIDDicom();
                //value += ::fwTools::getString< int >(idStudy);
                OSLM_INFO("ID: "<<value );
                medprop->SetPatientID(value.c_str());

                // Birthdate
                // tagkey = "0010|0030";
                value = ::fwTools::getDate(param.m_dataPatient->getBirthdate());
                medprop->SetPatientBirthDate(value.c_str());

                // Hospital
                // tagkey = "0008|0080";
                value = pStudy->getHospital();
                if(value.empty())
                    value = "unknown";
                medprop->SetInstitutionName(value.c_str());


                // AcqDate
                // tagkey = "0008|0022";
                value = ::fwTools::getDate(pAcquisition->getCreationDate());
                medprop->SetStudyDate(value.c_str());
                medprop->SetAcquisitionDate(value.c_str());

                // AcqTime
                // tagkey = "0008|0032";
                value = ::fwTools::getTime(pAcquisition->getCreationDate());
                medprop->SetStudyTime(value.c_str());
                medprop->SetAcquisitionTime(value.c_str());


                // Spacing
                // tagkey = "0028|0030";
                std::vector<double> spacing = dataImage->getCRefSpacing();
                value = ::fwTools::getString< double >(spacing[0]);
                value += '\\';
                value += ::fwTools::getString< double >(spacing[1]);

                const gdcm::Tag tagSpacing(0x0028,0x0030);
                const gdcm::DictEntry & dicEntrySpacing = ds.GetDictEntry( tagSpacing );
                medprop->AddUserDefinedValue( dicEntrySpacing.GetName(), value.c_str() );



                // Thickness
                // tagkey = "0018|0050";
                value = ::fwTools::getString< double >(spacing[2]);
                medprop->SetSliceThickness(value.c_str());


                // InterSlice
                // tagkey = "0018|0088";
                value = ::fwTools::getString< double >(spacing[2]);
                const gdcm::Tag tagInterSlice(0x0018,0x0088);
                const gdcm::DictEntry & dicEntryInterSlice = ds.GetDictEntry( tagInterSlice );
                medprop->AddUserDefinedValue( dicEntryInterSlice.GetName(), value.c_str() );


                // Center and Width
                // Center = tagkey = "0028|1050";
                // Width = "0028|1051";
                medprop->AddWindowLevelPreset( dataImage->getWindowWidth(), dataImage->getWindowCenter() );


/*
                // Image Type
                tagkey = "0008|0008";
                value = pAcquisition->getImageType();
                if(!value.empty())
                    VTK::EncapsulateMetaData<std::string>(dict, tagkey, value );
*/

                // Writing data
                //--------------------------------------------------------------
                vtkGDCMImageWriter * writer = vtkGDCMImageWriter::New();
                writer->SetInput( myVtkImage );

                // writer->SetFileLowerLeft( reader->GetFileLowerLeft() ); Replace by function
                writer->FileLowerLeftOn();

                // ? writer->SetDirectionCosines( reader->GetDirectionCosines() );
                // ? writer->SetImageFormat( reader->GetImageFormat() );
                writer->SetFileDimensionality( 2 ); // test the 3D to 2D writing mode
                writer->SetMedicalImageProperties( medprop );
                // ? writer->SetShift( reader->GetShift() );
                // ? writer->SetScale( reader->GetScale() );

                writer->SetFileNames( filenames );
                writer->Write();
                writer->Delete();
                filenames->Delete();
            }
        }
        //----------------------------------------------------------------------
    }
};

//------------------------------------------------------------------------------

void DicomPatientWriter::write()
{
    assert( m_object.use_count() );
    assert( !m_object.expired() );
    assert( m_object.lock() );

    VTKDicomSaverFunctor::Parameter saverParam;
    saverParam.m_filename =  this->getFolder();
    saverParam.m_dataPatient = getConcreteObject();
    saverParam.m_fwWriter =  this;
    assert( saverParam.m_dataPatient );
    // Study selection
    ::fwData::Patient::StudyIterator studyIter = saverParam.m_dataPatient->getStudies().first;
    // Acquisition selection
    ::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;

    ::fwTools::Dispatcher< fwTools::IntrinsicTypes , VTKDicomSaverFunctor >::invoke( (*acquisitionIter)->getImage()->getPixelType(), saverParam );
}

//------------------------------------------------------------------------------
std::string  DicomPatientWriter::extension()
{
    return "";
}

} // namespace dicomIO
