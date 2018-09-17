/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ITKREGISTRATIONOP_METRIC_HPP__
#define __ITKREGISTRATIONOP_METRIC_HPP__

namespace itkRegistrationOp
{

/**
 * @brief The different metrics that can be used to evaluate the transform during registration.
 *
 * @see https://itk.org/Doxygen/html/RegistrationPage.html
 *
 * We can choose between four metrics:
 *
 * - Mean squares : works best when the two images have the same intensity range.
 * - Normalized correlation : this metric allows to register objects whose intensity values
 * are related by a linear transformation.
 * - Pattern Intensity : this metric has the advantage of increasing simultaneously when more samples
 * are available and when intensity values are close
 * - Mutual Information : The most generic metric, can be used to match images with different modalities.
 */
typedef enum Metric
{
    MEAN_SQUARES,
    NORMALIZED_CORRELATION,
    PATTERN_INTENSITY,
    MUTUAL_INFORMATION
} MetricType;

} // itkRegistrationOp

#endif // __ITKREGISTRATIONOP_METRIC_HPP__
