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

#include "io/itk/jpg_image_writer.hpp"

#include "io/itk/helper/progress_itk_to_fw.hpp"
#include "io/itk/itk.hpp"

#include <core/tools/dispatcher.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <itkImageSeriesWriter.h>
#include <itkIntensityWindowingImageFilter.h>
#include <itkJPEGImageIOFactory.h>
#include <itkNumericSeriesFileNames.h>

#include <cmath>
#include <filesystem>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

namespace
{

struct itk_jpeg_registry_initializer
{
    itk_jpeg_registry_initializer()
    {
        // force register/link_with JPEGImageIOFactory
        ::itk::JPEGImageIOFactory::RegisterOneFactory();
    }
};

itk_jpeg_registry_initializer global_itk_jpeg_registry_initializer;

} // namespace

//------------------------------------------------------------------------------

void jpg_image_writer::write(sight::core::progress::observer::sptr _progress)
{
    auto object_lock = get_object();

    auto do_read =
        []<class PIXELTYPE>( const data::image::csptr _image,
                             const std::filesystem::path& _directory_path,
                             sight::core::progress::observer::sptr _progress)
        {
            SIGHT_DEBUG("itk::image_series_writer with PIXELTYPE " << core::type::get<PIXELTYPE>().name());

            const data::image::csptr& image = _image;

            // Reader IO (*1*)
            auto image_io_write = ::itk::ImageIOFactory::CreateImageIO("image.jpg", ::itk::ImageIOFactory::WriteMode);
            assert(image_io_write.IsNotNull());

            // create writer
            using itk_image_type = ::itk::Image<PIXELTYPE, 3>;
            using image_2d_type  = ::itk::Image<unsigned char, 2>;
            using writer_t       = ::itk::ImageSeriesWriter<itk_image_type, image_2d_type>;
            auto writer = writer_t::New();
            progressor progress(writer, _progress);

            // create itk Image
            auto itk_image = io::itk::move_to_itk<itk_image_type>(image);

            using rescale_filter_t = ::itk::IntensityWindowingImageFilter<itk_image_type, itk_image_type>;
            typename rescale_filter_t::Pointer rescale_filter = rescale_filter_t::New();

            const auto& [min, max] = data::helper::medical_image::get_min_max<double>(image);

            rescale_filter->SetWindowMinimum(PIXELTYPE(min));
            rescale_filter->SetWindowMaximum(PIXELTYPE(max));
            rescale_filter->SetOutputMinimum(PIXELTYPE(0));
            rescale_filter->SetOutputMaximum(std::numeric_limits<PIXELTYPE>::max());
            rescale_filter->InPlaceOff();
            rescale_filter->SetInput(itk_image);
            rescale_filter->Update();

            writer->SetInput(rescale_filter->GetOutput());

            using name_generator_t = ::itk::NumericSeriesFileNames;

            name_generator_t::Pointer name_generator = name_generator_t::New();

            std::string format = _directory_path.string();
            format += "/%04d.jpg";
            name_generator->SetSeriesFormat(format.c_str());
            name_generator->SetStartIndex(1);
            name_generator->SetEndIndex(image->size()[2]);
            name_generator->SetIncrementIndex(1);

            writer->SetFileNames(name_generator->GetFileNames());

            writer->SetImageIO(image_io_write);

            // save image;
            writer->Update();
        };

    auto directory_path = this->get_folder();
    auto image          = this->get_concrete_object();

    using sight::core::tools::dispatcher;
    using sight::core::tools::intrinsic_types;
    dispatcher<intrinsic_types, decltype(do_read)>::invoke(image->type(), image, directory_path.string(), _progress);
}

//------------------------------------------------------------------------------

std::string jpg_image_writer::extension() const
{
    return ".jpg";
}

} // namespace sight::io::itk
