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

#include <data/Image.hpp>

#include <io/__/reader/registry/macros.hpp>

#include <itkImageFileReader.h>
#include <itkImageIOFactory.h>

#include <filesystem>

SIGHT_REGISTER_IO_READER(sight::io::itk::InrImageReader);

namespace sight::io::itk
{

//------------------------------------------------------------------------------

struct InrLoaderFunctor
{
    struct Parameter
    {
        data::Image::sptr m_dataImage;
        std::string m_filename;
        io::itk::InrImageReader::sptr m_fwReader;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameter& param)
    {
        SIGHT_INFO(
            "::io::itk::InrImageReader::InrLoaderFunctor with PIXELTYPE "
            << core::type::get<PIXELTYPE>().name()
        );

        // Reader IO (*1*)
        typename ::itk::ImageIOBase::Pointer imageIORead = ::itk::ImageIOFactory::CreateImageIO(
            param.m_filename.c_str(),
            ::itk::ImageIOFactory::ReadMode
        );

        // set observation (*2*)
        Progressor progress(imageIORead, param.m_fwReader, param.m_filename);

        // the reader
        using ImageType  = ::itk::Image<PIXELTYPE, 3>;
        using ReaderType = ::itk::ImageFileReader<ImageType>;
        typename ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName(param.m_filename);

        // attach its IO (*3*)
        reader->SetImageIO(imageIORead);

        reader->Update();
        typename ImageType::Pointer itkimage = reader->GetOutput();
        io::itk::moveFromItk<ImageType>(itkimage, param.m_dataImage);
    }

    //// get pixel type from Header
    static const core::type& getImageType(const std::string& imageFileName)
    {
        ::itk::ImageIOBase::Pointer imageIO = ::itk::ImageIOFactory::CreateImageIO(
            imageFileName.c_str(),
            ::itk::ImageIOFactory::ReadMode
        );

        if(imageIO == nullptr)
        {
            const std::string errMsg = "no ImageIOFactory found to read header of file : " + imageFileName;
            throw(std::ios_base::failure(errMsg));
        }

        imageIO->SetFileName(imageFileName.c_str());
        imageIO->ReadImageInformation();
        auto type = imageIO->GetComponentType();

        return sight::io::itk::ITK_TYPE_CONVERTER.at(type);
    }
};

//------------------------------------------------------------------------------

void InrImageReader::read()
{
    std::filesystem::path file = get_file();
    SIGHT_ASSERT("File: " << file << " doesn't exist", std::filesystem::exists(file));
    assert(!m_object.expired());
    assert(m_object.lock());

    const core::type type = InrLoaderFunctor::getImageType(file.string());

    InrLoaderFunctor::Parameter param;
    param.m_filename  = file.string();
    param.m_dataImage = this->getConcreteObject();
    param.m_fwReader  = this->get_sptr();

    core::tools::dispatcher<core::tools::intrinsic_types, InrLoaderFunctor>::invoke(type, param);

    SIGHT_ASSERT("sight::data::Image is not well produced", m_object.lock()); // verify that data::Image is well
    // produced
    // Post Condition image with a pixel type
    SIGHT_ASSERT(
        "Image has an unspecified type",
        getConcreteObject()->getType() != core::type::NONE
    );
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk
