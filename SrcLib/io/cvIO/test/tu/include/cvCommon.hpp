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

#ifndef __CVIO_UT_CVCOMMON_HPP__
#define __CVIO_UT_CVCOMMON_HPP__

#include <fwCore/spyLog.hpp>

#include <opencv2/core.hpp>

#include <random>

namespace cvIO
{
namespace ut
{

//------------------------------------------------------------------------------

template <typename T>
struct getCvFormat;

#define declareCvFormat(_TYPE, _CVTYPE1, _CVTYPE2, _CVTYPE3, _CVTYPE4 ) \
    template <> \
    struct getCvFormat<_TYPE> \
    { \
        static const std::array< std::int32_t, 4 > type; \
    };

declareCvFormat(std::uint8_t, CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4)
declareCvFormat(std::int8_t, CV_8SC1, CV_8SC2, CV_8SC3, CV_8SC4)
declareCvFormat(std::uint16_t, CV_16UC1, CV_16UC2, CV_16UC3, CV_16UC4)
declareCvFormat(std::int16_t, CV_16SC1, CV_16SC2, CV_16SC3, CV_16SC4)
declareCvFormat(std::int32_t, CV_32SC1, CV_32SC2, CV_32SC3, CV_32SC4)
declareCvFormat(float, CV_32FC1, CV_32FC2, CV_32FC3, CV_32FC4)
declareCvFormat(double, CV_64FC1, CV_64FC2, CV_64FC3, CV_64FC4)

//------------------------------------------------------------------------------

template <typename T>
::cv::Mat genCvImage(  const std::vector<T>& _imageBuffer, size_t _w, size_t _h, size_t _d,
                       std::uint8_t _numChannels)
{
    SLM_ASSERT("Width should be at least 1", _w >= 1);

    std::vector<int> cvSize;
    if(_d > 0)
    {
        cvSize.push_back( static_cast<int>(_d) );
    }
    if(_h > 0)
    {
        cvSize.push_back( static_cast<int>(_h) );
    }
    else
    {
        cvSize.push_back( static_cast<int>(1) );
    }
    cvSize.push_back( static_cast<int>(_w) );

    const auto cvType = getCvFormat<T>::type[_numChannels - 1];
    ::cv::Mat cvImage = ::cv::Mat(cvSize, cvType, static_cast<void*>(const_cast<T*>(_imageBuffer.data())));

    return cvImage;
}

//------------------------------------------------------------------------------

template <typename T>
static const std::vector<T> genImageBuffer(size_t _w, size_t _h, size_t _d, std::uint8_t _numChannels)
{
    const size_t imageSize = _w * (_h == 0 ? 1 : _h) * (_d == 0 ? 1 : _d) * _numChannels;
    std::vector<T> buffer;
    buffer.resize(imageSize);

    std::random_device rd;
    std::mt19937 engine(rd());

    if(std::is_integral<T>::value)
    {
        std::uniform_int_distribution<> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

        for(size_t i = 0; i < imageSize; ++i)
        {
            auto value   = dist(engine);
            T boundValue = static_cast<T>(value);
            buffer[i] = boundValue;
        }
    }
    else
    {
        std::uniform_real_distribution<> dist(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());

        for(size_t i = 0; i < imageSize; ++i)
        {
            auto value   = dist(engine);
            T boundValue = static_cast<T>(value);
            buffer[i] = boundValue;
        }
    }

    return buffer;
}

//------------------------------------------------------------------------------

} // namespace ut

}// namespace cvIO

#endif //__CVIO_UT_CVCOMMON_HPP__
