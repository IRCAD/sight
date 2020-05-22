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
