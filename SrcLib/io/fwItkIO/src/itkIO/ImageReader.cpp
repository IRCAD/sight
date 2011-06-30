/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>

#include <itkImageFileReader.h>
#include <itkImageIOFactory.h>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <itkIO/itk.hpp>


#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/Dispatcher.hpp>
#include <fwTools/TypeInfoKeyTypeMapping.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include "inr2itk/itkInrImageIOFactory.h"

#include "itkIO/ImageReader.hpp"
#include "itkIO/helper/ProgressItkToFw.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::itkIO::ImageReader,  ::itkIO::ImageReader );


namespace itkIO
{

//------------------------------------------------------------------------------

ImageReader::ImageReader()  : ::fwData::location::enableSingleFile< IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

ImageReader::~ImageReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

struct ITKLoaderFunctor
{
    struct Parameter
    {
        ::fwData::Image::sptr       m_dataImage;
        std::string                 m_filename;
        ::itkIO::ImageReader::sptr  m_fwReader;
    };

    template<class PIXELTYPE>
    void operator()(Parameter &param)
    {
        OSLM_INFO( "::itkIO::ImageReader::ITKLoaderFunctor with PIXELTYPE "<<  ::fwTools::DynamicType::string<PIXELTYPE>() );

        // VAG attention : ImageFileReader ne notifie AUCUNE progressEvent mais son ImageIO oui!!!! mais ImageFileReader ne permet pas de l'atteindre
        // car soit mis a la mano ou alors construit lors de l'Update donc trop tard
        // Il faut dont creer une ImageIO a la mano (*1*): affecter l'observation  sur IO (*2*) et mettre le IO dans le reader (voir *3*)

        // Reader IO (*1*)
        typename itk::ImageIOBase::Pointer imageIORead = itk::ImageIOFactory::CreateImageIO( param.m_filename.c_str(), itk::ImageIOFactory::ReadMode);

        // set observation (*2*)
        Progressor progress(imageIORead, param.m_fwReader, param.m_filename);

        // the reader
        typedef itk::Image< PIXELTYPE, 3 >              ImageType;
        typedef itk::ImageFileReader< ImageType >       ReaderType;
        typename ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName( param.m_filename.c_str() );
        // attach its IO (*3*)
        reader->SetImageIO( imageIORead );

        reader->Update();
        typename ImageType::Pointer itkimage = reader->GetOutput();
        ::itkIO::dataImageFactory< ImageType>( itkimage, param.m_dataImage );
    }

    //// get pixel type from Header
    static const std::type_info& getImageType( const std::string &imageFileName )
    {
        itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(imageFileName.c_str(), itk::ImageIOFactory::ReadMode);

        if( !imageIO )
        {
            std::string errMsg;
            errMsg  = "no ImageIOFactory found to read header of file : ";
            errMsg.append( imageFileName );

            throw( std::ios_base::failure( errMsg ) );
        }

        imageIO->SetFileName( imageFileName.c_str() );
        imageIO->ReadImageInformation();
        return imageIO->GetComponentTypeInfo();
    }
};

//------------------------------------------------------------------------------

void ImageReader::read()
{
    ::boost::filesystem::path file = getFile();
    OSLM_ASSERT("File: "<<file<<" doesn't exist", ::boost::filesystem::exists( file ) );
    assert( !m_object.expired() );
    assert( m_object.lock() );

    const std::type_info& ti    = ITKLoaderFunctor::getImageType( file.string() );

    ITKLoaderFunctor::Parameter param;
    param.m_filename =  file.string();
    param.m_dataImage = this->getConcreteObject();
    param.m_fwReader = this->getSptr();

    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes , ITKLoaderFunctor >::invoke(ti , param );

    assert( m_object.lock() ); // verify that ::fwData::Image is well produced
    // Post Condition image with a pixel type
    SLM_ASSERT("Image has an unspecified type", getConcreteObject()->getPixelType() != ::fwTools::DynamicType() );
}

//------------------------------------------------------------------------------

} // namespace itkIO
