/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/* ************************************
 * This work is inspired by :
 * CamOdoCal: Automatic Intrinsic and Extrinsic Calibration of a Rig with
 * Multiple Generic Cameras and Odometry
 * Copyright (c) 2013 Lionel Heng
 ************************************** */

#ifndef __HANDEYECALIBRATION_CAMODOCAL_GPL_H__
#define __HANDEYECALIBRATION_CAMODOCAL_GPL_H__

#include <handEyeCalibration/config.hpp>

#include <fwCore/spyLog.hpp>

#include <algorithm>
#define _USE_MATH_DEFINES
#include <cstdint>
#include <math.h>

#include <opencv2/core/core.hpp>

namespace camodocal
{

//------------------------------------------------------------------------------

template<class T>
const T clamp(const T& v, const T& a, const T& b)
{
    return std::min(b, std::max(a, v));
}

HANDEYECALIBRATION_API double hypot3(const double x, const double y, const double z);
HANDEYECALIBRATION_API float hypot3f(const float x, const float y, const float z);

//------------------------------------------------------------------------------

template<class T>
const T normalizeTheta(const T& theta)
{
    T normTheta = theta;

    while (normTheta < -M_PI)
    {
        normTheta += 2.0 * M_PI;
    }
    while (normTheta > M_PI)
    {
        normTheta -= 2.0 * M_PI;
    }

    return normTheta;
}

HANDEYECALIBRATION_API double d2r(const double deg);
HANDEYECALIBRATION_API float d2r(const float deg);
HANDEYECALIBRATION_API double r2d(const double rad);
HANDEYECALIBRATION_API float r2d(const float rad);

HANDEYECALIBRATION_API double sinc(const double theta);

//------------------------------------------------------------------------------

template<class T>
const T square(const T& x)
{
    return x * x;
}

//------------------------------------------------------------------------------

template<class T>
const T cube(const T& x)
{
    return x * x * x;
}

//------------------------------------------------------------------------------

template<class T>
const T random(const T& a, const T& b)
{
    return static_cast<double>(rand()) / RAND_MAX * (b - a) + a;
}

//------------------------------------------------------------------------------

template<class T>
const T randomNormal(const T& sigma)
{
    T x1, x2, w;

    do
    {
        x1 = 2.0 * random(0.0, 1.0) - 1.0;
        x2 = 2.0 * random(0.0, 1.0) - 1.0;
        w  = x1 * x1 + x2 * x2;
    }
    while (w >= 1.0 || w == 0.0);

    w = sqrt((-2.0 * log(w)) / w);

    return x1 * w * sigma;
}

HANDEYECALIBRATION_API unsigned long long timeInMicroseconds(void);

HANDEYECALIBRATION_API double timeInSeconds(void);

HANDEYECALIBRATION_API std::vector<cv::Point2i> bresLine(int x0, int y0, const int x1, const int y1);
HANDEYECALIBRATION_API std::vector<cv::Point2i> bresCircle(const int x0, const int y0, const int r);

HANDEYECALIBRATION_API void fitCircle(const std::vector<cv::Point2d>& points,
                                      double& centerX, double& centerY, double& radius);

HANDEYECALIBRATION_API std::vector<cv::Point2d> intersectCircles(const double x1, const double y1, const double r1,
                                                                 const double x2, const double y2, const double r2);

HANDEYECALIBRATION_API long int timestampDiff(const uint64_t t1, const uint64_t t2);

}

#endif //__HANDEYECALIBRATION_CAMODOCAL_GPL_H__
