/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "itkRegistrationOp/AutomaticRegistration.hpp"

#include "itkRegistrationOp/RegistrationObserver.hxx"

#include <fwDataTools/TransformationMatrix3D.hpp>

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

#include <itkCenteredTransformInitializer.h>
#include <itkCommand.h>
#include <itkGradientDescentOptimizer.h>
#include <itkImage.h>
#include <itkImageRegistrationMethod.h>
#include <itkImageToImageMetric.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkMeanReciprocalSquareDifferenceImageToImageMetric.h>
#include <itkMeanSquaresImageToImageMetric.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkNormalizedCorrelationImageToImageMetric.h>
#include <itkVersorRigid3DTransform.h>
#include <itkVersorRigid3DTransformOptimizer.h>

namespace itkRegistrationOp
{

//------------------------------------------------------------------------------

struct RegistratorParameters
{
    ::fwData::Image::csptr i_target;
    ::fwData::Image::csptr i_reference;
    ::fwData::TransformationMatrix3D::sptr o_trf;
    MetricType i_metric;
    double i_minStep;
    double i_maxStep;
    unsigned long i_maxIters;
};

//------------------------------------------------------------------------------

template<typename TARGET_PIXELTYPE>
struct Registrator
{
    //------------------------------------------------------------------------------

    template<class REFERENCE_PIXELTYPE>
    void operator()(RegistratorParameters& params)
    {
        const unsigned int Dimension = 3;
        typedef itk::Image< TARGET_PIXELTYPE, Dimension >  TargetImageType;
        typedef itk::Image< REFERENCE_PIXELTYPE, Dimension >  ReferenceImageType;

        typedef ::itk::VersorRigid3DTransform< double > TransformType;
        typedef ::itk::VersorRigid3DTransformOptimizer OptimizerType;
        typedef ::itk::LinearInterpolateImageFunction< ReferenceImageType, double > InterpolatorType;
        typedef ::itk::ImageRegistrationMethod< TargetImageType, ReferenceImageType > RegistrationType;

        const typename TargetImageType::Pointer targetImage =
            ::fwItkIO::itkImageFactory< TargetImageType >(params.i_target);
        const typename ReferenceImageType::Pointer referenceImage =
            ::fwItkIO::itkImageFactory< ReferenceImageType >(params.i_reference);

        typename RegistrationType::Pointer registration = RegistrationType::New();
        typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
        OptimizerType::Pointer optimizer = OptimizerType::New();

        typename ::itk::ImageToImageMetric< TargetImageType, ReferenceImageType >::Pointer metric;

        switch(params.i_metric)
        {
            case MEAN_SQUARES:
                metric = ::itk::MeanSquaresImageToImageMetric< TargetImageType, ReferenceImageType >::New();
                break;
            case NORMALIZED_CORRELATION:
                metric = ::itk::NormalizedCorrelationImageToImageMetric< TargetImageType, ReferenceImageType >::New();
                break;
            case PATTERN_INTENSITY:
                metric =
                    ::itk::MeanReciprocalSquareDifferenceImageToImageMetric< TargetImageType,
                                                                             ReferenceImageType >::New();
                break;
            case MUTUAL_INFORMATION:
                auto mutInfo =
                    ::itk::MattesMutualInformationImageToImageMetric< TargetImageType, ReferenceImageType >::New();
                mutInfo->SetNumberOfSpatialSamples(targetImage->GetLargestPossibleRegion().GetNumberOfPixels() * 0.5);
                mutInfo->SetNumberOfHistogramBins(3000);

                metric = mutInfo;
        }

        registration->SetMetric(metric);
        registration->SetOptimizer(optimizer.GetPointer());
        registration->SetInterpolator(interpolator);

        // Initialize transform.
        TransformType::Pointer itkTransform = TransformType::New();

        typedef itk::CenteredTransformInitializer< TransformType, TargetImageType, ReferenceImageType >
            TransformInitializerType;

        typename TransformInitializerType::Pointer initializer = TransformInitializerType::New();

        initializer->SetTransform( itkTransform );
        initializer->SetFixedImage( targetImage );
        initializer->SetMovingImage(referenceImage );

        initializer->MomentsOn();
        initializer->InitializeTransform();

        // Extract rotation and translation from the f4s matrix.
        const ::glm::dmat4 initMat = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(params.o_trf);
        const double scale         = std::pow(::glm::determinant(initMat), 1./3.);

        const ::glm::dquat orientation = ::glm::toQuat(initMat / scale);

        const ::glm::dvec3 translation = ::glm::dvec3(::glm::column(initMat, 3));
        const ::glm::dvec3 axis        = ::glm::axis(orientation);

        typedef TransformType::VersorType VersorType;
        typedef VersorType::VectorType VectorType;
        VersorType rotation;
        VectorType itkAxis;
        itkAxis[0] = axis.x;
        itkAxis[1] = axis.y;
        itkAxis[2] = axis.z;

        VectorType itkTrans;
        itkTrans[0] = translation.x;
        itkTrans[1] = translation.y;
        itkTrans[2] = translation.z;

        rotation.Set(itkAxis, ::glm::angle(orientation));
        itkTransform->SetRotation( rotation );
        itkTransform->SetTranslation(itkTrans);

        registration->SetTransform(itkTransform);
        registration->SetFixedImage(targetImage);
        registration->SetMovingImage(referenceImage);
        registration->SetFixedImageRegion(targetImage->GetLargestPossibleRegion());
        registration->SetInitialTransformParameters(itkTransform->GetParameters());

        // Scale the step size for each parameter.
        OptimizerType::ScalesType optimizerScales( static_cast<unsigned int>(itkTransform->GetNumberOfParameters()) );
        const double translationScale = 1.0 / 1000.0;
        optimizerScales[0] = 1.0;
        optimizerScales[1] = 1.0;
        optimizerScales[2] = 1.0;
        optimizerScales[3] = translationScale;
        optimizerScales[4] = translationScale;
        optimizerScales[5] = translationScale;
        optimizer->SetScales( optimizerScales );
        optimizer->SetMaximumStepLength( params.i_maxStep  );
        optimizer->SetMinimumStepLength( params.i_minStep );
        optimizer->SetNumberOfIterations( params.i_maxIters );

        RegistrationObserver<OptimizerType>::Pointer observer = RegistrationObserver<OptimizerType>::New();
        observer->setMaxIterations( params.i_maxIters );
        optimizer->AddObserver( itk::IterationEvent(), observer );

        registration->Update();

        // Set the new transform if the registration wasn't canceled by the user.
        if(!observer->forceStopped())
        {
            auto parameters = registration->GetLastTransformParameters();

            itkTransform->SetParameters(parameters);

            TransformType::Pointer finalTransform = TransformType::New();
            finalTransform->SetCenter( itkTransform->GetCenter() );
            finalTransform->SetParameters( parameters );
            finalTransform->SetFixedParameters( itkTransform->GetFixedParameters() );

            const ::itk::Matrix<double, 3, 3> rigidMat = finalTransform->GetMatrix();
            const ::itk::Vector<double, 3> offset      = finalTransform->GetOffset();

            // Convert ::itk::RigidTransform to f4s matrix.
            for(std::uint8_t i = 0; i < 3; ++i)
            {
                params.o_trf->setCoefficient(i, 3, offset[i]);
                for(std::uint8_t j = 0; j < 3; ++j)
                {
                    params.o_trf->setCoefficient(i, j, rigidMat(i, j));
                }
            }
        }
    }
};

//------------------------------------------------------------------------------

struct RegistratorCaller
{
    //------------------------------------------------------------------------------

