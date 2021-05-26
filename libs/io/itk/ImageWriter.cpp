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

#include "io/itk/ImageWriter.hpp"

#include "io/itk/helper/ProgressItkToFw.hpp"
#include "io/itk/itk.hpp"

#include <core/base.hpp>
#include <core/tools/Dispatcher.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <io/base/writer/registry/macros.hpp>

#include <itkImageFileWriter.h>

#include <filesystem>

SIGHT_REGISTER_IO_WRITER(::sight::io::itk::ImageWriter);

namespace sight::io::itk
{

//------------------------------------------------------------------------------

ImageWriter::ImageWriter(io::base::writer::IObjectWriter::Key key)
{
}

//------------------------------------------------------------------------------

ImageWriter::~ImageWriter()
{
}

struct ITKSaverFunctor
{
    struct Parameter
    {
        std::string m_filename;
        data::Image::csptr m_dataImage;
        io::itk::ImageWriter::sptr m_fwWriter;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(const Parameter& param)
    {
        SIGHT_DEBUG("itk::ImageFileWriter with PIXELTYPE " << core::tools::Type::create<PIXELTYPE>().string());

        // VAG attention : ImageFileReader ne notifie AUCUNE progressEvent mais son ImageIO oui!!!! mais ImageFileReader
        // ne permet pas de l'atteindre
        // car soit mis a la mano ou alors construit lors de l'Update donc trop tard
        // Il faut dont creer une ImageIO a la mano (*1*): affecter l'observation  sur IO (*2*) et mettre le IO dans le
        // reader (voir *3*)

        // Reader IO (*1*)
        typename ::itk::ImageIOBase::Pointer imageIOWrite = ::itk::ImageIOFactory::CreateImageIO(
            param.m_filename.c_str(),
            ::itk::ImageIOFactory::WriteMode
        );
        assert(imageIOWrite.IsNotNull());

        // create writer
        typedef ::itk::Image<PIXELTYPE, 3> itkImageType;
        typedef typename ::itk::ImageFileWriter<itkImageType> WriterType;
        typename WriterType::Pointer writer = WriterType::New();

        // set observation (*2*)
        ::itk::LightProcessObject::Pointer castHelper = (::itk::LightProcessObject*) (imageIOWrite.GetPointer());
        assert(castHelper.IsNotNull());
        Progressor progress(castHelper, param.m_fwWriter, param.m_filename);

        // create itk Image
        typename itkImageType::Pointer itkImage = io::itk::itkImageFactory<itkImageType>(param.m_dataImage);

        writer->SetFileName(param.m_filename.c_str());
        writer->SetInput(itkImage);
        writer->SetImageIO(imageIOWrite); // (*3*)

        // save image;
        writer->Update();
    }
};

//------------------------------------------------------------------------------

void ImageWriter::write()
{
    assert(!m_object.expired());
    assert(m_object.lock());

    ITKSaverFunctor::Parameter saverParam;
    saverParam.m_filename  = this->getFile().string();
    saverParam.m_dataImage = getConcreteObject();
    saverParam.m_fwWriter  = this->getSptr();
    assert(saverParam.m_dataImage);

    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, ITKSaverFunctor>::invoke(
        saverParam.m_dataImage->getType(),
        saverParam
    );
}

//------------------------------------------------------------------------------

std::string ImageWriter::extension()
{
    if(getFile().empty() || (getFile().string().find(".inr.gz") != std::string::npos))
    {
        return ".inr.gz";
    }
    else
    {
        return getFile().filename().extension().string();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk
