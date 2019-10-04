/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwItkIO/ImageWriter.hpp"

#include "fwItkIO/helper/ProgressItkToFw.hpp"
#include "fwItkIO/itk.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>

#include <filesystem>
#include <itkImageFileWriter.h>

fwDataIOWriterRegisterMacro( ::fwItkIO::ImageWriter );

namespace fwItkIO
{
//------------------------------------------------------------------------------

ImageWriter::ImageWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

ImageWriter::~ImageWriter()
{
    SLM_TRACE_FUNC();
}

struct ITKSaverFunctor
{

    struct Parameter
    {
        std::string m_filename;
        ::fwData::Image::csptr m_dataImage;
        ::fwItkIO::ImageWriter::sptr m_fwWriter;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()( const Parameter& param )
    {
        OSLM_DEBUG( "itk::ImageFileWriter with PIXELTYPE "<<  fwTools::DynamicType::string<PIXELTYPE>() );

        // VAG attention : ImageFileReader ne notifie AUCUNE progressEvent mais son ImageIO oui!!!! mais ImageFileReader
        // ne permet pas de l'atteindre
        // car soit mis a la mano ou alors construit lors de l'Update donc trop tard
        // Il faut dont creer une ImageIO a la mano (*1*): affecter l'observation  sur IO (*2*) et mettre le IO dans le
        // reader (voir *3*)

        // Reader IO (*1*)
        typename itk::ImageIOBase::Pointer imageIOWrite = itk::ImageIOFactory::CreateImageIO(
            param.m_filename.c_str(), itk::ImageIOFactory::WriteMode);
        assert( imageIOWrite.IsNotNull() );

        // create writer
        typedef itk::Image< PIXELTYPE, 3> itkImageType;
        typedef typename itk::ImageFileWriter< itkImageType >      WriterType;
        typename WriterType::Pointer writer = WriterType::New();

        // set observation (*2*)
        itk::LightProcessObject::Pointer castHelper = (itk::LightProcessObject*)(imageIOWrite.GetPointer());
        assert( castHelper.IsNotNull() );
        Progressor progress(castHelper, param.m_fwWriter, param.m_filename);

        // create itk Image
        typename itkImageType::Pointer itkImage = ::fwItkIO::itkImageFactory<itkImageType>( param.m_dataImage );

        writer->SetFileName( param.m_filename.c_str() );
        writer->SetInput( itkImage );
        writer->SetImageIO( imageIOWrite  ); // (*3*)

        // save image;
        writer->Update();
    }
};

//------------------------------------------------------------------------------

void ImageWriter::write()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ITKSaverFunctor::Parameter saverParam;
    saverParam.m_filename  = this->getFile().string();
    saverParam.m_dataImage = getConcreteObject();
    saverParam.m_fwWriter  = this->getSptr();
    assert( saverParam.m_dataImage );

    ::fwTools::Dispatcher< fwTools::IntrinsicTypes, ITKSaverFunctor >::invoke(
        saverParam.m_dataImage->getPixelType(), saverParam );
}

//------------------------------------------------------------------------------

std::string ImageWriter::extension()
{
    if ( getFile().empty() || ( getFile().string().find(".inr.gz") != std::string::npos ) )
    {
        return ".inr.gz";
    }
    else
    {
        return getFile().filename().extension().string();
    }
}

//------------------------------------------------------------------------------

} // namespace fwItkIO
