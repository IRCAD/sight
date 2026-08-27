/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "io/itk/inr_image_writer.hpp"

#include "core/spy_log.hpp"

#include "io/itk/helper/progress_itk_to_fw.hpp"
#include "io/itk/itk.hpp"

#include <core/tools/dispatcher.hpp>

#include <itkImageFileWriter.h>

#include <filesystem>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

void inr_image_writer::write(sight::core::notification::observer::sptr _progress)
{
    auto object_lock = get_object();

    auto do_write =
        []<class PIXEL_TYPE>(
            data::image::csptr _data_image,
            const std::string& _filename,
            core::notification::observer::sptr _progress
    )
        {
            SIGHT_DEBUG("itk::ImageFileWriter with PIXEL_TYPE " << core::type::get<PIXEL_TYPE>().name());

            // Reader IO (*1*)
            ::itk::ImageIOBase::Pointer image_io_write = ::itk::ImageIOFactory::CreateImageIO(
                _filename.c_str(),
                ::itk::ImageIOFactory::WriteMode
            );
            assert(image_io_write.IsNotNull());

            // create writer
            using itk_image_type = ::itk::Image<PIXEL_TYPE, 3>;
            using writer_t       = ::itk::ImageFileWriter<itk_image_type>;
            typename writer_t::Pointer writer = writer_t::New();
            progressor progress(writer, _progress);

            // set observation (*2*)
            ::itk::LightProcessObject::Pointer cast_helper =
                static_cast<::itk::LightProcessObject*>(image_io_write.GetPointer());
            assert(cast_helper.IsNotNull());

            // create itk Image
            typename itk_image_type::Pointer itk_image = io::itk::move_to_itk<itk_image_type>(_data_image);

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

std::string inr_image_writer::extension() const
{
    if(get_file().empty() || (get_file().string().find(".inr.gz") != std::string::npos))
    {
        return ".inr.gz";
    }

    return get_file().filename().extension().string();
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk
