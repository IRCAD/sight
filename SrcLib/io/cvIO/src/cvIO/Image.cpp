/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "cvIO/Image.hpp"

#include <fwData/Array.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/ArrayGetter.hpp>
#include <fwDataTools/helper/Image.hpp>

namespace cvIO
{

//------------------------------------------------------------------------------

static void toCv(const fwData::Image::csptr& _image, cv::Mat& _cvImage, bool _copy)
{
    const auto imageType = _image->getType();
    const auto imageComp = _image->getNumberOfComponents();

    SLM_ASSERT("Number of dimensions should be between 1 and 3", imageComp > 0 && imageComp <= 4);

    static std::map< ::fwTools::Type, std::array< std::int32_t, 4 > > s_IMAGE_FORMAT_TO_CV =
    {{
         { ::fwTools::Type::s_UINT8, {{ CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4 }} },
         { ::fwTools::Type::s_INT8, {{ CV_8SC1, CV_8SC2, CV_8SC3, CV_8SC4 }} },
         { ::fwTools::Type::s_UINT16, {{ CV_16UC1, CV_16UC2, CV_16UC3, CV_16UC4 }} },
         { ::fwTools::Type::s_INT16, {{ CV_16SC1, CV_16SC2, CV_16SC3, CV_16SC4 }} },
         { ::fwTools::Type::s_INT32, {{ CV_32SC1, CV_32SC2, CV_32SC3, CV_32SC4 }} },
         { ::fwTools::Type::s_FLOAT, {{ CV_32FC1, CV_32FC2, CV_32FC3, CV_32FC4 }} },
         { ::fwTools::Type::s_DOUBLE, {{ CV_64FC1, CV_64FC2, CV_64FC3, CV_64FC4 }} }
     }};

    const auto cvType = s_IMAGE_FORMAT_TO_CV[imageType][imageComp - 1];

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
        // If we have a single row, we want to initialize the cv::Math with (1, N) since it takes (rows,cols)
        cvSize.push_back(1);
    }
    std::reverse(cvSize.begin(), cvSize.end());

    if(_copy)
    {
        ::cv::Mat mat = ::cv::Mat(cvSize, cvType, const_cast<void*>(arraySrcHelper.getBuffer()));
        _cvImage      = mat.clone();
    }
    else
    {
        _cvImage = ::cv::Mat(cvSize, cvType, const_cast<void*>(arraySrcHelper.getBuffer()));
    }
}

//------------------------------------------------------------------------------

void Image::moveToCv(fwData::Image::sptr& _image, cv::Mat& _cvImage)
{
    toCv(_image, _cvImage, false);
}

//------------------------------------------------------------------------------

void Image::copyFromCv(fwData::Image::sptr& _image, const cv::Mat& _cvImage)
{
    static std::map< std::int32_t, std::pair< ::fwTools::Type, size_t> > s_IMAGE_FORMAT_FROM_CV =
    {{
         { CV_8UC1, { ::fwTools::Type::s_UINT8, 1} },
         { CV_8UC2, { ::fwTools::Type::s_UINT8, 2} },
         { CV_8UC3, { ::fwTools::Type::s_UINT8, 3} },
         { CV_8UC4, { ::fwTools::Type::s_UINT8, 4} },
         { CV_8SC1, { ::fwTools::Type::s_INT8, 1} },
         { CV_8SC2, { ::fwTools::Type::s_INT8, 2} },
         { CV_8SC3, { ::fwTools::Type::s_INT8, 3} },
         { CV_8SC4, { ::fwTools::Type::s_INT8, 4} },
         { CV_16UC1, { ::fwTools::Type::s_UINT16, 1} },
         { CV_16UC2, { ::fwTools::Type::s_UINT16, 2} },
         { CV_16UC3, { ::fwTools::Type::s_UINT16, 3} },
         { CV_16UC4, { ::fwTools::Type::s_UINT16, 4} },
         { CV_16SC1, { ::fwTools::Type::s_INT16, 1} },
         { CV_16SC2, { ::fwTools::Type::s_INT16, 2} },
         { CV_16SC3, { ::fwTools::Type::s_INT16, 3} },
         { CV_16SC4, { ::fwTools::Type::s_INT16, 4} },
         { CV_32SC1, { ::fwTools::Type::s_INT32, 1} },
         { CV_32SC2, { ::fwTools::Type::s_INT32, 2} },
         { CV_32SC3, { ::fwTools::Type::s_INT32, 3} },
         { CV_32SC4, { ::fwTools::Type::s_INT32, 4} },
         { CV_32FC1, { ::fwTools::Type::s_FLOAT, 1} },
         { CV_32FC2, { ::fwTools::Type::s_FLOAT, 2} },
         { CV_32FC3, { ::fwTools::Type::s_FLOAT, 3} },
         { CV_32FC4, { ::fwTools::Type::s_FLOAT, 4} },
         { CV_64FC1, { ::fwTools::Type::s_DOUBLE, 1} },
         { CV_64FC2, { ::fwTools::Type::s_DOUBLE, 2} },
         { CV_64FC3, { ::fwTools::Type::s_DOUBLE, 3} },
         { CV_64FC4, { ::fwTools::Type::s_DOUBLE, 4} }
     }};

    const auto prevImageType = _image->getType();
    const auto prevImageComp = _image->getNumberOfComponents();

    const auto cvType      = _cvImage.type();
    const auto imageFormat = s_IMAGE_FORMAT_FROM_CV[cvType];
    const auto imageType   = imageFormat.first;
    const auto imageComp   = imageFormat.second;
    SLM_ASSERT("Number of components should be between 1 and 4", imageComp >= 1 && imageComp <= 4);

    const auto imageSize = _image->getSize();

    std::vector<size_t> cvImageSize;
    for(int i = _cvImage.dims - 1; i >= 0; --i)
    {
        cvImageSize.push_back(static_cast<size_t>(_cvImage.size[i]));
    }

    if(_cvImage.dims == 2 && _cvImage.rows == 1)
    {
        cvImageSize.erase(cvImageSize.begin() + 1);
    }
    if(prevImageComp != imageComp || prevImageType != imageType || cvImageSize != imageSize)
    {
        _image->allocate(cvImageSize, imageType, imageComp);
    }

    if(prevImageComp != imageComp || prevImageType != imageType || cvImageSize != imageSize)
    {
        _image->allocate(cvImageSize, imageType, imageComp);
    }

    ::fwData::Array::sptr arraySrc = _image->getDataArray();
    ::fwDataTools::helper::Array arraySrcHelper(arraySrc);
    SLM_ASSERT("Empty image buffer", arraySrcHelper.getBuffer());

    std::uint8_t* buffer = arraySrcHelper.begin< std::uint8_t >();
    std::copy(_cvImage.data, _cvImage.data+arraySrc->getSizeInBytes(), buffer);
}

//------------------------------------------------------------------------------

void Image::copyToCv(const fwData::Image::csptr& _image, cv::Mat& _cvImage)
{
    toCv(_image, _cvImage, true);
}

//------------------------------------------------------------------------------

}//namespace cvIO
