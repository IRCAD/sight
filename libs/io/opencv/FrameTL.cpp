/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include "FrameTL.hpp"

#include "io/opencv/Type.hpp"

#include <data/Array.hpp>

namespace sight::io::opencv
{

//------------------------------------------------------------------------------

static void toCv(
    const data::FrameTL::csptr& _timeline,
    const data::FrameTL::BufferType::ElementType* _buffer,
    cv::Mat& _cvImage,
    bool _copy
)
{
    const auto imageType = _timeline->getType();
    const auto imageComp = _timeline->numComponents();

    const auto cvType = io::opencv::Type::toCv(imageType, imageComp);

    cv::Size cvSize(static_cast<int>(_timeline->getWidth()), static_cast<int>(_timeline->getHeight()));

    auto buffer = static_cast<void*>(const_cast<data::FrameTL::BufferType::ElementType*>(_buffer));
    if(_copy)
    {
        cv::Mat mat = cv::Mat(cvSize, cvType, buffer);
        _cvImage = mat.clone();
    }
    else
    {
        _cvImage = cv::Mat(cvSize, cvType, buffer);
    }
}

//------------------------------------------------------------------------------

void FrameTL::moveToCv(
    const data::FrameTL::csptr& _timeline,
    data::FrameTL::BufferType::ElementType* _buffer,
    cv::Mat& _cvImage
)
{
    toCv(_timeline, _buffer, _cvImage, false);
}

//------------------------------------------------------------------------------

const cv::Mat FrameTL::moveToCv(
    const data::FrameTL::csptr& _timeline,
    const data::FrameTL::BufferType::ElementType* _buffer
)
{
    cv::Mat mat;
    toCv(_timeline, _buffer, mat, false);
    return mat;
}

//------------------------------------------------------------------------------

void FrameTL::copyFromCv(
    const data::FrameTL::csptr& _timeline,
    data::FrameTL::BufferType::ElementType* _buffer,
    const cv::Mat& _cvImage
)
{
    const auto prevImageType = _timeline->getType();
    const auto prevImageComp = _timeline->numComponents();

    const auto imageFormat = io::opencv::Type::fromCv(_cvImage.type());
    const auto imageType   = imageFormat.first;
    const auto imageComp   = imageFormat.second;
    SIGHT_ASSERT("Number of components should be between 1 and 4", imageComp >= 1 && imageComp <= 4);

    std::vector<size_t> cvImageSize;
    for(int i = _cvImage.dims - 1 ; i >= 0 ; --i)
    {
        cvImageSize.push_back(static_cast<size_t>(_cvImage.size[i]));
    }

    const std::vector<size_t> imageSize = {{_timeline->getWidth(), _timeline->getHeight()}};

    if(prevImageComp != imageComp || prevImageType != imageType || cvImageSize != imageSize)
    {
        SIGHT_ERROR("Cannot copy OpenCV image into this timeline buffer because their format or size differ.");
    }

    const size_t size = _timeline->getWidth() * _timeline->getHeight() * imageComp * imageType.sizeOf();
    std::copy(_cvImage.data, _cvImage.data + size, _buffer);
}

//------------------------------------------------------------------------------

void FrameTL::copyToCv(
    const data::FrameTL::csptr& _timeline,
    const data::FrameTL::BufferType::ElementType* _buffer,
    cv::Mat& _cvImage
)
{
    toCv(_timeline, _buffer, _cvImage, true);
}

//------------------------------------------------------------------------------

} //namespace sight::io::opencv
