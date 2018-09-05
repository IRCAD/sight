/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATACAMP_AUTOLOAD_HPP__
#define __ARDATACAMP_AUTOLOAD_HPP__

#include "arDataCamp/Camera.hpp"
#include "arDataCamp/CameraSeries.hpp"
#include "arDataCamp/config.hpp"

#include <fwCamp/macros.hpp>

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>
#include <arData/CalibrationInfo.hpp>

namespace arDataCamp
{

struct runner
{
    runner()
    {
        localDeclarearDataCameraSourceType();
        localDeclarearDataCameraPixelFormat();
        localDeclarearDataCamera();
        localDeclarearDataCameraSeries();
        localDeclarearDataCalibrationInfo();
    }

    static runner r;
};

} //end namespace arDataCamp

#endif //__ARDATACAMP_AUTOLOAD_HPP__
