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

#include "io/itk/inr_image_reader.hpp"

#include <core/notification/observer.hpp>

#include "io/itk/helper/progress_itk_to_fw.hpp"
#include "io/itk/itk.hpp"

#include <core/tools/dispatcher.hpp>

#include <data/image.hpp>

#include <itkImageFileReader.h>
#include <itkImageIOFactory.h>

#include <filesystem>

namespace sight::io::itk
{

//// get pixel type from Header
static const core::type& get_image_type(const std::string& _image_file_name)
{
    ::itk::ImageIOBase::Pointer image_io = ::itk::ImageIOFactory::CreateImageIO(
        _image_file_name.c_str(),
        ::itk::ImageIOFactory::ReadMode
    );

    if(image_io == nullptr)
    {
        const std::string err_msg = "no ImageIOFactory found to read header of file : " + _image_file_name;
        throw(std::ios_base::failure(err_msg));
    }

    image_io->SetFileName(_image_file_name.c_str());
    image_io->ReadImageInformation();
    auto type = image_io->GetComponentType();

    return sight::io::itk::ITK_TYPE_CONVERTER.at(type);
}

//------------------------------------------------------------------------------

void inr_image_reader::read(sight::core::notification::observer::sptr _progress)
{
    auto do_read =
        []<class PIXEL_TYPE>(
            data::image::sptr _data_image,
            std::string _filename,
            core::notification::observer::sptr _observer
    )
        {
            SIGHT_INFO(
                "::io::itk::InrImageReader::InrLoaderFunctor with PIXELTYPE "
                << core::type::get<PIXEL_TYPE>().name()
            );

            // Reader IO (*1*)
            ::itk::ImageIOBase::Pointer image_io_read = ::itk::ImageIOFactory::CreateImageIO(
                _filename.c_str(),
                ::itk::ImageIOFactory::ReadMode
            );

            // the reader
            using image_t  = ::itk::Image<PIXEL_TYPE, 3>;
            using reader_t = ::itk::ImageFileReader<image_t>;
            typename reader_t::Pointer reader = reader_t::New();
            progressor progress(reader, _observer);
            reader->SetFileName(_filename);

            // attach its IO (*3*)
            reader->SetImageIO(image_io_read);

            reader->Update();
            typename image_t::Pointer itkimage = reader->GetOutput();
            io::itk::move_from_itk<image_t>(itkimage, *_data_image);
        };

    const std::filesystem::path file = get_file();
    SIGHT_ASSERT("File: " << file << " doesn't exist", std::filesystem::exists(file));
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
