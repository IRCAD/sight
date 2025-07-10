/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/itk/nifti_image_writer.hpp"

#include "io/itk/helper/progress_itk_to_fw.hpp"
#include "io/itk/itk.hpp"

#include <core/base.hpp>
#include <core/tools/dispatcher.hpp>
#include <core/tools/type_key_type_mapping.hpp>

#include <itkImageFileWriter.h>
#include <itkNiftiImageIO.h>

#include <filesystem>

namespace sight::io::itk
{

struct nifti_saver_functor
{
    struct parameter
    {
        std::string m_filename;
        data::image::csptr m_data_image;
        io::itk::nifti_image_writer::sptr m_fw_writer;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(const parameter& _param)
    {
        SIGHT_DEBUG("itk::ImageFileWriter with PIXELTYPE " << core::type::get<PIXELTYPE>().name());

        // Reader IO (*1*)
        auto image_io_write = ::itk::NiftiImageIO::New();

        // create writer
        using itk_image_type = ::itk::Image<PIXELTYPE, 3>;
        using writer_t       = typename ::itk::ImageFileWriter<itk_image_type>;
        auto writer = writer_t::New();

        // set observation (*2*)
        ::itk::LightProcessObject::Pointer cast_helper = (::itk::LightProcessObject*) (image_io_write.GetPointer());
        assert(cast_helper.IsNotNull());
        progressor progress(cast_helper, _param.m_fw_writer, _param.m_filename);

        // create itk Image
        auto itk_image = io::itk::move_to_itk<itk_image_type>(_param.m_data_image);

        writer->SetFileName(_param.m_filename.c_str());
        writer->SetInput(itk_image);
        writer->SetImageIO(image_io_write); // (*3*)

        // save image;
        writer->Update();
    }
};

//------------------------------------------------------------------------------

void nifti_image_writer::write()
{
    assert(!m_object.expired());
    assert(m_object.lock());

    nifti_saver_functor::parameter saver_param;
    saver_param.m_filename   = this->get_file().string();
    saver_param.m_data_image = get_concrete_object();
    saver_param.m_fw_writer  = this->get_sptr();
    assert(saver_param.m_data_image);

    core::tools::dispatcher<core::tools::supported_dispatcher_types, nifti_saver_functor>::invoke(
        saver_param.m_data_image->type(),
        saver_param
    );
}

//------------------------------------------------------------------------------

std::string nifti_image_writer::extension() const
{
    if(get_file().empty() || (get_file().string().find(".nii") != std::string::npos))
    {
        return ".nii";
    }

    return get_file().filename().extension().string();
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk
