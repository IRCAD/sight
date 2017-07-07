/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ITKREGISTRATIONOP_AUTOMATICREGISTRATION_HPP__
#define __ITKREGISTRATIONOP_AUTOMATICREGISTRATION_HPP__

#include "itkRegistrationOp/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

namespace itkRegistrationOp
{

/**
 * @brief Static class for automatic image registration.
 */
class ITKREGISTRATIONOP_CLASS_API AutomaticRegistration
{
public:

    /**
     * @brief The different metrics used to evaluate the transform during the optimization stage.
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

    /**
     * @brief find a rigid transform matching the reference image with the target image.
     * @param[in] _target target, i.e. the static image.
     * @param[in] _reference reference, i.e. the image that will be transformed into the target.
     * @param[out] _trf the resulting rigid transform.
     * @param[in] _metric the metric to be used for registration.
     */
    static ITKREGISTRATIONOP_API void registerImage(const ::fwData::Image::csptr& _target,
                                                    const ::fwData::Image::csptr& _reference,
                                                    const ::fwData::TransformationMatrix3D::sptr& _trf,
                                                    MetricType _metric,
                                                    double _minStep = 0.0001,
                                                    double _maxStep = 0.2,
                                                    unsigned long _maxIterations = 200);
};

} // itkRegistrationOp

#endif // __ITKREGISTRATIONOP_AUTOMATICREGISTRATION_HPP__
