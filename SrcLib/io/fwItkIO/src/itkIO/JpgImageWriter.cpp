/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>

#include <fwCore/base.hpp>

#include <itkImageSeriesWriter.h>
#include <itkNumericSeriesFileNames.h>
#include <itkIntensityWindowingImageFilter.h>

#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/Dispatcher.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>

#include <fwComEd/Dictionary.hpp>

#include <fwData/Integer.hpp>

#include <itkIO/itk.hpp>

#include "itkIO/helper/ProgressItkToFw.hpp"
#include "itkIO/JpgImageWriter.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::itkIO::JpgImageWriter, ::itkIO::JpgImageWriter );


namespace itkIO
{

//------------------------------------------------------------------------------

JpgImageWriter::JpgImageWriter() :
        ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

JpgImageWriter::~JpgImageWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

struct JpgITKSaverFunctor
{

    struct Parameter
    {
        std::string                   m_filename;
        ::fwData::Image::sptr         m_dataImage;
        ::itkIO::JpgImageWriter::sptr m_fwWriter;
    };

    template<class PIXELTYPE>
    void operator()( const  Parameter &param )
    {
        OSLM_DEBUG( "itk::ImageSeriesWriter with PIXELTYPE "<<  fwTools::DynamicType::string<PIXELTYPE>() );

        // VAG attention : ImageFileReader ne notifie AUCUNE progressEvent mais son ImageIO oui!!!! mais ImageFileReader ne permet pas de l'atteindre
        // car soit mis a la mano ou alors construit lors de l'Update donc trop tard
        // Il faut dont creer une ImageIO a la mano (*1*): affecter l'observation  sur IO (*2*) et mettre le IO dans le reader (voir *3*)

        // Reader IO (*1*)
        typename itk::ImageIOBase::Pointer imageIOWrite = itk::ImageIOFactory::CreateImageIO( "image.png", itk::ImageIOFactory::WriteMode);
        assert( imageIOWrite.IsNotNull() );

        // create writer
        typedef itk::Image< PIXELTYPE, 3> itkImageType;
        typedef itk::Image< unsigned char, 2 >      Image2DType;
        typedef typename  itk::ImageSeriesWriter< itkImageType, Image2DType > WriterType;
        typename WriterType::Pointer writer = WriterType::New();

        // set observation (*2*)
        itk::LightProcessObject::Pointer castHelper= (itk::LightProcessObject *)(imageIOWrite.GetPointer());
        assert( castHelper.IsNotNull() );
        Progressor progress(castHelper, param.m_fwWriter, param.m_filename);

        // create itk Image
        typename itkImageType::Pointer itkImage = ::itkIO::itkImageFactory<itkImageType>( param.m_dataImage );

        typedef ::itk::IntensityWindowingImageFilter< itkImageType, itkImageType > RescaleFilterType;
        typename RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();

        assert( param.m_dataImage->getFieldSize( ::fwComEd::Dictionary::m_windowMinId) &&
                param.m_dataImage->getFieldSize( ::fwComEd::Dictionary::m_windowMaxId));

        int min = param.m_dataImage->getFieldSingleElement< ::fwData::Integer >( fwComEd::Dictionary::m_windowMinId)->value();
        int max = param.m_dataImage->getFieldSingleElement< ::fwData::Integer >( fwComEd::Dictionary::m_windowMaxId)->value();

        rescaleFilter->SetWindowMinimum( min );
        rescaleFilter->SetWindowMaximum( max );
        rescaleFilter->SetOutputMinimum( 0 );
        rescaleFilter->SetOutputMaximum( 255 );
        rescaleFilter->InPlaceOff();
        rescaleFilter->SetInput( itkImage );
        rescaleFilter->Update();

        writer->SetInput( rescaleFilter->GetOutput() );

        typedef itk::NumericSeriesFileNames    NameGeneratorType;

        NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();

        std::string format = param.m_filename;
        format += "/%04d.jpg";
        nameGenerator->SetSeriesFormat( format.c_str() );
        nameGenerator->SetStartIndex( 1 );
        nameGenerator->SetEndIndex( param.m_dataImage->getSize()[2] );
        nameGenerator->SetIncrementIndex( 1 );

        writer->SetFileNames( nameGenerator->GetFileNames() );

        writer->SetImageIO( imageIOWrite  );

        // save image;
        writer->Update();
    }
};

//------------------------------------------------------------------------------

void JpgImageWriter::write()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    JpgITKSaverFunctor::Parameter saverParam;
    saverParam.m_filename =  this->getFolder().string();
    saverParam.m_dataImage = this->getConcreteObject();
    saverParam.m_fwWriter =  this->getSptr();
    assert( saverParam.m_dataImage );

    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes , JpgITKSaverFunctor >::invoke( saverParam.m_dataImage->getPixelType(), saverParam );
}

//------------------------------------------------------------------------------

std::string  JpgImageWriter::extension()
{
    return ".jpg";
}

} // namespace itkIO
