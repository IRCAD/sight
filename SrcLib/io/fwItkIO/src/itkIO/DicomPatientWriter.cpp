/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>

#include <time.h>

#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include <itkImageSeriesWriter.h>
#include <itkNumericSeriesFileNames.h>

#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/Dispatcher.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/dateAndTime.hpp>

#include <itkIO/itk.hpp>

#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>

#include <fwCore/base.hpp>

#include "itkIO/helper/ProgressItkToFw.hpp"
#include "itkIO/DicomPatientWriter.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::itkIO::DicomPatientWriter, ::itkIO::DicomPatientWriter );


namespace itkIO
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


struct ITKDicomSaverFunctor
{

    struct Parameter
    {
        std::string                        m_filename;
        ::fwData::Patient::sptr            m_dataPatient;
        ::itkIO::DicomPatientWriter::sptr  m_fwWriter;
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

                // create itk Image
                typedef itk::Image< PIXELTYPE, 3> itkImageType;
                typename itkImageType::Pointer itkImage = ::itkIO::itkImageFactory<itkImageType>( dataImage );

                //--------------------------------------------------------------------------------
                // Writer
                typedef itk::GDCMImageIO                        ImageIOType;
                typedef itk::NumericSeriesFileNames             NamesGeneratorType;

                ImageIOType::Pointer gdcmIO = ImageIOType::New();

                std::string outputDirectory = param.m_filename;
                outputDirectory += "/image_" + ::fwTools::getString< int >(idSerie);
                itksys::SystemTools::MakeDirectory( outputDirectory.c_str() );

                typedef signed short    OutputPixelType;
                const unsigned int      OutputDimension = 2;
                typedef itk::Image< OutputPixelType, OutputDimension >    Image2DType;
                typedef itk::ImageSeriesWriter< itkImageType, Image2DType >  SeriesWriterType;
                NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();
                itk::MetaDataDictionary & dict = gdcmIO->GetMetaDataDictionary();

                std::string tagkey, value;

                //--------------------------------------------------------------------------------
                // Series Instance UID
                tagkey =  "0020|000E";
                value = ::fwTools::getString< int >(idSerie);
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // Patient name
                tagkey = "0010|0010";
                if (param.m_dataPatient->getFirstname().empty())
                    value = param.m_dataPatient->getName();
                else
                    value = param.m_dataPatient->getName() + "^" + param.m_dataPatient->getFirstname();
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value);

                // Patient sex
                tagkey = "0010|0040";
                value = (param.m_dataPatient->getIsMale()?"M":"F");
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value);

                // Modality
                tagkey = "0008|0060";
                value = pStudy->getModality();
                if(value.empty())
                    value = "CT";
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // Zone
                tagkey = "0008|1030";
                value = pStudy->getAcquisitionZone();
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // ID
                tagkey =  "0010|0020";
                value = param.m_dataPatient->getIDDicom();
                //value += ::fwTools::getString< int >(idStudy);
                OSLM_INFO("ID: "<<value );
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // Birthdate
                tagkey = "0010|0030";
                value = ::fwTools::getDate(param.m_dataPatient->getBirthdate());
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // Hospital
                tagkey = "0008|0080";
                value = pStudy->getHospital();
                if(value.empty())
                    value = "unknown";
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // AcqDate
                tagkey = "0008|0022";
                value = ::fwTools::getDate(pAcquisition->getCreationDate());
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // AcqTime
                tagkey = "0008|0032";
                value = ::fwTools::getTime(pAcquisition->getCreationDate());
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // Spacing
                tagkey = "0028|0030";
                std::vector<double> spacing = dataImage->getCRefSpacing();
                value = ::fwTools::getString< double >(spacing[0]);
                value += '\\';
                value += ::fwTools::getString< double >(spacing[1]);
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );
                typename itkImageType::SpacingType spacingITK = itkImage->GetSpacing();

                // Thickness
                tagkey = "0018|0050";
                value = ::fwTools::getString< double >(spacing[2]);
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // InterSlice
                tagkey = "0018|0088";
                value = ::fwTools::getString< double >(spacing[2]);
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // Center
                tagkey = "0028|1050";
                value = ::fwTools::getString< double >(dataImage->getWindowCenter());
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // Width
                tagkey = "0028|1051";
                value = ::fwTools::getString< double >(dataImage->getWindowWidth());
                itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                // Image Type
                tagkey = "0008|0008";
                value = pAcquisition->getImageType();
                if(!value.empty())
                    itk::EncapsulateMetaData<std::string>(dict, tagkey, value );

                //--------------------------------------------------------------------------------

                typename SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();

                seriesWriter->SetInput( itkImage );
                seriesWriter->SetImageIO( gdcmIO );

                typename itkImageType::RegionType region = itkImage->GetLargestPossibleRegion();
                typename itkImageType::IndexType start = region.GetIndex();
                typename itkImageType::SizeType  size  = region.GetSize();

                std::string format = outputDirectory;
                std::stringstream file_str;
                file_str << "/image_%03d.dcm";

                format += file_str.str();

                namesGenerator->SetSeriesFormat( format.c_str() );
                namesGenerator->SetStartIndex( start[2] );
                namesGenerator->SetEndIndex( start[2] + size[2] - 1 );
                namesGenerator->SetIncrementIndex( 1 );

                seriesWriter->SetFileNames( namesGenerator->GetFileNames() );

                std::stringstream msg;
                msg << "Path : " << outputDirectory << " - Serie : " << nbSerie << std::endl;
                // now progress event from itkReader are forwared to fwReader
                itk::LightProcessObject::Pointer castHelper = (itk::LightProcessObject *)(seriesWriter.GetPointer());
                Progressor progress(castHelper, param.m_fwWriter, msg.str());

                seriesWriter->Update();
            }
        }
        //--------------------------------------------------------------------------------

    }
};

//------------------------------------------------------------------------------

void DicomPatientWriter::write()
{
    assert( m_object.use_count() );
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ITKDicomSaverFunctor::Parameter saverParam;
    saverParam.m_filename =  this->getFolder().string();
    saverParam.m_dataPatient = this->getConcreteObject();
    saverParam.m_fwWriter =  this->getSptr();
    assert( saverParam.m_dataPatient );
    // Study selection
    ::fwData::Patient::StudyIterator studyIter = saverParam.m_dataPatient->getStudies().first;
    // Acquisition selection
    ::fwData::Study::AcquisitionIterator acquisitionIter = (*studyIter)->getAcquisitions().first;

    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes , ITKDicomSaverFunctor >::invoke( (*acquisitionIter)->getImage()->getPixelType(), saverParam );
}

//------------------------------------------------------------------------------
std::string  DicomPatientWriter::extension()
{
    return "";
}
} // namespace itkIO
