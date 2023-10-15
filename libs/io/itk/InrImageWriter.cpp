/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/itk/InrImageWriter.hpp"

#include "io/itk/helper/ProgressItkToFw.hpp"
#include "io/itk/itk.hpp"

#include <core/base.hpp>
#include <core/tools/dispatcher.hpp>

#include <io/__/writer/registry/macros.hpp>

#include <itkImageFileWriter.h>
#include <itkNiftiImageIOFactory.h>

#include <filesystem>

SIGHT_REGISTER_IO_WRITER(sight::io::itk::InrImageWriter);

namespace sight::io::itk
{

struct InrSaverFunctor
{
    struct Parameter
    {
        std::string m_filename;
        data::image::csptr m_dataImage;
        io::itk::InrImageWriter::sptr m_fwWriter;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(const Parameter& param)
    {
        SIGHT_DEBUG("itk::ImageFileWriter with PIXELTYPE " << core::type::get<PIXELTYPE>().name());

        // Reader IO (*1*)
        typename ::itk::ImageIOBase::Pointer imageIOWrite = ::itk::ImageIOFactory::CreateImageIO(
            param.m_filename.c_str(),
            ::itk::ImageIOFactory::WriteMode
        );
        assert(imageIOWrite.IsNotNull());

        // create writer
        using itkImageType = ::itk::Image<PIXELTYPE, 3>;
        using WriterType   = typename ::itk::ImageFileWriter<itkImageType>;
        typename WriterType::Pointer writer = WriterType::New();

        // set observation (*2*)
        ::itk::LightProcessObject::Pointer castHelper = (::itk::LightProcessObject*) (imageIOWrite.GetPointer());
        assert(castHelper.IsNotNull());
        Progressor progress(castHelper, param.m_fwWriter, param.m_filename);

        // create itk Image
        typename itkImageType::Pointer itkImage = io::itk::moveToItk<itkImageType>(param.m_dataImage);

        writer->SetFileName(param.m_filename.c_str());
        writer->SetInput(itkImage);
        writer->SetImageIO(imageIOWrite); // (*3*)

        // save image;
        writer->Update();
    }
};

//------------------------------------------------------------------------------

void InrImageWriter::write()
{
    assert(!m_object.expired());
    assert(m_object.lock());

    InrSaverFunctor::Parameter saverParam;
    saverParam.m_filename  = this->get_file().string();
    saverParam.m_dataImage = getConcreteObject();
    saverParam.m_fwWriter  = this->get_sptr();
    assert(saverParam.m_dataImage);

    core::tools::dispatcher<core::tools::supported_dispatcher_types, InrSaverFunctor>::invoke(
        saverParam.m_dataImage->getType(),
        saverParam
    );
}

//------------------------------------------------------------------------------

std::string InrImageWriter::extension() const
{
    if(get_file().empty() || (get_file().string().find(".inr.gz") != std::string::npos))
    {
        return ".inr.gz";
    }

    return get_file().filename().extension().string();
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk
