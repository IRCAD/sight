/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include "io/itk/nifti_image_reader.hpp"

#include "core/exceptionmacros.hpp"

#include "io/itk/helper/progress_itk_to_fw.hpp"
#include "io/itk/itk.hpp"

#include <core/progress/observer.hpp>
#include <core/tools/dispatcher.hpp>

#include <data/image.hpp>

#include <itkImageFileReader.h>
#include <itkNiftiImageIO.h>

#include <filesystem>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

//// get pixel type from Header
static const core::type& get_image_type(const std::string& _image_file_name)
{
    ::itk::NiftiImageIO::Pointer image_io = ::itk::NiftiImageIO::New();
    image_io->SetFileName(_image_file_name.c_str());
    image_io->ReadImageInformation();
    auto itk_type = image_io->GetComponentType();
    return sight::io::itk::ITK_TYPE_CONVERTER.at(itk_type);
}

//------------------------------------------------------------------------------

void nifti_image_reader::read(sight::core::progress::observer::sptr _progress)
{
    auto do_read =
        []<class PIXELTYPE>(data::image::sptr _image,
                            const std::string& _filename,
                            core::progress::observer::sptr _progress)
        {
            SIGHT_INFO(
                "::io::itk::NiftiImageReader::NiftiLoaderFunctor with PIXELTYPE "
                << core::type::get<PIXELTYPE>().name()
            );

            // Reader IO (*1*)
            ::itk::NiftiImageIO::Pointer image_io_read = ::itk::NiftiImageIO::New();
            image_io_read->SetFileName(_filename.c_str());
            image_io_read->ReadImageInformation();

            // the reader
            using image_t  = ::itk::Image<PIXELTYPE, 3>;
            using reader_t = ::itk::ImageFileReader<image_t>;
            typename reader_t::Pointer reader = reader_t::New();
            progressor progress(reader, _progress);

            reader->SetFileName(_filename);

            // attach its IO (*3*)
            reader->SetImageIO(image_io_read);

            reader->Update();
            typename image_t::Pointer itkimage = reader->GetOutput();

            io::itk::move_from_itk<image_t>(itkimage, *_image);
        };

    const std::filesystem::path file = get_file();
    SIGHT_THROW_EXCEPTION_IF(
        core::exception("File: " + file.string() + " doesn't exist"),
        !std::filesystem::exists(file)
    );
    auto object_lock = get_object();

    const core::type type = get_image_type(file.string());

    using sight::core::tools::dispatcher;
    using sight::core::tools::intrinsic_types;
    dispatcher<intrinsic_types, decltype(do_read)>::invoke(type, this->get_concrete_object(), file.string(), _progress);

    // Post Condition image with a pixel type
    SIGHT_ASSERT(
        "Image has an unspecified type",
        get_concrete_object()->type() != core::type::NONE
    );
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk
