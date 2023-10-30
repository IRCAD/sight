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

#include "io/itk/InrImageReader.hpp"

#include "factory/InrImageIOFactory.hpp"

#include "io/itk/helper/ProgressItkToFw.hpp"
#include "io/itk/itk.hpp"

#include <core/base.hpp>
#include <core/tools/dispatcher.hpp>

#include <data/image.hpp>

#include <io/__/reader/registry/macros.hpp>

#include <itkImageFileReader.h>
#include <itkImageIOFactory.h>

#include <filesystem>

SIGHT_REGISTER_IO_READER(sight::io::itk::inr_image_reader);

namespace sight::io::itk
{

//------------------------------------------------------------------------------

struct inr_loader_functor
{
    struct parameter
    {
        data::image::sptr m_data_image;
        std::string m_filename;
        io::itk::inr_image_reader::sptr m_fw_reader;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(parameter& _param)
    {
        SIGHT_INFO(
            "::io::itk::InrImageReader::InrLoaderFunctor with PIXELTYPE "
            << core::type::get<PIXELTYPE>().name()
        );

        // Reader IO (*1*)
        typename ::itk::ImageIOBase::Pointer image_io_read = ::itk::ImageIOFactory::CreateImageIO(
            _param.m_filename.c_str(),
            ::itk::ImageIOFactory::ReadMode
        );

        // set observation (*2*)
        progressor progress(image_io_read, _param.m_fw_reader, _param.m_filename);

        // the reader
        using image_t  = ::itk::Image<PIXELTYPE, 3>;
        using reader_t = ::itk::ImageFileReader<image_t>;
        typename reader_t::Pointer reader = reader_t::New();
        reader->SetFileName(_param.m_filename);

        // attach its IO (*3*)
        reader->SetImageIO(image_io_read);

        reader->Update();
        typename image_t::Pointer itkimage = reader->GetOutput();
        io::itk::move_from_itk<image_t>(itkimage, _param.m_data_image);
    }

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
};

//------------------------------------------------------------------------------

void inr_image_reader::read()
{
    std::filesystem::path file = get_file();
    SIGHT_ASSERT("File: " << file << " doesn't exist", std::filesystem::exists(file));
    assert(!m_object.expired());
    assert(m_object.lock());

    const core::type type = inr_loader_functor::get_image_type(file.string());

    inr_loader_functor::parameter param;
    param.m_filename   = file.string();
    param.m_data_image = this->get_concrete_object();
    param.m_fw_reader  = this->get_sptr();

    core::tools::dispatcher<core::tools::intrinsic_types, inr_loader_functor>::invoke(type, param);

    SIGHT_ASSERT("sight::data::image is not well produced", m_object.lock()); // verify that data::image is well
    // produced
    // Post Condition image with a pixel type
    SIGHT_ASSERT(
        "Image has an unspecified type",
        get_concrete_object()->type() != core::type::NONE
    );
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk
