/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/itk/NiftiImageWriter.hpp"

#include "io/itk/helper/ProgressItkToFw.hpp"
#include "io/itk/itk.hpp"

#include <core/base.hpp>
#include <core/tools/dispatcher.hpp>
#include <core/tools/type_key_type_mapping.hpp>

#include <io/__/writer/registry/macros.hpp>

#include <itkImageFileWriter.h>
#include <itkNiftiImageIO.h>

#include <filesystem>

SIGHT_REGISTER_IO_WRITER(sight::io::itk::NiftiImageWriter);

namespace sight::io::itk
{

struct NiftiSaverFunctor
{
    struct Parameter
    {
        std::string m_filename;
        data::image::csptr m_dataImage;
        io::itk::NiftiImageWriter::sptr m_fwWriter;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(const Parameter& _param)
    {
        SIGHT_DEBUG("itk::ImageFileWriter with PIXELTYPE " << core::type::get<PIXELTYPE>().name());

        // Reader IO (*1*)
        typename ::itk::NiftiImageIO::Pointer image_io_write = ::itk::NiftiImageIO::New();

        // create writer
        using itkImageType = ::itk::Image<PIXELTYPE, 3>;
        using writer_t     = typename ::itk::ImageFileWriter<itkImageType>;
        typename writer_t::Pointer writer = writer_t::New();

        // set observation (*2*)
        ::itk::LightProcessObject::Pointer cast_helper = (::itk::LightProcessObject*) (image_io_write.GetPointer());
        assert(cast_helper.IsNotNull());
        Progressor progress(cast_helper, _param.m_fwWriter, _param.m_filename);

        // create itk Image
        typename itkImageType::Pointer itk_image = io::itk::move_to_itk<itkImageType>(_param.m_dataImage);

        writer->SetFileName(_param.m_filename.c_str());
        writer->SetInput(itk_image);
        writer->SetImageIO(image_io_write); // (*3*)

        // save image;
        writer->Update();
    }
};

//------------------------------------------------------------------------------

void NiftiImageWriter::write()
{
    assert(!m_object.expired());
    assert(m_object.lock());

    NiftiSaverFunctor::Parameter saver_param;
    saver_param.m_filename  = this->get_file().string();
    saver_param.m_dataImage = getConcreteObject();
    saver_param.m_fwWriter  = this->get_sptr();
    assert(saver_param.m_dataImage);

    core::tools::dispatcher<core::tools::supported_dispatcher_types, NiftiSaverFunctor>::invoke(
        saver_param.m_dataImage->getType(),
        saver_param
    );
}

//------------------------------------------------------------------------------

std::string NiftiImageWriter::extension() const
{
    if(get_file().empty() || (get_file().string().find(".nii") != std::string::npos))
    {
        return ".nii";
    }

    return get_file().filename().extension().string();
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk
