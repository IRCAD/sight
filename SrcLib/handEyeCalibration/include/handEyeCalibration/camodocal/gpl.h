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

#include "handEyeCalibration/config.hpp"

#include <fwCore/spyLog.hpp>

#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cstdint>

#include <opencv2/core/core.hpp>

namespace camodocal
{

//------------------------------------------------------------------------------

template<class T>
const T clamp(const T& v, const T& a, const T& b)
{
    return std::min(b, std::max(a, v));
}

HANDEYECALIBRATION_API double hypot3(double x, double y, double z);
HANDEYECALIBRATION_API float hypot3f(float x, float y, float z);

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

HANDEYECALIBRATION_API double d2r(double deg);
HANDEYECALIBRATION_API float d2r(float deg);
HANDEYECALIBRATION_API double r2d(double rad);
HANDEYECALIBRATION_API float r2d(float rad);

HANDEYECALIBRATION_API double sinc(double theta);

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

HANDEYECALIBRATION_API void colorDepthImage(cv::Mat& imgDepth,
                                            cv::Mat& imgColoredDepth,
                                            float minRange, float maxRange);

HANDEYECALIBRATION_API bool colormap(const std::string& name, unsigned char idx,
                                     float& r, float& g, float& b);

HANDEYECALIBRATION_API std::vector<cv::Point2i> bresLine(int x0, int y0, int x1, int y1);
HANDEYECALIBRATION_API std::vector<cv::Point2i> bresCircle(int x0, int y0, int r);

HANDEYECALIBRATION_API void fitCircle(const std::vector<cv::Point2d>& points,
                                      double& centerX, double& centerY, double& radius);

HANDEYECALIBRATION_API std::vector<cv::Point2d> intersectCircles(double x1, double y1, double r1,
                                                                 double x2, double y2, double r2);

HANDEYECALIBRATION_API void LLtoUTM(double latitude, double longitude,
                                    double& utmNorthing, double& utmEasting,
                                    std::string& utmZone);
HANDEYECALIBRATION_API void UTMtoLL(double utmNorthing, double utmEasting,
                                    const std::string& utmZone,
                                    double& latitude, double& longitude);

HANDEYECALIBRATION_API long int timestampDiff(uint64_t t1, uint64_t t2);

}

#endif //__HANDEYECALIBRATION_CAMODOCAL_GPL_H__
