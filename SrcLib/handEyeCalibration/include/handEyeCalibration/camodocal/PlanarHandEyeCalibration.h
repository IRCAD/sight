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

#ifndef __HANDEYECALIBRATION_CAMODOCAL_PLANARHANDEYECALIBRATION_H__
#define __HANDEYECALIBRATION_CAMODOCAL_PLANARHANDEYECALIBRATION_H__

#include "handEyeCalibration/config.hpp"

#include <Eigen/Eigen>
#include <Eigen/StdVector>

namespace camodocal
{

class HANDEYECALIBRATION_CLASS_API PlanarHandEyeCalibration
{
public:
    HANDEYECALIBRATION_API PlanarHandEyeCalibration();

    HANDEYECALIBRATION_API bool addMotions(const std::vector<Eigen::Matrix4d,
                                                             Eigen::aligned_allocator<Eigen::Matrix4d> >& H1,
                                           const std::vector<Eigen::Matrix4d,
                                                             Eigen::aligned_allocator<Eigen::Matrix4d> >& H2);

    HANDEYECALIBRATION_API size_t getMotionCount(void) const;

    HANDEYECALIBRATION_API bool calibrate(Eigen::Matrix4d& H_12);

    HANDEYECALIBRATION_API bool getVerbose(void);
    HANDEYECALIBRATION_API void setVerbose(bool on = false);

    HANDEYECALIBRATION_API bool estimate(const std::vector<Eigen::Quaterniond,
                                                           Eigen::aligned_allocator<Eigen::Quaterniond> >& quats1,
                                         const std::vector<Eigen::Vector3d,
                                                           Eigen::aligned_allocator<Eigen::Vector3d> >& tvecs1,
                                         const std::vector<Eigen::Quaterniond,
                                                           Eigen::aligned_allocator<Eigen::Quaterniond> >& quats2,
                                         const std::vector<Eigen::Vector3d,
                                                           Eigen::aligned_allocator<Eigen::Vector3d> >& tvecs2,
                                         Eigen::Matrix4d& H_12) const;

private:
    bool estimateRyx(const std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond> >& quats1,
                     const std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> >& tvecs1,
                     const std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond> >& quats2,
                     const std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> >& tvecs2,
                     Eigen::Matrix3d& R_yx) const;

    void refineEstimate(Eigen::Matrix4d& H_12,
                        const std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond> >& quats1,
                        const std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> >& tvecs1,
                        const std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond> >& quats2,
                        const std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> >& tvecs2) const;

    // solve ax^2 + bx + c = 0
    bool solveQuadraticEquation(double a, double b, double c, double& x1, double& x2) const;

    typedef struct
    {
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        Eigen::Quaterniond rotation;
        Eigen::Vector3d translation;
    } Motion;

    std::vector<Motion, Eigen::aligned_allocator<Motion> > m_motions1;
    std::vector<Motion, Eigen::aligned_allocator<Motion> > m_motions2;

    bool m_verbose;
};

}

#endif // __HANDEYECALIBRATION_CAMODOCAL_PLANARHANDEYECALIBRATION_H__
