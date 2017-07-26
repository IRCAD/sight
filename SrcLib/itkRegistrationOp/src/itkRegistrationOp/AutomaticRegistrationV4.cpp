/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "itkRegistrationOp/AutomaticRegistrationV4.hpp"

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

#include <itkCastImageFilter.h>
#include <itkCenteredTransformInitializer.h>
#include <itkCommand.h>
#include <itkCorrelationImageToImageMetricv4.h>
#include <itkImage.h>
#include <itkImageRegistrationMethodv4.h>
#include <itkImageToImageMetricv4.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMattesMutualInformationImageToImageMetricv4.h>
#include <itkMeanSquaresImageToImageMetricv4.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkRegularStepGradientDescentOptimizerv4.h>
#include <itkTextOutput.h>
#include <itkVersorRigid3DTransform.h>

namespace itkRegistrationOp
{

typedef typename ::itk::Image< float, 3 > RegisteredImageType;

//------------------------------------------------------------------------------

template < typename OUTPUT_PIXELTYPE >
struct ItkImageCaster
{
    typedef typename ::itk::Image<OUTPUT_PIXELTYPE, 3> OutputImageType;

    struct Params
    {
        ::fwData::Image::csptr i_img;
        typename OutputImageType::Pointer o_img;
    };

    //------------------------------------------------------------------------------

