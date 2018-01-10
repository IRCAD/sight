/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "itkRegistrationOp/AutomaticRegistration.hpp"

#include "itkRegistrationOp/ItkImageCaster.hpp"

#include <fwDataTools/TransformationMatrix3D.hpp>

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
#include <itkImageToImageMetricv4.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMattesMutualInformationImageToImageMetricv4.h>
#include <itkMeanSquaresImageToImageMetricv4.h>
#include <itkNearestNeighborInterpolateImageFunction.h>

namespace itkRegistrationOp
{

typedef typename ::itk::Image< float, 3 > RegisteredImageType;

//------------------------------------------------------------------------------

class RegistrationObserver : public ::itk::Command
{
public:
    typedef  RegistrationObserver Self;
    typedef ::itk::Command Superclass;
    typedef ::itk::SmartPointer<Self>   Pointer;
    itkNewMacro( Self )

    /// Command to be executed. Updates the progress bar.
    void Execute(::itk::Object* caller, const ::itk::EventObject& event) override
    {
        const itk::Object* constCaller = caller;
        Execute( constCaller, event);
    }

    /// Const overload of the above method.
    void Execute(const ::itk::Object*, const ::itk::EventObject& event) override
    {
        {
            if( ::itk::IterationEvent().CheckEvent( &event ) )
            {
                m_iterationCallback();
            }
        }
    }

    //------------------------------------------------------------------------------

    void setCallback(std::function<void()> _callback)
    {
        m_iterationCallback = _callback;
    }

private:

    /// Constructor, initializes progress dialog and sets the user cancel callback.
    RegistrationObserver()
    {
    }

