/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <data/Array.hpp>

namespace cvIO
{

//------------------------------------------------------------------------------

static ::cv::Mat toCv(const data::Image::csptr& _image, bool _copy)
{
    const auto imageType = _image->getType();
    const auto imageComp = _image->getNumberOfComponents();

    const auto cvType = ::cvIO::Type::toCv(imageType, imageComp);

    const auto dumpLock = _image->lock();

    SLM_ASSERT("Empty image buffer", _image->getBuffer());

    const auto imageSize = _image->getSize2();
    std::vector<int> cvSize;
    for(size_t i = 0; i < _image->getNumberOfDimensions(); ++i)
    {
        cvSize.push_back( static_cast<int>(imageSize[i]) );
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
        ::cv::Mat mat = ::cv::Mat(cvSize, cvType, const_cast<void*>(_image->getBuffer()));
        cvImage       = mat.clone();
    }
    else
    {
        cvImage = ::cv::Mat(cvSize, cvType, const_cast<void*>(_image->getBuffer()));
    }
    return cvImage;
}

//------------------------------------------------------------------------------

::cv::Mat Image::moveToCv(data::Image::sptr& _image)
{
    return toCv(_image, false);
}

//------------------------------------------------------------------------------

const ::cv::Mat Image::moveToCv(const data::Image::csptr& _image)
{
    return toCv(_image, false);
}

//------------------------------------------------------------------------------

void Image::copyFromCv(data::Image::sptr& _image, const ::cv::Mat& _cvImage)
{
    const auto prevImageType = _image->getType();
    const auto prevImageComp = _image->getNumberOfComponents();

    const auto imageFormat = ::cvIO::Type::fromCv(_cvImage.type());
    const auto imageType   = imageFormat.first;
    const auto imageComp   = imageFormat.second;
    SLM_ASSERT("Number of components should be between 1 and 4", imageComp >= 1 && imageComp <= 4);
    SLM_ASSERT("Number of dimension should be between 1 and 3", _cvImage.dims >= 1 && _cvImage.dims <= 3);

    data::Image::Size imageSize = {0, 0, 0};

    if (_cvImage.dims == 1)
    {
        imageSize[0] = _cvImage.size[0];
    }
    else if(_cvImage.dims == 2 && _cvImage.rows == 1)
    {
        // This means this is actually a 1D image so remove the first dimension (==1)
        imageSize[0] = _cvImage.size[1];
        imageSize[1] = 0;
    }
    else if (_cvImage.dims == 2)
    {
        imageSize[0] = _cvImage.size[1];
        imageSize[1] = _cvImage.size[0];
    }
    else // 3D
    {
        imageSize[0] = _cvImage.size[2];
        imageSize[1] = _cvImage.size[1];
        imageSize[2] = _cvImage.size[0];
    }

    const auto prevImageSize = _image->getSize2();
    if(prevImageComp != imageComp || prevImageType != imageType || imageSize != prevImageSize)
    {
        // The pixel format is not changed here, we have no way to know the format from a ::cv::Mat
        _image->setSize2(imageSize);
        _image->setType(imageType);
        _image->setNumberOfComponents(imageComp);
        _image->resize();
    }

    const auto dumpLock = _image->lock();
    SLM_ASSERT("Empty image buffer", _image->getAllocatedSizeInBytes() > 0);

    auto buffer = _image->begin< std::uint8_t >();
    std::copy(_cvImage.data, _cvImage.data+_image->getSizeInBytes(), buffer);
}

//------------------------------------------------------------------------------

::cv::Mat Image::copyToCv(const data::Image::csptr& _image)
{
    return toCv(_image, true);
}

//------------------------------------------------------------------------------

}//namespace cvIO
