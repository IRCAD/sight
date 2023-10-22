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

#include "io/itk/JpgImageWriter.hpp"

#include "io/itk/helper/ProgressItkToFw.hpp"
#include "io/itk/itk.hpp"

#include <core/base.hpp>
#include <core/tools/dispatcher.hpp>

#include <data/composite.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/transfer_function.hpp>

#include <io/__/writer/registry/macros.hpp>

#include <itkImageSeriesWriter.h>
#include <itkIntensityWindowingImageFilter.h>
#include <itkJPEGImageIOFactory.h>
#include <itkNumericSeriesFileNames.h>

#include <cmath>
#include <filesystem>

SIGHT_REGISTER_IO_WRITER(sight::io::itk::JpgImageWriter);

namespace sight::io::itk
{

//------------------------------------------------------------------------------

struct JpgITKSaverFunctor
{
    JpgITKSaverFunctor()
    {
        // force register/link_with JPEGImageIOFactory
        ::itk::JPEGImageIOFactory::RegisterOneFactory();
    }

    struct Parameter
    {
        std::string m_directoryPath;
        data::image::csptr m_dataImage;
        io::itk::JpgImageWriter::sptr m_fwWriter;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(const Parameter& _param)
    {
        SIGHT_DEBUG("itk::image_series_writer with PIXELTYPE " << core::type::get<PIXELTYPE>().name());

        data::image::csptr image = _param.m_dataImage;

        // Reader IO (*1*)
        auto image_io_write = ::itk::ImageIOFactory::CreateImageIO("image.jpg", ::itk::ImageIOFactory::WriteMode);
        assert(image_io_write.IsNotNull());

        // create writer
        using itkImageType = ::itk::Image<PIXELTYPE, 3>;
        using Image2DType  = ::itk::Image<unsigned char, 2>;
        using writer_t     = typename ::itk::ImageSeriesWriter<itkImageType, Image2DType>;
        typename writer_t::Pointer writer = writer_t::New();

        // set observation (*2*)
        ::itk::LightProcessObject::Pointer cast_helper = (::itk::LightProcessObject*) (image_io_write.GetPointer());
        assert(cast_helper.IsNotNull());
        Progressor progress(cast_helper, _param.m_fwWriter, _param.m_directoryPath);

        // create itk Image
        typename itkImageType::Pointer itk_image = io::itk::move_to_itk<itkImageType>(image);

        using rescale_filter_t = ::itk::IntensityWindowingImageFilter<itkImageType, itkImageType>;
        typename rescale_filter_t::Pointer rescale_filter = rescale_filter_t::New();

        double min = NAN;
        double max = NAN;
        data::helper::medical_image::get_min_max(image, min, max);

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

        std::string format = _param.m_directoryPath;
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

void JpgImageWriter::write()
{
    assert(!m_object.expired());
    assert(m_object.lock());

    JpgITKSaverFunctor::Parameter saver_param;
    saver_param.m_directoryPath = this->get_folder().string();
    saver_param.m_dataImage     = this->getConcreteObject();
    saver_param.m_fwWriter      = this->get_sptr();
    assert(saver_param.m_dataImage);

    core::tools::dispatcher<core::tools::supported_dispatcher_types, JpgITKSaverFunctor>::invoke(
        saver_param.m_dataImage->getType(),
        saver_param
    );
}

//------------------------------------------------------------------------------

std::string JpgImageWriter::extension() const
{
    return ".jpg";
}

} // namespace sight::io::itk