    std::function<void()> m_iterationCallback;

};

//------------------------------------------------------------------------------

void AutomaticRegistration::registerImage(const ::fwData::Image::csptr& _target,
                                          const ::fwData::Image::csptr& _reference,
                                          const ::fwData::TransformationMatrix3D::sptr& _trf,
                                          MetricType _metric,
                                          const MultiResolutionParametersType& _multiResolutionParameters,
                                          RealType _samplingPercentage,
                                          double _minStep,
                                          unsigned long _maxIterations,
                                          IterationCallbackType _callback)
{
    typename ::itk::ImageToImageMetricv4< RegisteredImageType, RegisteredImageType, RegisteredImageType,
                                          RealType >::Pointer metric;

    // Convert input images to float. Integer images aren't supported yet.
    RegisteredImageType::Pointer target    = castTo<float>(_target);
    RegisteredImageType::Pointer reference = castTo<float>(_reference);

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

    TransformType::Pointer itkTransform = TransformType::New();

    const RegisteredImageType::SizeType targetSize = target->GetLargestPossibleRegion().GetSize();
    ::itk::Vector<RealType, 3> targetCenter;

    for(std::uint8_t i = 0; i < 3; ++i)
    {
        targetCenter[i] = RealType(targetSize[i]/2 - 1);
    }

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

    itkTransform->SetCenter(targetCenter);
    // Setting the offset also recomputes the translation using the offset, rotation and center
    // so the matrix needs to be set first.
    itkTransform->SetMatrix(m);
    itkTransform->SetOffset(t);

    // Registration.
    m_registrator = RegistrationMethodType::New();
    auto optimizer = OptimizerType::New();

    m_registrator->SetMetric(metric);
    m_registrator->SetOptimizer(optimizer);

    OptimizerType::ScalesType optimizerScales(static_cast<unsigned int>(itkTransform->GetNumberOfParameters()));
    const double translationScale = 1.0 / 1000.0;
    optimizerScales[0] = 1.0;
    optimizerScales[1] = 1.0;
    optimizerScales[2] = 1.0;
    optimizerScales[3] = translationScale;
    optimizerScales[4] = translationScale;
    optimizerScales[5] = translationScale;

    optimizer->SetScales( optimizerScales );
    optimizer->SetDoEstimateLearningRateAtEachIteration( true );
    optimizer->SetMinimumStepLength(_minStep);
    optimizer->SetReturnBestParametersAndValue(true);
    optimizer->SetNumberOfIterations(_maxIterations);

    // The fixed image isn't tranformed, nearest neighbor interpolation is enough.
    auto fixedInterpolator  = ::itk::NearestNeighborInterpolateImageFunction< RegisteredImageType, RealType >::New();
    auto movingInterpolator = ::itk::LinearInterpolateImageFunction< RegisteredImageType, RealType >::New();

    metric->SetFixedInterpolator(fixedInterpolator.GetPointer());
    metric->SetMovingInterpolator(movingInterpolator.GetPointer());

    // Number of registration stages
    SLM_ASSERT("255 is the maximum number of steps.", _multiResolutionParameters.size() < 256);
    const std::uint8_t numberOfLevels = std::uint8_t(_multiResolutionParameters.size());

    RegistrationMethodType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
    shrinkFactorsPerLevel.SetSize( numberOfLevels );
    RegistrationMethodType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
    smoothingSigmasPerLevel.SetSize( numberOfLevels );

    // For each stages, we set the shrink factor and smoothing Sigma
    for( std::uint8_t i = 0; i < numberOfLevels; ++i  )
    {
        const auto& stageParameters = _multiResolutionParameters[i];
        shrinkFactorsPerLevel[i]   = stageParameters.first;
        smoothingSigmasPerLevel[i] = stageParameters.second;
    }

    m_registrator->SetInitialTransform(itkTransform);
    m_registrator->SetFixedImage(target);
    m_registrator->SetMovingImage(reference);
    m_registrator->SetMetricSamplingPercentage(_samplingPercentage);
    m_registrator->SetNumberOfLevels(::itk::SizeValueType(numberOfLevels));
    m_registrator->SetSmoothingSigmasPerLevel( smoothingSigmasPerLevel );
    m_registrator->SetShrinkFactorsPerLevel( shrinkFactorsPerLevel );
    m_registrator->SetSmoothingSigmasAreSpecifiedInPhysicalUnits(true);

    RegistrationObserver::Pointer observer = RegistrationObserver::New();

    if(_callback)
    {
        observer->setCallback(_callback);
        optimizer->AddObserver( ::itk::IterationEvent(), observer );
    }

    m_optimizer = optimizer;

    try
    {
        // Time for lift-off.
        m_registrator->Update();
    }
    catch( ::itk::ExceptionObject& err )
    {
        OSLM_ERROR("Error while registering : " << err);
        return;
    }

    m_optimizer = nullptr;

    // Get the last transform.
    const TransformType* finalTransform = m_registrator->GetTransform();

    convertToF4sMatrix(finalTransform, _trf);
}

//------------------------------------------------------------------------------

void AutomaticRegistration::stopRegistration()
{
    if(m_optimizer)
    {
        // Stop registration by removing all levels.
        m_registrator->SetNumberOfLevels(0);
        m_optimizer->StopOptimization();
    }
}

//------------------------------------------------------------------------------

AutomaticRegistration::RealType AutomaticRegistration::getCurrentMetricValue() const
{
    OSLM_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetCurrentMetricValue();
}

//------------------------------------------------------------------------------

const AutomaticRegistration::OptimizerType::ParametersType& AutomaticRegistration::getCurrentParameters() const
{
    OSLM_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetCurrentPosition();
}

//------------------------------------------------------------------------------

AutomaticRegistration::RealType AutomaticRegistration::getRelaxationFactor() const
{
    OSLM_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetRelaxationFactor();
}

//------------------------------------------------------------------------------

AutomaticRegistration::RealType AutomaticRegistration::getLearningRate() const
{
    OSLM_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetLearningRate();
}

//------------------------------------------------------------------------------

AutomaticRegistration::RealType AutomaticRegistration::getGradientMagnitudeTolerance() const
{
    OSLM_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetGradientMagnitudeTolerance();
}

//------------------------------------------------------------------------------

itk::SizeValueType AutomaticRegistration::getCurrentIteration() const
{
    OSLM_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetCurrentIteration();
}

//------------------------------------------------------------------------------

itk::SizeValueType itkRegistrationOp::AutomaticRegistration::getCurrentLevel() const
{
    OSLM_ASSERT("No registration process running.", m_registrator);
    return m_registrator->GetCurrentLevel();
}

//------------------------------------------------------------------------------

void AutomaticRegistration::getCurrentMatrix(const ::fwData::TransformationMatrix3D::sptr& _trf) const
{
    OSLM_ASSERT("No registration process running.", m_registrator);
    auto itkMatrix = m_registrator->GetTransform();
    convertToF4sMatrix(itkMatrix, _trf);
}

//------------------------------------------------------------------------------

void AutomaticRegistration::convertToF4sMatrix(const AutomaticRegistration::TransformType* _itkMat,
                                               const fwData::TransformationMatrix3D::sptr& _f4sMat)
{
    const ::itk::Matrix<RealType, 3, 3> rigidMat = _itkMat->GetMatrix();
    const ::itk::Vector<RealType, 3> offset      = _itkMat->GetOffset();

    // Convert ::itk::RigidTransform to f4s matrix.
    for(std::uint8_t i = 0; i < 3; ++i)
    {
        _f4sMat->setCoefficient(i, 3, offset[i]);
        for(std::uint8_t j = 0; j < 3; ++j)
        {
            _f4sMat->setCoefficient(i, j, rigidMat(i, j));
        }
    }
}

//------------------------------------------------------------------------------

} // itkRegistrationOp
