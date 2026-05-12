/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/tools/dispatcher.hpp>
#include <core/tools/type_key_type_mapping.hpp>

#include <itkImageFileWriter.h>
#include <itkNiftiImageIO.h>

#include <filesystem>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

void nifti_image_writer::write(sight::core::progress::observer::sptr _progress)
{
    auto object_lock = get_object();

    auto do_write =
        []<class PIXELTYPE>(const data::image::csptr _image,
                            const std::string& _filename,
                            core::progress::observer::sptr _progress)
        {
            SIGHT_DEBUG("itk::ImageFileWriter with PIXELTYPE " << core::type::get<PIXELTYPE>().name());

            // Reader IO (*1*)
            auto image_io_write = ::itk::NiftiImageIO::New();

            // create writer
            using itk_image_type = ::itk::Image<PIXELTYPE, 3>;
            using writer_t       = ::itk::ImageFileWriter<itk_image_type>;
            auto writer = writer_t::New();
            progressor progress(writer, _progress);

            // create itk Image
            auto itk_image = io::itk::move_to_itk<itk_image_type>(_image);

            writer->SetFileName(_filename.c_str());
            writer->SetInput(itk_image);
            writer->SetImageIO(image_io_write); // (*3*)

            // save image;
            writer->Update();
        };

    auto file  = this->get_file();
    auto image = get_concrete_object();

    using sight::core::tools::dispatcher;
    using sight::core::tools::supported_dispatcher_types;
    dispatcher<supported_dispatcher_types, decltype(do_write)>::invoke(image->type(), image, file.string(), _progress);
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