    template<class TARGET_PIXELTYPE>
    void operator() (RegistratorParameters& params)
    {
        const ::fwTools::DynamicType refType = params.i_reference->getPixelType();

        ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, Registrator<TARGET_PIXELTYPE> >::invoke(refType, params);
    }
};

//------------------------------------------------------------------------------

void AutomaticRegistration::registerImage(const ::fwData::Image::csptr& _target,
                                          const ::fwData::Image::csptr& _reference,
                                          const ::fwData::TransformationMatrix3D::sptr& _trf,
                                          MetricType _metric,
                                          double _minStep,
                                          double _maxStep,
                                          unsigned long _maxIterations)
{
    RegistratorParameters params;
    params.i_target    = _target;
    params.i_reference = _reference;
    params.o_trf       = _trf;
    params.i_metric    = _metric;
    params.i_minStep   = _minStep;
    params.i_maxStep   = _maxStep;
    params.i_maxIters  = _maxIterations;

    // We don't handle int64, uint64 or double images to reduce compile times.
    typedef ::boost::mpl::vector<
            signed char, unsigned char,
            signed short, unsigned short,
            signed int, unsigned int, float > RegistrationType;

    const ::fwTools::DynamicType targetType = _target->getPixelType();
    ::fwTools::Dispatcher< RegistrationType, RegistratorCaller>::invoke(targetType, params);
}

//------------------------------------------------------------------------------

} // itkRegistrationOp
