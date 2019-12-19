/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "igtlProtocol/converter/ImageConverter.hpp"

#include "igtlProtocol/DataConverter.hpp"
#include "igtlProtocol/ImageTypeConverter.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtlImageMessage.h>

#include <algorithm>

namespace igtlProtocol
{
namespace converter
{
const std::string ImageConverter::s_IGTL_TYPE          = "IMAGE";
const std::string ImageConverter::s_FWDATA_OBJECT_TYPE = ::fwData::Image::classname();

converterRegisterMacro(::igtlProtocol::converter::ImageConverter);

ImageConverter::ImageConverter()
{
}

//-----------------------------------------------------------------------------

ImageConverter::~ImageConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer ImageConverter::fromFwDataObject(::fwData::Object::csptr src) const
{
    ::fwData::Image::csptr srcImg = ::fwData::Image::dynamicCast(src);
    ::igtl::Matrix4x4 matrix;

    const auto dumpLock = srcImg->lock();
    auto itr            = srcImg->begin();
    auto itrEnd         = srcImg->end();

    ::igtl::ImageMessage::Pointer dest = ::igtl::ImageMessage::New();
    ::igtl::IdentityMatrix(matrix);
    dest->SetMatrix(matrix);
    dest->SetScalarType(ImageTypeConverter::getIgtlType(srcImg->getType()));
    dest->SetCoordinateSystem(igtl::ImageMessage::COORDINATE_LPS);
    dest->SetOrigin(srcImg->getOrigin2()[0], srcImg->getOrigin2()[1], srcImg->getOrigin2()[2]);
    dest->SetSpacing(srcImg->getSpacing2()[0], srcImg->getSpacing2()[1], srcImg->getSpacing2()[2]);
    dest->SetNumComponents(static_cast<int>(srcImg->getNumberOfComponents()));
    dest->SetDimensions(static_cast<int>(srcImg->getSize2()[0]),
                        static_cast<int>(srcImg->getSize2()[1]),
                        static_cast<int>(srcImg->getSize2()[2]));
    dest->AllocateScalars();
    char*  igtlImgBuffer = reinterpret_cast<char*>(dest->GetScalarPointer());
    std::copy(itr, itrEnd, igtlImgBuffer);
    return ::igtl::MessageBase::Pointer(dest.GetPointer());
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr ImageConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    ::igtl::ImageMessage::Pointer srcImg;
    char* imgBuffer;
    char* igtlImageBuffer;
    ::fwData::Image::sptr destImg = ::fwData::Image::New();
    ::fwDataTools::helper::Image imgHelper(destImg);
    float igtlSpacing[3];
    float igtlOrigins[3];
    int igtlDimensions[3];
    ::fwData::Image::Spacing spacing;
    ::fwData::Image::Origin origins;
    ::fwData::Image::Size size;

    srcImg = ::igtl::ImageMessage::Pointer(dynamic_cast< ::igtl::ImageMessage* >(src.GetPointer()));
    srcImg->GetSpacing(igtlSpacing);
    srcImg->GetOrigin(igtlOrigins);
    srcImg->GetDimensions(igtlDimensions);
    std::transform(igtlSpacing, igtlSpacing + 3, spacing.begin(), ::boost::numeric_cast<double, float>);
    std::copy(igtlDimensions, igtlDimensions + 3, size.begin());
    std::transform(igtlOrigins, igtlOrigins + 3, origins.begin(), ::boost::numeric_cast<double, float>);
    destImg->setOrigin2(origins);
    destImg->setSpacing2(spacing);
    destImg->setSize2(size);
    destImg->setType(ImageTypeConverter::getFwToolsType(srcImg->GetScalarType()));
    destImg->setNumberOfComponents(srcImg->GetNumComponents());
    if (srcImg->GetNumComponents() == 1)
    {
        destImg->setPixelFormat(::fwData::Image::GRAY_SCALE);
    }
    else if (srcImg->GetNumComponents() == 3)
    {
        destImg->setPixelFormat(::fwData::Image::RGB);
    }
    else if (srcImg->GetNumComponents() == 4)
    {
        destImg->setPixelFormat(::fwData::Image::RGBA);
    }

    destImg->resize();
    imgBuffer       = reinterpret_cast<char*>(imgHelper.getBuffer());
    igtlImageBuffer = reinterpret_cast<char*>(srcImg->GetScalarPointer());
    std::copy(igtlImageBuffer, igtlImageBuffer + srcImg->GetImageSize(), imgBuffer);

    return destImg;
}

//-----------------------------------------------------------------------------

IConverter::sptr ImageConverter::New()
{
    return std::make_shared< ImageConverter >();
}

//-----------------------------------------------------------------------------

std::string const& ImageConverter::getIgtlType() const
{
    return ImageConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& ImageConverter::getFwDataObjectType() const
{
    return ImageConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter
} // namespace igtlProtocol}
