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

#include "io/igtl/detail/converter/ImageConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"
#include "io/igtl/detail/ImageTypeConverter.hpp"

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtlImageMessage.h>

#include <algorithm>

namespace sight::io::igtl::detail::converter
{

const std::string ImageConverter::s_IGTL_TYPE          = "IMAGE";
const std::string ImageConverter::s_FWDATA_OBJECT_TYPE = data::image::classname();

CONVERTER_REGISTER_MACRO(io::igtl::detail::converter::ImageConverter);

ImageConverter::ImageConverter()
= default;

//-----------------------------------------------------------------------------

ImageConverter::~ImageConverter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer ImageConverter::fromFwDataObject(data::object::csptr _src) const
{
    data::image::csptr src_img = std::dynamic_pointer_cast<const data::image>(_src);
    ::igtl::Matrix4x4 matrix;

    const auto dump_lock = src_img->dump_lock();
    auto itr             = src_img->begin();
    auto itr_end         = src_img->end();

    ::igtl::ImageMessage::Pointer dest = ::igtl::ImageMessage::New();
    ::igtl::IdentityMatrix(matrix);
    dest->SetMatrix(matrix);
    dest->SetScalarType(ImageTypeConverter::get_igtl_type(src_img->getType()));
    dest->SetCoordinateSystem(::igtl::ImageMessage::COORDINATE_LPS);
    dest->SetOrigin(float(src_img->getOrigin()[0]), float(src_img->getOrigin()[1]), float(src_img->getOrigin()[2]));
    dest->SetSpacing(float(src_img->getSpacing()[0]), float(src_img->getSpacing()[1]), float(src_img->getSpacing()[2]));
    dest->SetNumComponents(static_cast<int>(src_img->numComponents()));
    dest->SetDimensions(
        static_cast<int>(src_img->size()[0]),
        static_cast<int>(src_img->size()[1]),
        static_cast<int>(src_img->size()[2])
    );
    dest->AllocateScalars();
    char* igtl_img_buffer = reinterpret_cast<char*>(dest->GetScalarPointer());
    std::copy(itr, itr_end, igtl_img_buffer);
    return {dest.GetPointer()};
}

//-----------------------------------------------------------------------------

data::object::sptr ImageConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer _src) const
{
    ::igtl::ImageMessage::Pointer src_img;
    char* igtl_image_buffer    = nullptr;
    data::image::sptr dest_img = std::make_shared<data::image>();
    const auto dump_lock       = dest_img->dump_lock();
    std::array<float, 3> igtl_spacing {};
    std::array<float, 3> igtl_origins {};
    std::array<int, 3> igtl_dimensions {};
    data::image::Spacing spacing;
    data::image::Origin origins;
    data::image::Size size;

    src_img = ::igtl::ImageMessage::Pointer(dynamic_cast< ::igtl::ImageMessage*>(_src.GetPointer()));
    src_img->GetSpacing(igtl_spacing.data());
    src_img->GetOrigin(igtl_origins.data());
    src_img->GetDimensions(igtl_dimensions.data());
    std::transform(igtl_spacing.begin(), igtl_spacing.end(), spacing.begin(), boost::numeric_cast<double, float>);
    std::copy(igtl_dimensions.begin(), igtl_dimensions.end(), size.begin());
    std::transform(igtl_origins.begin(), igtl_origins.end(), origins.begin(), boost::numeric_cast<double, float>);
    dest_img->setOrigin(origins);
    dest_img->setSpacing(spacing);

    sight::data::image::PixelFormat format = data::image::PixelFormat::GRAY_SCALE;
    if(src_img->GetNumComponents() == 1)
    {
        format = data::image::PixelFormat::GRAY_SCALE;
    }
    else if(src_img->GetNumComponents() == 3)
    {
        format = data::image::PixelFormat::RGB;
    }
    else if(src_img->GetNumComponents() == 4)
    {
        format = data::image::PixelFormat::RGBA;
    }
    else
    {
        SIGHT_ASSERT("invalid number of components: " + std::to_string(src_img->GetNumComponents()), false);
    }

    dest_img->resize(size, ImageTypeConverter::get_fw_tools_type(std::uint8_t(src_img->GetScalarType())), format);
    auto dest_iter = dest_img->begin();
    igtl_image_buffer = reinterpret_cast<char*>(src_img->GetScalarPointer());
    std::copy(igtl_image_buffer, igtl_image_buffer + src_img->GetImageSize(), dest_iter);

    if(sight::data::helper::medical_image::check_image_validity(dest_img))
    {
        sight::data::helper::medical_image::check_image_slice_index(dest_img);
    }

    return dest_img;
}

//-----------------------------------------------------------------------------

base::sptr ImageConverter::New()
{
    return std::make_shared<ImageConverter>();
}

//-----------------------------------------------------------------------------

std::string const& ImageConverter::get_igtl_type() const
{
    return ImageConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& ImageConverter::getFwDataObjectType() const
{
    return ImageConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace sight::io::igtl::detail::converter
