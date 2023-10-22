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

static cv::Mat to_cv(const data::image::csptr& _image, bool _copy)
{
    const auto image_type = _image->getType();
    const auto image_comp = _image->numComponents();

    const auto cv_type = io::opencv::type::toCv(image_type, image_comp);

    const auto dump_lock = _image->dump_lock();

    SIGHT_ASSERT("Empty image buffer", _image->buffer());

    const auto image_size = _image->size();
    std::vector<int> cv_size;
    for(std::size_t i = 0 ; i < _image->numDimensions() ; ++i)
    {
        cv_size.push_back(static_cast<int>(image_size[i]));
    }

    if(cv_size.size() == 1)
    {
        // If we have a single row, we want to initialize the cv::Math with (1, N) since it takes (rows,cols)
        cv_size.push_back(1);
    }

    // Reverse from (w,h,d) to (d,h,w) because OpenCV uses a row major format
    std::reverse(cv_size.begin(), cv_size.end());

    cv::Mat cv_image;
    if(_copy)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        cv::Mat mat = cv::Mat(cv_size, cv_type, const_cast<void*>(_image->buffer()));
        cv_image = mat.clone();
    }
    else
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        cv_image = cv::Mat(cv_size, cv_type, const_cast<void*>(_image->buffer()));
    }

    return cv_image;
}

//------------------------------------------------------------------------------

cv::Mat image::move_to_cv(data::image::sptr& _image)
{
    return to_cv(_image, false);
}

//------------------------------------------------------------------------------

cv::Mat image::move_to_cv(const data::image::csptr& _image)
{
    return to_cv(_image, false);
}

//------------------------------------------------------------------------------

void image::copy_from_cv(data::image& _image, const cv::Mat& _cv_image)
{
    const auto prev_image_type = _image.getType();
    const auto prev_image_comp = _image.numComponents();

    const auto image_format = io::opencv::type::fromCv(_cv_image.type());
    const auto image_type   = image_format.first;
    const auto image_comp   = image_format.second;
    SIGHT_ASSERT("Number of components should be between 1 and 4", image_comp >= 1 && image_comp <= 4);
    SIGHT_ASSERT("Number of dimension should be between 1 and 3", _cv_image.dims >= 1 && _cv_image.dims <= 3);

    data::image::Size image_size = {0, 0, 0};

    if(_cv_image.dims == 1)
    {
        image_size[0] = std::size_t(_cv_image.size[0]);
    }
    else if(_cv_image.dims == 2 && _cv_image.rows == 1)
    {
        // This means this is actually a 1D image so remove the first dimension (==1)
        image_size[0] = std::size_t(_cv_image.size[1]);
        image_size[1] = 0;
    }
    else if(_cv_image.dims == 2)
    {
        image_size[0] = std::size_t(_cv_image.size[1]);
        image_size[1] = std::size_t(_cv_image.size[0]);
    }
    else // 3D
    {
        image_size[0] = std::size_t(_cv_image.size[2]);
        image_size[1] = std::size_t(_cv_image.size[1]);
        image_size[2] = std::size_t(_cv_image.size[0]);
    }

    const auto prev_image_size = _image.size();
    if(prev_image_comp != image_comp || prev_image_type != image_type || image_size != prev_image_size)
    {
        data::image::PixelFormat format = data::image::PixelFormat::GRAY_SCALE;
        switch(image_comp)
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

        _image.resize(image_size, image_type, format);
    }

    const auto dump_lock = _image.dump_lock();
    SIGHT_ASSERT("Empty image buffer", _image.getAllocatedSizeInBytes() > 0);

    auto buffer = _image.begin<std::uint8_t>();
    std::copy(_cv_image.data, _cv_image.data + _image.getSizeInBytes(), buffer);
}

//------------------------------------------------------------------------------

cv::Mat image::copyToCv(const data::image::csptr& _image)
{
    return to_cv(_image, true);
}

//------------------------------------------------------------------------------

} //namespace sight::io::opencv
