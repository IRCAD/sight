/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_UNCERTAINTYPROPAG_H
#define _ARLCORE_UNCERTAINTYPROPAG_H
#include <arlcore/Common.h>

#include <vector>
#include <string>

#include <arlcore/Camera.h>
#include <arlcore/Reconst3D.h>

//#define DEFAULT_MODEL "synthe.CKeys1"
//#define DEFAULT_SCENE "synthe.CKeys2"

namespace arlCore
{
    /**
     * @brief Compute the covariance of a 3D point which is reconstructed from several (at least 2) video
     * image. Obviously, we consider that cameras are jointly calibrated. After computation, the covariance
     * matrix of the 3D Point is updated. Do not forget that you MUST fill the covariance matrix of each
     * 2D Point in list2D.
     *
     * Up to now, covariance computation is available for ARLCORE_R3D_REPROJECTION_OPTIMIZATION,
     * ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY and ARLCORE_R3D_TWO_LINES_APPROX only.
     *
     * @param[in] list2D[i] : 2D detection of the 3D point in the i-th camera
     * @param[in] camsList[i] : camera parameters of the i-th camera
     * @param[in] point3D : 3D coordinates of the reconstructed point
     * @param[in] methode : method used to recontruct the point in 3D (see Reconst3D.h)
     * @param[out] log : still not used
     * @param[in] pixelFrame : still not used
     *
     */
    ARLCORE_API bool propagateCovarianceToReconst3D(const std::vector<arlCore::Point::csptr>&list2D, const std::vector<const arlCore::Camera*> &camsList,
         arlCore::Point &point3D, arlCore::ARLCORE_RECONSTRUCTION3D methode, std::vector<double> &log, const bool pixelFrame);

    ARLCORE_API void WriteTableau ( char *nom, std::vector<double> tab, unsigned int n );
    /**
     * @brief Khi Square Validation for 3 dimensional data
     *
     */
    ARLCORE_API double* KSValidation3D(char *index_file);
    ARLCORE_API double CumulativeChi2_3(double mu2);

    /**
     * @brief Probabilistic tests and densities
     *
     */
    ARLCORE_API double ProbKSTest(double alam); /* Kolmogorov-Smirnov probability */
    ARLCORE_API double LnGamma(double xx); /* ln(Gamma(a,x)) */
    ARLCORE_API double IncompleteGammaP(double a, double x);
    ARLCORE_API double CumulativeChi2(double mu2, int n);

    ARLCORE_API double IncompleteBetaI(register double a, register double b,register  double x);
    ARLCORE_API double CumulativeFisher(double a, double b, double x);
    ARLCORE_API double CumulativeHotelling(double p, double n, double x);

    ARLCORE_API double SimpleKSTest( double *data, int n, double (*pdf)(double),
                    double *d);/* Simple Kolmogorov-Smirnov test */
    ARLCORE_API double DoubleKSTest( double *data1, int n1, double *data2, int n2,
                double *d);/* DoubleKolmogorov-Smirnov test */
} // namespace arlCore
#endif //_ARLCORE_UNCERTAINTYPROPAG_H
