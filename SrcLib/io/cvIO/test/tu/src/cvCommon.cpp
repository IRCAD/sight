/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "cvCommon.hpp"

#include <opencv2/core.hpp>

namespace cvIO
{
namespace ut
{

//------------------------------------------------------------------------------

#define declareCvFormatValue(_TYPE, _CVTYPE1, _CVTYPE2, _CVTYPE3, _CVTYPE4 ) \
    const std::array< std::int32_t, 4 > getCvFormat<_TYPE>::type = {{ _CVTYPE1, _CVTYPE2, _CVTYPE3, _CVTYPE4 } \
    };

declareCvFormatValue(std::uint8_t, CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4)
declareCvFormatValue(std::int8_t, CV_8SC1, CV_8SC2, CV_8SC3, CV_8SC4)
declareCvFormatValue(std::uint16_t, CV_16UC1, CV_16UC2, CV_16UC3, CV_16UC4)
declareCvFormatValue(std::int16_t, CV_16SC1, CV_16SC2, CV_16SC3, CV_16SC4)
declareCvFormatValue(std::int32_t, CV_32SC1, CV_32SC2, CV_32SC3, CV_32SC4)
declareCvFormatValue(float, CV_32FC1, CV_32FC2, CV_32FC3, CV_32FC4)
declareCvFormatValue(double, CV_64FC1, CV_64FC2, CV_64FC3, CV_64FC4)

//------------------------------------------------------------------------------

} // namespace ut

}// namespace cvIO
