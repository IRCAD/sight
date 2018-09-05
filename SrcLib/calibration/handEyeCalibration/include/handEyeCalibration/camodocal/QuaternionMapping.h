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

#ifndef __HANDEYECALIBRATION_CAMODOCAL_QUATERNIONMAPPING_H__
#define __HANDEYECALIBRATION_CAMODOCAL_QUATERNIONMAPPING_H__

#include "handEyeCalibration/camodocal/DualQuaternion.h"
#include "handEyeCalibration/config.hpp"

#include <Eigen/Dense>

#include <iostream>

namespace camodocal
{

//------------------------------------------------------------------------------

template<typename T>
Eigen::Quaternion<T> expq(const Eigen::Quaternion<T>& q)
{
    T a     = q.vec().norm();
    T exp_w = exp(q.w());

    if (a == T(0))
    {
        return Eigen::Quaternion<T>(exp_w, 0, 0, 0);
    }

    Eigen::Quaternion<T> res;
    res.w()   = exp_w * T(cos(a));
    res.vec() = exp_w * T(sinc(a)) * q.vec();

    return res;
}

//------------------------------------------------------------------------------

template<typename T>
Eigen::Quaternion<T> logq(const Eigen::Quaternion<T>& q)
{
    T exp_w = q.norm();
    T w     = log(exp_w);
    T a     = acos(q.w() / exp_w);

    if (a == T(0))
    {
        return Eigen::Quaternion<T>(w, T(0), T(0), T(0));
    }

    Eigen::Quaternion<T> res;
    res.w()   = w;
    res.vec() = q.vec() / exp_w / (sin(a) / a);

    return res;
}

}

#endif // __HANDEYECALIBRATION_CAMODOCAL_QUATERNIONMAPPING_H__
