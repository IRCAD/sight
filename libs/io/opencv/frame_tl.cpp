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

#include "frame_tl.hpp"

#include "io/opencv/type.hpp"

#include <data/array.hpp>

namespace sight::io::opencv
{

//------------------------------------------------------------------------------

static void to_cv(
    const data::frame_tl::csptr& _timeline,
    const data::frame_tl::buffer_t::element_t* _buffer,
    cv::Mat& _cv_image,
    bool _copy
)
{
    const auto image_type = _timeline->getType();
    const auto image_comp = _timeline->numComponents();

    const auto cv_type = io::opencv::type::toCv(image_type, image_comp);

    cv::Size cv_size(static_cast<int>(_timeline->getWidth()), static_cast<int>(_timeline->getHeight()));

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto* buffer = static_cast<void*>(const_cast<data::frame_tl::buffer_t::element_t*>(_buffer));
    if(_copy)
    {
        cv::Mat mat = cv::Mat(cv_size, cv_type, buffer);
        _cv_image = mat.clone();
    }
    else
    {
        _cv_image = cv::Mat(cv_size, cv_type, buffer);
    }
}

//------------------------------------------------------------------------------

void frame_tl::move_to_cv(
    const data::frame_tl::csptr& _timeline,
    data::frame_tl::buffer_t::element_t* _buffer,
    cv::Mat& _cv_image
)
{
    to_cv(_timeline, _buffer, _cv_image, false);
}

//------------------------------------------------------------------------------

cv::Mat frame_tl::move_to_cv(
    const data::frame_tl::csptr& _timeline,
    const data::frame_tl::buffer_t::element_t* _buffer
)
{
    cv::Mat mat;
    to_cv(_timeline, _buffer, mat, false);
    return mat;
}

//------------------------------------------------------------------------------

void frame_tl::copy_from_cv(
    const data::frame_tl::csptr& _timeline,
    data::frame_tl::buffer_t::element_t* _buffer,
    const cv::Mat& _cv_image
)
{
    const auto prev_image_type = _timeline->getType();
    const auto prev_image_comp = _timeline->numComponents();

    const auto image_format = io::opencv::type::fromCv(_cv_image.type());
    const auto image_type   = image_format.first;
    const auto image_comp   = image_format.second;
    SIGHT_ASSERT("Number of components should be between 1 and 4", image_comp >= 1 && image_comp <= 4);

    std::vector<std::size_t> cv_image_size;
    for(int i = _cv_image.dims - 1 ; i >= 0 ; --i)
    {
        cv_image_size.push_back(static_cast<std::size_t>(_cv_image.size[i]));
    }

    const std::vector<std::size_t> image_size = {{_timeline->getWidth(), _timeline->getHeight()}};

    if(prev_image_comp != image_comp || prev_image_type != image_type || cv_image_size != image_size)
    {
        SIGHT_ERROR("Cannot copy OpenCV image into this timeline buffer because their format or size differ.");
    }

    const std::size_t size = _timeline->getWidth() * _timeline->getHeight() * image_comp * image_type.size();
    std::copy(_cv_image.data, _cv_image.data + size, _buffer);
}

//------------------------------------------------------------------------------

void frame_tl::copyToCv(
    const data::frame_tl::csptr& _timeline,
    const data::frame_tl::buffer_t::element_t* _buffer,
    cv::Mat& _cv_image
)
{
    to_cv(_timeline, _buffer, _cv_image, true);
}

//------------------------------------------------------------------------------

} //namespace sight::io::opencv
