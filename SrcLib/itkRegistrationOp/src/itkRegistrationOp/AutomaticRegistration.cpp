/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "itkRegistrationOp/AutomaticRegistration.hpp"

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
        typename InputImageType::Pointer tmp = ::fwItkIO::itkImageFactory< InputImageType >(p.i_img);

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

void AutomaticRegistration::registerImage(const ::fwData::Image::csptr& _target,
                                          const ::fwData::Image::csptr& _reference,
                                          const ::fwData::TransformationMatrix3D::sptr& _trf,
                                          MetricType _metric,
                                          double _minStep,
                                          unsigned long _maxIterations)
{
    typedef double RealType; // Numeric type used for internal computations.
    typedef typename ::itk::RegularStepGradientDescentOptimizerv4<RealType> OptimizerType;

    typedef typename ::itk::VersorRigid3DTransform< RealType > TransformType;
    typedef typename ::itk::ImageRegistrationMethodv4< RegisteredImageType, RegisteredImageType, TransformType >
        RegistrationMethodType;

    typename ::itk::ImageToImageMetricv4< RegisteredImageType, RegisteredImageType, RegisteredImageType,
                                          RealType >::Pointer metric;

    // Convert input images to float. Integer images aren't supported yet.
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
            // TODO: find a strategy to compute the appropriate number of bins or let the user set it.
            // More bins means better precision but longer evaluation.
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

    // This class initializes the transform by setting its center and translation.
    // If our target and reference image don't change than this will always yield the same result.
    typename TransformInitializerType::Pointer initializer = TransformInitializerType::New();

    TransformType::Pointer itkTransform = TransformType::New();

    ::itk::Matrix<RealType, 3, 3> m;
    ::itk::Vector<RealType, 3> t;

    for(std::uint8_t i = 0; i < 3; ++i)
    {
        t[i] = _trf->getCoefficient(i, 3);
        for(std::uint8_t j = 0; j < 3; ++j)
        {
            m(i, j) = _trf->getCoefficient(i, j);
        }
    }

    initializer->SetTransform(itkTransform);
    initializer->SetFixedImage(target);
    initializer->SetMovingImage(reference);

    initializer->MomentsOn();
    initializer->InitializeTransform();

    // Setting the offset also recomputes the translation using the offset, rotation and center
    // so the matrix needs to be set first.
    itkTransform->SetMatrix(m);
    itkTransform->SetOffset(t);

    // Registration.
    auto registrator = RegistrationMethodType::New();
    auto optimizer   = OptimizerType::New();

    registrator->SetMetric(metric);
    registrator->SetOptimizer(optimizer);

    OptimizerType::ScalesType optimizerScales(static_cast<unsigned int>(itkTransform->GetNumberOfParameters()));
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

    registrator->SetInitialTransform(itkTransform);
    registrator->SetFixedImage(target);
    registrator->SetMovingImage(reference);

    // TODO: handle multi-resolution registration.
    const unsigned int numberOfLevels = 1;

    RegistrationMethodType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
    shrinkFactorsPerLevel.SetSize( numberOfLevels );
    shrinkFactorsPerLevel[0] = 1;
    RegistrationMethodType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
    smoothingSigmasPerLevel.SetSize( numberOfLevels );
    smoothingSigmasPerLevel[0] = 0;
    registrator->SetNumberOfLevels( numberOfLevels );
    registrator->SetSmoothingSigmasPerLevel( smoothingSigmasPerLevel );
    registrator->SetShrinkFactorsPerLevel( shrinkFactorsPerLevel );

    const bool hasGui = ::fwGui::isBackendLoaded();

    RegistrationObserver<OptimizerType>::Pointer observer;

    if(hasGui)
    {
        observer = RegistrationObserver<OptimizerType>::New();
        observer->setMaxIterations( _maxIterations );
        optimizer->AddObserver( ::itk::IterationEvent(), observer );
    }

    try
    {
        // Time for lift-off.
        registrator->Update();
    }
    catch( ::itk::ExceptionObject& err )
    {
        OSLM_FATAL("Error while registering : " << err);
    }

    if(!hasGui || !observer->forceStopped())
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
