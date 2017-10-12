/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "cvIO/FrameTL.hpp"

#include "cvIO/Type.hpp"

#include <fwData/Array.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/ArrayGetter.hpp>
#include <fwDataTools/helper/Image.hpp>

namespace cvIO
{

//------------------------------------------------------------------------------

static void toCv(const ::arData::FrameTL::csptr& _timeline, const ::arData::FrameTL::BufferType::ElementType* _buffer,
                 cv::Mat& _cvImage, bool _copy)
{
    const auto imageType = _timeline->getType();
    const auto imageComp = _timeline->getNumberOfComponents();

    const auto cvType = ::cvIO::Type::toCv(imageType, imageComp);

    ::cv::Size cvSize(static_cast<int>(_timeline->getWidth()), static_cast<int>(_timeline->getHeight()));

    auto buffer = static_cast<void*>(const_cast< ::arData::FrameTL::BufferType::ElementType*>(_buffer));
    if(_copy)
    {
        ::cv::Mat mat = ::cv::Mat(cvSize, cvType, buffer, ::cv::Mat::AUTO_STEP);
        _cvImage      = mat.clone();
    }
    else
    {
        _cvImage = ::cv::Mat(cvSize, cvType, buffer, ::cv::Mat::AUTO_STEP);
    }
}

//------------------------------------------------------------------------------

void FrameTL::moveToCv(const ::arData::FrameTL::csptr& _timeline,
                       ::arData::FrameTL::BufferType::ElementType* _buffer,
                       ::cv::Mat& _cvImage)
{
    toCv(_timeline, _buffer, _cvImage, false);
}

//------------------------------------------------------------------------------

void FrameTL::copyFromCv(const ::arData::FrameTL::csptr& _timeline,
                         ::arData::FrameTL::BufferType::ElementType* _buffer,
                         const ::cv::Mat& _cvImage)
{
    const auto prevImageType = _timeline->getType();
    const auto prevImageComp = _timeline->getNumberOfComponents();

    const auto imageFormat = ::cvIO::Type::fromCv(_cvImage.type());
    const auto imageType   = imageFormat.first;
    const auto imageComp   = imageFormat.second;
    SLM_ASSERT("Number of components should be between 1 and 4", imageComp >= 1 && imageComp <= 4);

    std::vector<size_t> cvImageSize;
    for(int i = _cvImage.dims - 1; i >= 0; --i)
    {
        cvImageSize.push_back(static_cast<size_t>(_cvImage.size[i]));
    }

    if(_cvImage.dims == 2 && _cvImage.rows == 1)
    {
        cvImageSize.erase(cvImageSize.begin() + 1);
    }
    const std::vector<size_t> imageSize = {{ _timeline->getWidth(), _timeline->getHeight() }};

    if(prevImageComp != imageComp || prevImageType != imageType || cvImageSize != imageSize)
    {
        SLM_ERROR("Cannot copy OpenCV image into this timeline buffer because their format or size differ.");
    }

    const size_t size = _timeline->getWidth() * _timeline->getHeight() * imageComp * imageType.sizeOf();
    std::copy(_cvImage.data, _cvImage.data+size, _buffer);
}

//------------------------------------------------------------------------------

void FrameTL::copyToCv(const ::arData::FrameTL::csptr& _timeline,
                       const ::arData::FrameTL::BufferType::ElementType* _buffer,
                       ::cv::Mat& _cvImage)
{
    toCv(_timeline, _buffer, _cvImage, true);
}

//------------------------------------------------------------------------------

}//namespace cvIO
