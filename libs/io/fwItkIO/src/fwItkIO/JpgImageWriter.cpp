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

#include "fwItkIO/JpgImageWriter.hpp"

#include "fwItkIO/helper/ProgressItkToFw.hpp"
#include "fwItkIO/itk.hpp"

#include <core/base.hpp>
#include <core/tools/Dispatcher.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/tools/fieldHelper/Image.hpp>
#include <data/tools/fieldHelper/MedicalImageHelpers.hpp>
#include <data/TransferFunction.hpp>

#include <io/base/writer/registry/macros.hpp>

#include <itkImageSeriesWriter.h>
#include <itkIntensityWindowingImageFilter.h>
#include <itkJPEGImageIOFactory.h>
#include <itkNumericSeriesFileNames.h>

#include <filesystem>

fwDataIOWriterRegisterMacro( ::fwItkIO::JpgImageWriter );

namespace fwItkIO
{

//------------------------------------------------------------------------------

JpgImageWriter::JpgImageWriter(io::base::writer::IObjectWriter::Key key) :
    data::location::enableFolder< io::base::writer::IObjectWriter >(this)
{
}

//------------------------------------------------------------------------------

JpgImageWriter::~JpgImageWriter()
{
}

//------------------------------------------------------------------------------

struct JpgITKSaverFunctor
{
    JpgITKSaverFunctor()
    {
        // force register/link_with JPEGImageIOFactory
        ::itk::JPEGImageIOFactory::RegisterOneFactory();
    }
    struct Parameter
    {
        std::string m_filename;
        data::Image::csptr m_dataImage;
        ::fwItkIO::JpgImageWriter::sptr m_fwWriter;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()( const Parameter& param )
    {
        SLM_DEBUG( "itk::ImageSeriesWriter with PIXELTYPE "<<  core::tools::Type::create<PIXELTYPE>().string() );

        data::Image::csptr image = param.m_dataImage;

        // VAG attention : ImageFileReader ne notifie AUCUNE progressEvent mais son ImageIO oui!!!! mais ImageFileReader
        // ne permet pas de l'atteindre
        // car soit mis a la mano ou alors construit lors de l'Update donc trop tard
        // Il faut dont creer une ImageIO a la mano (*1*): affecter l'observation  sur IO (*2*) et mettre le IO dans le
        // reader (voir *3*)

        // Reader IO (*1*)
        typename itk::ImageIOBase::Pointer imageIOWrite = itk::ImageIOFactory::CreateImageIO( "image.jpg",
                                                                                              itk::ImageIOFactory::WriteMode);
        assert( imageIOWrite.IsNotNull() );

        // create writer
        typedef itk::Image< PIXELTYPE, 3> itkImageType;
        typedef itk::Image< unsigned char, 2 >      Image2DType;
        typedef typename itk::ImageSeriesWriter< itkImageType, Image2DType > WriterType;
        typename WriterType::Pointer writer = WriterType::New();

        // set observation (*2*)
        itk::LightProcessObject::Pointer castHelper = (itk::LightProcessObject*)(imageIOWrite.GetPointer());
        assert( castHelper.IsNotNull() );
        Progressor progress(castHelper, param.m_fwWriter, param.m_filename);

        // create itk Image
        typename itkImageType::Pointer itkImage = ::fwItkIO::itkImageFactory<itkImageType>( image );

        typedef ::itk::IntensityWindowingImageFilter< itkImageType, itkImageType > RescaleFilterType;
        typename RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();

        double min, max;
        data::Composite::sptr poolTF;
        poolTF =
            image->getField< data::Composite>( data::tools::fieldHelper::Image::m_transferFunctionCompositeId );
        if(poolTF)
        {
            data::Composite::iterator iter = poolTF->find(data::TransferFunction::s_DEFAULT_TF_NAME);
            if(iter != poolTF->end())
            {
                data::TransferFunction::sptr tf;
                tf  = data::TransferFunction::dynamicCast(iter->second);
                min = tf->getWLMinMax().first;
                max = tf->getWLMinMax().second;
            }
        }
        else
        {
            data::tools::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
        }

        rescaleFilter->SetWindowMinimum( min );
        rescaleFilter->SetWindowMaximum( max );
        rescaleFilter->SetOutputMinimum( 0 );
        rescaleFilter->SetOutputMaximum( 255 );
        rescaleFilter->InPlaceOff();
        rescaleFilter->SetInput( itkImage );
        rescaleFilter->Update();

        writer->SetInput( rescaleFilter->GetOutput() );

        typedef itk::NumericSeriesFileNames NameGeneratorType;

        NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();

        std::string format = param.m_filename;
        format += "/%04d.jpg";
        nameGenerator->SetSeriesFormat( format.c_str() );
        nameGenerator->SetStartIndex( 1 );
        nameGenerator->SetEndIndex( image->getSize2()[2] );
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
    saverParam.m_filename  = this->getFolder().string();
    saverParam.m_dataImage = this->getConcreteObject();
    saverParam.m_fwWriter  = this->getSptr();
    assert( saverParam.m_dataImage );

    core::tools::Dispatcher< core::tools::SupportedDispatcherTypes, JpgITKSaverFunctor >::invoke(
        saverParam.m_dataImage->getType(), saverParam );
}

//------------------------------------------------------------------------------

std::string JpgImageWriter::extension()
{
    return ".jpg";
}

} // namespace fwItkIO
