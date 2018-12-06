/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "cvIO/Image.hpp"

#include "cvIO/Type.hpp"

#include <fwData/Array.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/ArrayGetter.hpp>
#include <fwDataTools/helper/Image.hpp>

namespace cvIO
{

//------------------------------------------------------------------------------

static ::cv::Mat toCv(const ::fwData::Image::csptr& _image, bool _copy)
{
    const auto imageType = _image->getType();
    const auto imageComp = _image->getNumberOfComponents();

    const auto cvType = ::cvIO::Type::toCv(imageType, imageComp);

    const ::fwData::Array::sptr arraySrc = _image->getDataArray();
    const ::fwDataTools::helper::ArrayGetter arraySrcHelper(arraySrc);
    SLM_ASSERT("Empty image buffer", arraySrcHelper.getBuffer());

    const auto imageSize = _image->getSize();
    std::vector<int> cvSize;
    for(size_t i : imageSize)
    {
        cvSize.push_back( static_cast<int>(i) );
    }
    if(cvSize.size() == 1)
    {
        // If we have a single row, we want to initialize the ::cv::Math with (1, N) since it takes (rows,cols)
        cvSize.push_back(1);
    }
    // Reverse from (w,h,d) to (d,h,w) because OpenCV uses a row major format
    std::reverse(cvSize.begin(), cvSize.end());

    ::cv::Mat cvImage;
    if(_copy)
    {
        ::cv::Mat mat = ::cv::Mat(cvSize, cvType, const_cast<void*>(arraySrcHelper.getBuffer()));
        cvImage       = mat.clone();
    }
    else
    {
        cvImage = ::cv::Mat(cvSize, cvType, const_cast<void*>(arraySrcHelper.getBuffer()));
    }
    return cvImage;
}

//------------------------------------------------------------------------------

::cv::Mat Image::moveToCv(::fwData::Image::sptr& _image)
{
    return toCv(_image, false);
}

//------------------------------------------------------------------------------

void Image::copyFromCv(::fwData::Image::sptr& _image, const ::cv::Mat& _cvImage)
{
    const auto prevImageType = _image->getType();
    const auto prevImageComp = _image->getNumberOfComponents();

    const auto imageFormat = ::cvIO::Type::fromCv(_cvImage.type());
    const auto imageType   = imageFormat.first;
    const auto imageComp   = imageFormat.second;
    SLM_ASSERT("Number of components should be between 1 and 4", imageComp >= 1 && imageComp <= 4);

    std::vector<size_t> imageSize;
    for(int i = 0; i < _cvImage.dims; ++i)
    {
        imageSize.push_back(static_cast<size_t>(_cvImage.size[i]));
    }

    if(_cvImage.dims == 2 && _cvImage.rows == 1)
    {
        // This means this is actually a 1D image so remove the first dimension (==1)
        imageSize.erase(imageSize.begin());
    }
    // Reverse from (d,h,w) to  because OpenCV uses a row major format
    std::reverse(imageSize.begin(), imageSize.end());

    const auto prevImageSize = _image->getSize();
    if(prevImageComp != imageComp || prevImageType != imageType || imageSize != prevImageSize)
    {
        _image->allocate(imageSize, imageType, imageComp);
    }

    ::fwData::Array::sptr arraySrc = _image->getDataArray();
    ::fwDataTools::helper::Array arraySrcHelper(arraySrc);
    SLM_ASSERT("Empty image buffer", arraySrcHelper.getBuffer());

    std::uint8_t* buffer = arraySrcHelper.begin< std::uint8_t >();
    std::copy(_cvImage.data, _cvImage.data+arraySrc->getSizeInBytes(), buffer);
}

//------------------------------------------------------------------------------

::cv::Mat Image::copyToCv(const ::fwData::Image::csptr& _image)
{
    return toCv(_image, true);
}

//------------------------------------------------------------------------------

}//namespace cvIO
