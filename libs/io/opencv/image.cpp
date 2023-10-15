/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#include "image.hpp"

#include "io/opencv/type.hpp"

#include <data/array.hpp>

namespace sight::io::opencv
{

//------------------------------------------------------------------------------

static cv::Mat toCv(const data::image::csptr& _image, bool _copy)
{
    const auto imageType = _image->getType();
    const auto imageComp = _image->numComponents();

    const auto cvType = io::opencv::type::toCv(imageType, imageComp);

    const auto dumpLock = _image->dump_lock();

    SIGHT_ASSERT("Empty image buffer", _image->buffer());

    const auto imageSize = _image->size();
    std::vector<int> cvSize;
    for(std::size_t i = 0 ; i < _image->numDimensions() ; ++i)
    {
        cvSize.push_back(static_cast<int>(imageSize[i]));
    }

    if(cvSize.size() == 1)
    {
        // If we have a single row, we want to initialize the cv::Math with (1, N) since it takes (rows,cols)
        cvSize.push_back(1);
    }

    // Reverse from (w,h,d) to (d,h,w) because OpenCV uses a row major format
    std::reverse(cvSize.begin(), cvSize.end());

    cv::Mat cvImage;
    if(_copy)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        cv::Mat mat = cv::Mat(cvSize, cvType, const_cast<void*>(_image->buffer()));
        cvImage = mat.clone();
    }
    else
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        cvImage = cv::Mat(cvSize, cvType, const_cast<void*>(_image->buffer()));
    }

    return cvImage;
}

//------------------------------------------------------------------------------

cv::Mat image::moveToCv(data::image::sptr& _image)
{
    return toCv(_image, false);
}

//------------------------------------------------------------------------------

cv::Mat image::moveToCv(const data::image::csptr& _image)
{
    return toCv(_image, false);
}

//------------------------------------------------------------------------------

void image::copyFromCv(data::image& _image, const cv::Mat& _cvImage)
{
    const auto prevImageType = _image.getType();
    const auto prevImageComp = _image.numComponents();

    const auto imageFormat = io::opencv::type::fromCv(_cvImage.type());
    const auto imageType   = imageFormat.first;
    const auto imageComp   = imageFormat.second;
    SIGHT_ASSERT("Number of components should be between 1 and 4", imageComp >= 1 && imageComp <= 4);
    SIGHT_ASSERT("Number of dimension should be between 1 and 3", _cvImage.dims >= 1 && _cvImage.dims <= 3);

    data::image::Size imageSize = {0, 0, 0};

    if(_cvImage.dims == 1)
    {
        imageSize[0] = std::size_t(_cvImage.size[0]);
    }
    else if(_cvImage.dims == 2 && _cvImage.rows == 1)
    {
        // This means this is actually a 1D image so remove the first dimension (==1)
        imageSize[0] = std::size_t(_cvImage.size[1]);
        imageSize[1] = 0;
    }
    else if(_cvImage.dims == 2)
    {
        imageSize[0] = std::size_t(_cvImage.size[1]);
        imageSize[1] = std::size_t(_cvImage.size[0]);
    }
    else // 3D
    {
        imageSize[0] = std::size_t(_cvImage.size[2]);
        imageSize[1] = std::size_t(_cvImage.size[1]);
        imageSize[2] = std::size_t(_cvImage.size[0]);
    }

    const auto prevImageSize = _image.size();
    if(prevImageComp != imageComp || prevImageType != imageType || imageSize != prevImageSize)
    {
        data::image::PixelFormat format = data::image::PixelFormat::GRAY_SCALE;
        switch(imageComp)
        {
            case 1:
                format = data::image::PixelFormat::GRAY_SCALE;
                break;

            case 2:
                format = data::image::PixelFormat::RG;
                break;

            case 3:
                format = data::image::PixelFormat::RGB;
                break;

            case 4:
                format = data::image::PixelFormat::RGBA;
                break;

            default:
                SIGHT_FATAL("Unhandled OpenCV format");
        }

        _image.resize(imageSize, imageType, format);
    }

    const auto dumpLock = _image.dump_lock();
    SIGHT_ASSERT("Empty image buffer", _image.getAllocatedSizeInBytes() > 0);

    auto buffer = _image.begin<std::uint8_t>();
    std::copy(_cvImage.data, _cvImage.data + _image.getSizeInBytes(), buffer);
}

//------------------------------------------------------------------------------

cv::Mat image::copyToCv(const data::image::csptr& _image)
{
    return toCv(_image, true);
}

//------------------------------------------------------------------------------

} //namespace sight::io::opencv
