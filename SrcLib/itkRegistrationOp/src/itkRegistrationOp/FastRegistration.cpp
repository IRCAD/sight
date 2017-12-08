/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "itkRegistrationOp/FastRegistration.hpp"

#include "itkRegistrationOp/RegistrationObserver.hxx"

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwGui/backend.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwItkIO/helper/Transform.hpp>
#include <fwItkIO/itk.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <itkCastImageFilter.h>
#include <itkCenteredTransformInitializer.h>
#include <itkCorrelationImageToImageMetricv4.h>
#include <itkImage.h>
#include <itkImageRegistrationMethodv4.h>
#include <itkImageToImageMetricv4.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMattesMutualInformationImageToImageMetricv4.h>
#include <itkMeanSquaresImageToImageMetricv4.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkRegularStepGradientDescentOptimizerv4.h>
#include <itkVersorRigid3DTransform.h>

namespace itkRegistrationOp
{

typedef typename ::itk::Image< float, 3 > RegisteredImageType;

//------------------------------------------------------------------------------

void FastRegistration::registerImage(const ::fwData::Image::csptr& _target,
                                     const ::fwData::Image::csptr& _reference,
                                     const ::fwData::TransformationMatrix3D::sptr& _trf,
                                     MetricType _metric,
                                     double _minStep,
                                     unsigned long _maxIterations)
{
    for(std::uint8_t i = 0; i != 4; ++i)
    {
        for(std::uint8_t j = 0; j != 4; ++j)
        {
            _trf->setCoefficient(i, j, 0.);
        }
    }
    for(std::uint8_t i = 0; i != 4; ++i)
    {
        _trf->setCoefficient(i, i, 1.);
    }
}

//------------------------------------------------------------------------------

} // itkRegistrationOp

