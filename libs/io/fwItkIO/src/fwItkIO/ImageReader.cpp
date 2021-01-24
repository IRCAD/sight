/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwItkIO/ImageReader.hpp"

#include "fwItkIO/helper/ProgressItkToFw.hpp"
#include "fwItkIO/itk.hpp"

#include "inr2itk/itkInrImageIOFactory.hpp"

#include <core/base.hpp>
#include <core/tools/Dispatcher.hpp>
#include <core/tools/IntrinsicTypes.hpp>
#include <core/tools/TypeInfoKeyTypeMapping.hpp>

#include <data/Image.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <itkImageFileReader.h>
#include <itkImageIOFactory.h>

#include <filesystem>

fwDataIOReaderRegisterMacro( ::fwItkIO::ImageReader );

namespace fwItkIO
{

//------------------------------------------------------------------------------

ImageReader::ImageReader(::fwDataIO::reader::IObjectReader::Key )  :
    data::location::enableSingleFile<
        IObjectReader >(this)
{
}

//------------------------------------------------------------------------------

ImageReader::~ImageReader()
{
}

//------------------------------------------------------------------------------

struct ITKLoaderFunctor
{
    struct Parameter
    {
        data::Image::sptr m_dataImage;
        std::string m_filename;
        ::fwItkIO::ImageReader::sptr m_fwReader;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameter& param)
    {
        SLM_INFO( "::fwItkIO::ImageReader::ITKLoaderFunctor with PIXELTYPE "<<
                  core::tools::Type::create<PIXELTYPE>().string());

        // VAG attention : ImageFileReader ne notifie AUCUNE progressEvent mais son ImageIO oui!!!! mais ImageFileReader
        // ne permet pas de l'atteindre
        // car soit mis a la mano ou alors construit lors de l'Update donc trop tard
        // Il faut dont creer une ImageIO a la mano (*1*): affecter l'observation  sur IO (*2*) et mettre le IO dans le
        // reader (voir *3*)

        // Reader IO (*1*)
        typename itk::ImageIOBase::Pointer imageIORead = itk::ImageIOFactory::CreateImageIO(
            param.m_filename.c_str(), itk::ImageIOFactory::ReadMode);

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
        ::fwItkIO::dataImageFactory< ImageType>( itkimage, param.m_dataImage );
    }

    //// get pixel type from Header
    static const std::type_info& getImageType( const std::string& imageFileName )
    {
        itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(
            imageFileName.c_str(), itk::ImageIOFactory::ReadMode);

        if( !imageIO )
        {
            std::string errMsg;
            errMsg = "no ImageIOFactory found to read header of file : ";
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
    std::filesystem::path file = getFile();
    SLM_ASSERT("File: "<<file<<" doesn't exist", std::filesystem::exists( file ) );
    assert( !m_object.expired() );
    assert( m_object.lock() );

    const std::type_info& ti = ITKLoaderFunctor::getImageType( file.string() );

    ITKLoaderFunctor::Parameter param;
    param.m_filename  = file.string();
    param.m_dataImage = this->getConcreteObject();
    param.m_fwReader  = this->getSptr();

    core::tools::Dispatcher< core::tools::IntrinsicTypes, ITKLoaderFunctor >::invoke(ti, param );

    SLM_ASSERT("data::Image is not well produced", m_object.lock() ); // verify that data::Image is well
    // produced
    // Post Condition image with a pixel type
    SLM_ASSERT("Image has an unspecified type",
               getConcreteObject()->getType() != core::tools::Type::s_UNSPECIFIED_TYPE );
}

//------------------------------------------------------------------------------

} // namespace fwItkIO
