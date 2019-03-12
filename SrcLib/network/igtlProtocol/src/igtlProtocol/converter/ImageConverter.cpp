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
    ::fwData::Image::csptr srcImg = ::fwData::Image::dynamicConstCast(src);
    ::igtl::Matrix4x4 matrix;
    char* igtlImgBuffer;
    ::fwDataTools::helper::Array arrayHelper(srcImg->getDataArray());

    ::igtl::ImageMessage::Pointer dest = ::igtl::ImageMessage::New();
    ::igtl::IdentityMatrix(matrix);
    dest->SetMatrix(matrix);
    dest->SetScalarType(ImageTypeConverter::getIgtlType(srcImg->getType()));
    dest->SetCoordinateSystem(igtl::ImageMessage::COORDINATE_LPS);
    dest->SetOrigin(srcImg->getOrigin()[0], srcImg->getOrigin()[1], srcImg->getOrigin()[2]);
    dest->SetSpacing(srcImg->getSpacing()[0], srcImg->getSpacing()[1], srcImg->getSpacing()[2]);
    dest->SetNumComponents(static_cast<int>(srcImg->getNumberOfComponents()));
    dest->SetDimensions(static_cast<int>(srcImg->getSize()[0]),
                        static_cast<int>(srcImg->getSize()[1]),
                        static_cast<int>(srcImg->getSize()[2]));
    dest->AllocateScalars();
    igtlImgBuffer = reinterpret_cast<char*>(dest->GetScalarPointer());
    std::copy(arrayHelper.begin(), arrayHelper.end(), igtlImgBuffer);
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
    std::vector<double> spacing;
    std::vector<double> origins;
    ::fwData::Image::SizeType size;

    srcImg = ::igtl::ImageMessage::Pointer(dynamic_cast< ::igtl::ImageMessage* >(src.GetPointer()));
    spacing.resize(3);
    origins.resize(3);
    srcImg->GetSpacing(igtlSpacing);
    srcImg->GetOrigin(igtlOrigins);
    srcImg->GetDimensions(igtlDimensions);
    std::transform(igtlSpacing, igtlSpacing + 3, spacing.begin(), ::boost::numeric_cast<double, float>);
    std::copy(igtlDimensions, igtlDimensions + 3, std::back_inserter< ::fwData::Image::SizeType> (size));
    std::transform(igtlOrigins, igtlOrigins + 3, origins.begin(), ::boost::numeric_cast<double, float>);
    destImg->setOrigin(origins);
    destImg->setSpacing(spacing);
    destImg->setSize(size);
    destImg->setType(ImageTypeConverter::getFwToolsType(srcImg->GetScalarType()));
    destImg->setNumberOfComponents(srcImg->GetNumComponents());
    destImg->allocate();
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
