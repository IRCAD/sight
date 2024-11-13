/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <core/base.hpp>
#include <core/tools/dispatcher.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/map.hpp>
#include <data/transfer_function.hpp>

#include <itkImageSeriesWriter.h>
#include <itkIntensityWindowingImageFilter.h>
#include <itkJPEGImageIOFactory.h>
#include <itkNumericSeriesFileNames.h>

#include <cmath>
#include <filesystem>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

struct jpg_itk_saver_functor
{
    jpg_itk_saver_functor()
    {
        // force register/link_with JPEGImageIOFactory
        ::itk::JPEGImageIOFactory::RegisterOneFactory();
    }

    struct parameter
    {
        std::string m_directory_path;
        data::image::csptr m_data_image;
        io::itk::jpg_image_writer::sptr m_fw_writer;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(const parameter& _param)
    {
        SIGHT_DEBUG("itk::image_series_writer with PIXELTYPE " << core::type::get<PIXELTYPE>().name());

        data::image::csptr image = _param.m_data_image;

        // Reader IO (*1*)
        auto image_io_write = ::itk::ImageIOFactory::CreateImageIO("image.jpg", ::itk::ImageIOFactory::WriteMode);
        assert(image_io_write.IsNotNull());

        // create writer
        using itk_image_type = ::itk::Image<PIXELTYPE, 3>;
        using image_2d_type  = ::itk::Image<unsigned char, 2>;
        using writer_t       = typename ::itk::ImageSeriesWriter<itk_image_type, image_2d_type>;
        auto writer = writer_t::New();

        // set observation (*2*)
        ::itk::LightProcessObject::Pointer cast_helper = (::itk::LightProcessObject*) (image_io_write.GetPointer());
        assert(cast_helper.IsNotNull());
        progressor progress(cast_helper, _param.m_fw_writer, _param.m_directory_path);

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

        std::string format = _param.m_directory_path;
        format += "/%04d.jpg";
        name_generator->SetSeriesFormat(format.c_str());
        name_generator->SetStartIndex(1);
        name_generator->SetEndIndex(image->size()[2]);
        name_generator->SetIncrementIndex(1);

        writer->SetFileNames(name_generator->GetFileNames());

        writer->SetImageIO(image_io_write);

        // save image;
        writer->Update();
    }
};

//------------------------------------------------------------------------------

void jpg_image_writer::write()
{
    assert(!m_object.expired());
    assert(m_object.lock());

    jpg_itk_saver_functor::parameter saver_param;
    saver_param.m_directory_path = this->get_folder().string();
    saver_param.m_data_image     = this->get_concrete_object();
    saver_param.m_fw_writer      = this->get_sptr();
    assert(saver_param.m_data_image);

    core::tools::dispatcher<core::tools::supported_dispatcher_types, jpg_itk_saver_functor>::invoke(
        saver_param.m_data_image->type(),
        saver_param
    );
}

//------------------------------------------------------------------------------

std::string jpg_image_writer::extension() const
{
    return ".jpg";
}

} // namespace sight::io::itk
