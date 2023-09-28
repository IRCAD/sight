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

#include <data/Composite.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/TransferFunction.hpp>

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
        data::Image::csptr m_dataImage;
        io::itk::JpgImageWriter::sptr m_fwWriter;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(const Parameter& param)
    {
        SIGHT_DEBUG("itk::ImageSeriesWriter with PIXELTYPE " << core::type::get<PIXELTYPE>().name());

        data::Image::csptr image = param.m_dataImage;

        // Reader IO (*1*)
        auto imageIOWrite = ::itk::ImageIOFactory::CreateImageIO("image.jpg", ::itk::ImageIOFactory::WriteMode);
        assert(imageIOWrite.IsNotNull());

        // create writer
        using itkImageType = ::itk::Image<PIXELTYPE, 3>;
        using Image2DType  = ::itk::Image<unsigned char, 2>;
        using WriterType   = typename ::itk::ImageSeriesWriter<itkImageType, Image2DType>;
        typename WriterType::Pointer writer = WriterType::New();

        // set observation (*2*)
        ::itk::LightProcessObject::Pointer castHelper = (::itk::LightProcessObject*) (imageIOWrite.GetPointer());
        assert(castHelper.IsNotNull());
        Progressor progress(castHelper, param.m_fwWriter, param.m_directoryPath);

        // create itk Image
        typename itkImageType::Pointer itkImage = io::itk::moveToItk<itkImageType>(image);

        using RescaleFilterType = ::itk::IntensityWindowingImageFilter<itkImageType, itkImageType>;
        typename RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();

        double min = NAN;
        double max = NAN;
        data::helper::MedicalImage::getMinMax(image, min, max);

        rescaleFilter->SetWindowMinimum(PIXELTYPE(min));
        rescaleFilter->SetWindowMaximum(PIXELTYPE(max));
        rescaleFilter->SetOutputMinimum(PIXELTYPE(0));
        rescaleFilter->SetOutputMaximum(std::numeric_limits<PIXELTYPE>::max());
        rescaleFilter->InPlaceOff();
        rescaleFilter->SetInput(itkImage);
        rescaleFilter->Update();

        writer->SetInput(rescaleFilter->GetOutput());

        using NameGeneratorType = ::itk::NumericSeriesFileNames;

        NameGeneratorType::Pointer nameGenerator = NameGeneratorType::New();

        std::string format = param.m_directoryPath;
        format += "/%04d.jpg";
        nameGenerator->SetSeriesFormat(format.c_str());
        nameGenerator->SetStartIndex(1);
        nameGenerator->SetEndIndex(image->size()[2]);
        nameGenerator->SetIncrementIndex(1);

        writer->SetFileNames(nameGenerator->GetFileNames());

        writer->SetImageIO(imageIOWrite);

        // save image;
        writer->Update();
    }
};

//------------------------------------------------------------------------------

void JpgImageWriter::write()
{
    assert(!m_object.expired());
    assert(m_object.lock());

    JpgITKSaverFunctor::Parameter saverParam;
    saverParam.m_directoryPath = this->get_folder().string();
    saverParam.m_dataImage     = this->getConcreteObject();
    saverParam.m_fwWriter      = this->get_sptr();
    assert(saverParam.m_dataImage);

    core::tools::dispatcher<core::tools::supported_dispatcher_types, JpgITKSaverFunctor>::invoke(
        saverParam.m_dataImage->getType(),
        saverParam
    );
}

//------------------------------------------------------------------------------

std::string JpgImageWriter::extension() const
{
    return ".jpg";
}

} // namespace sight::io::itk