    template< typename INPUT_PIXELTYPE >
    void operator()(Params& p)
    {
        typedef typename ::itk::Image<INPUT_PIXELTYPE, 3> InputImageType;

        // Convert to ITK.
        InputImageType::Pointer tmp = ::fwItkIO::itkImageFactory< InputImageType >(p.i_img);

        // Cast to the desired pixel type.
        auto castFilter = ::itk::CastImageFilter<InputImageType, OutputImageType>::New();
        castFilter->SetInput(tmp);
        castFilter->Update();
        p.o_img = castFilter->GetOutput();
    }
};

//------------------------------------------------------------------------------

static RegisteredImageType::Pointer castToFloat(const ::fwData::Image::csptr& _img)
{
    typedef ItkImageCaster<float> FloatCasterType;

    typename FloatCasterType::Params p;
    p.i_img = _img;

    const ::fwTools::DynamicType inType = _img->getPixelType();

    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, FloatCasterType >::invoke(inType, p);

    return p.o_img;
}

//------------------------------------------------------------------------------

void AutomaticRegistrationV4::registerImage(const ::fwData::Image::csptr& _target,
                                            const ::fwData::Image::csptr& _reference,
                                            const ::fwData::TransformationMatrix3D::sptr& _trf,
                                            MetricType _metric,
                                            double _minStep,
                                            unsigned long _maxIterations)
{
    typedef float RealType; // Numeric type used for internal computations.
    typedef typename ::itk::RegularStepGradientDescentOptimizerv4<RealType> OptimizerType;

    typedef typename ::itk::VersorRigid3DTransform< RealType > TransformType;
    typedef typename ::itk::ImageRegistrationMethodv4< RegisteredImageType, RegisteredImageType, TransformType >
        RegistrationMethodType;

    typename ::itk::ImageToImageMetricv4< RegisteredImageType, RegisteredImageType, RegisteredImageType,
                                          RealType >::Pointer metric;

    itk::OutputWindow::SetInstance(
        itk::TextOutput::New().GetPointer());

    // Convert input images to float.
    RegisteredImageType::Pointer target    = castToFloat(_target);
    RegisteredImageType::Pointer reference = castToFloat(_reference);

    // Choose a metric.
    switch(_metric)
    {
        case MEAN_SQUARES:
            metric =
                ::itk::MeanSquaresImageToImageMetricv4< RegisteredImageType, RegisteredImageType, RegisteredImageType,
                                                        RealType >::New();
            break;
        case NORMALIZED_CORRELATION:
            metric =
                ::itk::CorrelationImageToImageMetricv4< RegisteredImageType, RegisteredImageType, RegisteredImageType,
                                                        RealType >::New();
            break;
        case MUTUAL_INFORMATION:
        {
            auto mutInfoMetric =
                ::itk::MattesMutualInformationImageToImageMetricv4< RegisteredImageType, RegisteredImageType,
                                                                    RegisteredImageType,
                                                                    RealType >::New();
            mutInfoMetric->SetNumberOfHistogramBins(20);
            metric = mutInfoMetric;
        }

        break;
        default:
            OSLM_FATAL("Unknown metric");
    }

    // Initialize transform. Center the reference image.
    typedef typename ::itk::CenteredTransformInitializer< TransformType, RegisteredImageType, RegisteredImageType >
        TransformInitializerType;

    typename TransformInitializerType::Pointer initializer = TransformInitializerType::New();

    TransformType::Pointer itkTransform = TransformType::New();

    initializer->SetTransform(itkTransform);
    initializer->SetFixedImage(target);
    initializer->SetMovingImage(reference);

    initializer->MomentsOn();
    initializer->InitializeTransform();

    // Initialize transform. Set the initial translation and rotation.
    const ::glm::dmat4 initMat = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(_trf);
    const double scale         = std::pow(::glm::determinant(initMat), 1./3.);

    const ::glm::dquat orientation = ::glm::toQuat(initMat / scale);

    const ::glm::dvec3 translation = ::glm::dvec3(::glm::column(initMat, 3));
    const ::glm::dvec3 axis        = ::glm::axis(orientation);

    typedef TransformType::VersorType VersorType;
    typedef VersorType::VectorType VectorType;
    VectorType itkAxis;
    itkAxis[0] = axis.x;
    itkAxis[1] = axis.y;
    itkAxis[2] = axis.z;

    VectorType itkTrans;
    itkTrans[0] = translation.x;
    itkTrans[1] = translation.y;
    itkTrans[2] = translation.z;

    VersorType rotation;
    rotation.Set(itkAxis, ::glm::angle(orientation));

    itkTransform->SetRotation( rotation );
    itkTransform->SetTranslation(itkTrans);

    // Registration.
    auto registrator = RegistrationMethodType::New();
    auto optimizer   = OptimizerType::New();

    registrator->SetMetric(metric);
    registrator->SetOptimizer(optimizer);

    OptimizerType::ScalesType optimizerScales( static_cast<unsigned int>(itkTransform->GetNumberOfParameters()) );
    const double translationScale = 1.0 / 1000.0;
    optimizerScales[0] = 1.0;
    optimizerScales[1] = 1.0;
    optimizerScales[2] = 1.0;
    optimizerScales[3] = translationScale;
    optimizerScales[4] = translationScale;
    optimizerScales[5] = translationScale;

    optimizer->SetScales( optimizerScales );
    optimizer->SetDoEstimateLearningRateOnce( true );
    optimizer->SetDoEstimateLearningRateAtEachIteration( true );
    optimizer->SetMinimumStepLength(_minStep);
    optimizer->SetReturnBestParametersAndValue(true);
    optimizer->SetNumberOfIterations(_maxIterations);

    // The fixed image isn't tranformed, nearest neighbor interpolation is enough.
    auto fixedInterpolator  = ::itk::NearestNeighborInterpolateImageFunction< RegisteredImageType, RealType >::New();
    auto movingInterpolator = ::itk::LinearInterpolateImageFunction< RegisteredImageType, RealType >::New();

    metric->SetFixedInterpolator(fixedInterpolator.GetPointer());
    metric->SetMovingInterpolator(movingInterpolator.GetPointer());

    const unsigned int numberOfLevels = 1;

    registrator->SetInitialTransform(itkTransform);
    registrator->SetFixedImage(target);
    registrator->SetMovingImage(reference);

    RegistrationMethodType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
    shrinkFactorsPerLevel.SetSize( numberOfLevels );
    shrinkFactorsPerLevel[0] = 1;
    RegistrationMethodType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
    smoothingSigmasPerLevel.SetSize( numberOfLevels );
    smoothingSigmasPerLevel[0] = 0;
    registrator->SetNumberOfLevels( numberOfLevels );
    registrator->SetSmoothingSigmasPerLevel( smoothingSigmasPerLevel );
    registrator->SetShrinkFactorsPerLevel( shrinkFactorsPerLevel );

    RegistrationObserver<OptimizerType>::Pointer observer = RegistrationObserver<OptimizerType>::New();
    observer->SetMaxIterations( _maxIterations );
    optimizer->AddObserver( ::itk::IterationEvent(), observer );

    try
    {
        // Time for lift-off.
        registrator->Update();
    }
    catch( ::itk::ExceptionObject& err )
    {
        OSLM_FATAL("Error while registering : " << err);
    }

    if(!observer->ForceStopped())
    {
        // Get the last transform.
        const TransformType* finalTransform = registrator->GetTransform();

        const ::itk::Matrix<RealType, 3, 3> rigidMat = finalTransform->GetMatrix();
        const ::itk::Vector<RealType, 3> offset      = finalTransform->GetOffset();

        // Convert ::itk::RigidTransform to f4s matrix.
        for(std::uint8_t i = 0; i < 3; ++i)
        {
            _trf->setCoefficient(i, 3, offset[i]);
            for(std::uint8_t j = 0; j < 3; ++j)
            {
                _trf->setCoefficient(i, j, rigidMat(i, j));
            }
        }
    }
}

//------------------------------------------------------------------------------

} // itkRegistrationOp
