/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#ifdef WIN32
    #pragma warning(disable:4701)
#endif

#include "automatic_registration.hpp"

#include "filter/image/itk_image_caster.hpp"

#include <io/itk/helper/Transform.hpp>
#include <io/itk/itk.hpp>

#include <itkCommand.h>
#include <itkCorrelationImageToImageMetricv4.h>
#include <itkEuler3DTransform.h>
#include <itkImage.h>
#include <itkImageMomentsCalculator.h>
#include <itkImageRegistrationMethodv4.h>
#include <itkImageToImageMetricv4.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMattesMutualInformationImageToImageMetricv4.h>
#include <itkMeanSquaresImageToImageMetricv4.h>
#include <itkNearestNeighborInterpolateImageFunction.h>

#include <numeric>

namespace sight::filter::image
{

using TransformType = itk::Euler3DTransform<automatic_registration::RealType>;

using RegistrationMethodType = itk::ImageRegistrationMethodv4<
    automatic_registration::RegisteredImageType,
    automatic_registration::RegisteredImageType,
    TransformType
>;

class automatic_registration::automatic_registrationImpl final
{
public:

    /// Delete default constructors and assignment operators
    automatic_registrationImpl()                                             = delete;
    automatic_registrationImpl(const automatic_registrationImpl&)            = delete;
    automatic_registrationImpl(automatic_registrationImpl&&)                 = delete;
    automatic_registrationImpl& operator=(const automatic_registrationImpl&) = delete;
    automatic_registrationImpl& operator=(automatic_registrationImpl&&)      = delete;

    /// Constructor
    inline explicit automatic_registrationImpl(automatic_registration* const automaticRegistration) :
        m_automaticRegistration(automaticRegistration)
    {
    }

    /// Default destructor
    inline ~automatic_registrationImpl() noexcept = default;

    automatic_registration* m_automaticRegistration {nullptr};

    OptimizerType::Pointer m_optimizer {nullptr};

    RegistrationMethodType::Pointer m_registrator {nullptr};

    bool m_invert {false};

    //------------------------------------------------------------------------------

    inline void convertToF4sMatrix(const TransformType* _itkMat, const data::matrix4::sptr& _f4sMat) const
    {
        itk::Matrix<RealType, 3, 3> rigidMat = _itkMat->GetMatrix();
        itk::Vector<RealType, 3> offset      = _itkMat->GetOffset();

        if(m_invert)
        {
            rigidMat = itk::Matrix<RealType, 3, 3>(_itkMat->GetMatrix().GetTranspose());
            offset   = -(rigidMat * offset);
        }

        // Convert itk::RigidTransform to f4s matrix.
        for(std::uint8_t i = 0 ; i < 3 ; ++i)
        {
            (*_f4sMat)(i, 3) = offset[i];
            for(std::uint8_t j = 0 ; j < 3 ; ++j)
            {
                (*_f4sMat)(i, j) = rigidMat(i, j);
            }
        }
    }

    //------------------------------------------------------------------------------

    inline static double computeVolume(const data::image::csptr& _img)
    {
        const auto& spacing = _img->getSpacing();
        const auto& size    = _img->size();

        SIGHT_ASSERT(
            "Degenerated image. Spacing and size should be of the same dimension.",
            spacing.size() == size.size()
        );

        const double voxelVolume   = std::accumulate(spacing.begin(), spacing.end(), 1., std::multiplies<>());
        const std::size_t nbVoxels = std::accumulate(
            size.begin(),
            size.end(),
            std::size_t(1),
            std::multiplies<>()
        );

        return voxelVolume * static_cast<double>(nbVoxels);
    }
};

//------------------------------------------------------------------------------

class RegistrationObserver : public itk::Command
{
public:

    using Self       = RegistrationObserver;
    using Superclass = itk::Command;
    using Pointer    = itk::SmartPointer<Self>;
    itkNewMacro(Self)

    /// Command to be executed. Updates the progress bar.
    void Execute(itk::Object* caller, const itk::EventObject& event) override
    {
        const itk::Object* constCaller = caller;
        Execute(constCaller, event);
    }

    /// Const overload of the above method.
    void Execute(const itk::Object* /*caller*/, const itk::EventObject& event) override
    {
        {
            if(itk::IterationEvent().CheckEvent(&event))
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
    RegistrationObserver() = default;

    std::function<void()> m_iterationCallback;
};

automatic_registration::automatic_registration() noexcept :
    m_pimpl(std::make_unique<automatic_registration::automatic_registrationImpl>(this))
{
}

automatic_registration::~automatic_registration() noexcept = default;

//------------------------------------------------------------------------------

void automatic_registration::registerImage(
    const data::image::csptr& _target,
    const data::image::csptr& _reference,
    const data::matrix4::sptr& _trf,
    metric_t _metric,
    const MultiResolutionParametersType& _multiResolutionParameters,
    RealType _samplingPercentage,
    double _minStep,
    std::uint64_t _maxIterations,
    IterationCallbackType _callback
)
{
    typename itk::ImageToImageMetricv4<RegisteredImageType, RegisteredImageType, RegisteredImageType,
                                       RealType>::Pointer metric;

    data::image::csptr ref = _reference;
    data::image::csptr tgt = _target;

    m_pimpl->m_invert = m_pimpl->computeVolume(tgt) < m_pimpl->computeVolume(ref);

    // Always register images with the largest one being fixed.
    // Otherwise, our metric may not find any matching points between them.
    if(m_pimpl->m_invert)
    {
        std::swap(ref, tgt);
    }

    // Convert input images to float. Integer images aren't supported yet.
    RegisteredImageType::Pointer target    = castTo<float>(tgt);
    RegisteredImageType::Pointer reference = castTo<float>(ref);

    // Choose a metric.
    switch(_metric)
    {
        case MEAN_SQUARES:
            metric =
                itk::MeanSquaresImageToImageMetricv4<RegisteredImageType, RegisteredImageType, RegisteredImageType,
                                                     RealType>::New();
            break;

        case NORMALIZED_CORRELATION:
            metric =
                itk::CorrelationImageToImageMetricv4<RegisteredImageType, RegisteredImageType, RegisteredImageType,
                                                     RealType>::New();
            break;

        case MUTUAL_INFORMATION:
        {
            auto mutInfoMetric =
                itk::MattesMutualInformationImageToImageMetricv4<RegisteredImageType, RegisteredImageType,
                                                                 RegisteredImageType,
                                                                 RealType>::New();
            // TODO: find a strategy to compute the appropriate number of bins or let the user set it.
            // More bins means better precision but longer evaluation.
            mutInfoMetric->SetNumberOfHistogramBins(20);
            metric = mutInfoMetric;
            break;
        }

        default:
            SIGHT_FATAL("Unknown metric");
    }

    TransformType::Pointer itkTransform = TransformType::New();

    itk::Matrix<RealType, 3, 3> m;
    itk::Vector<RealType, 3> t;

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        t[i] = (*_trf)(i, 3);
        for(std::uint8_t j = 0 ; j < 3 ; ++j)
        {
            m(i, j) = (*_trf)(i, j);
        }
    }

    if(m_pimpl->m_invert)
    {
        // Our algorithm assumes m to be orthogonal. If it isn't, an exception will be thrown later on.
        m = itk::Matrix<RealType, 3, 3>(m.GetTranspose());
        t = -(m * t);
    }

    // Initialize the transform.
    itk::ImageMomentsCalculator<RegisteredImageType>::Pointer momentsCalculator =
        itk::ImageMomentsCalculator<RegisteredImageType>::New();

    momentsCalculator->SetImage(target);
    momentsCalculator->Compute();

    // Set the rigid transform center to the center of mass of the target image.
    // This truly helps the registration algorithm.
    itkTransform->SetCenter(momentsCalculator->GetCenterOfGravity());

    // Setting the offset also recomputes the translation using the offset, rotation and center
    // so the matrix needs to be set first.
    itkTransform->SetMatrix(m);
    itkTransform->SetOffset(t);

    // Registration.
    m_pimpl->m_registrator = RegistrationMethodType::New();
    m_pimpl->m_optimizer   = OptimizerType::New();

    m_pimpl->m_registrator->SetMetric(metric);
    m_pimpl->m_registrator->SetOptimizer(m_pimpl->m_optimizer);

    OptimizerType::ScalesType optimizerScales(static_cast<unsigned int>(itkTransform->GetNumberOfParameters()));
    const double translationScale = 1.0 / 1000.0;
    optimizerScales[0] = 1.0;
    optimizerScales[1] = 1.0;
    optimizerScales[2] = 1.0;
    optimizerScales[3] = translationScale;
    optimizerScales[4] = translationScale;
    optimizerScales[5] = translationScale;

    m_pimpl->m_optimizer->SetScales(optimizerScales);
    m_pimpl->m_optimizer->SetDoEstimateLearningRateAtEachIteration(true);
    m_pimpl->m_optimizer->SetMinimumStepLength(_minStep);

    // The solution is the transform returned when optimization ends.
    m_pimpl->m_optimizer->SetReturnBestParametersAndValue(false);
    m_pimpl->m_optimizer->SetNumberOfIterations(_maxIterations);

    // The fixed image isn't transformed, nearest neighbor interpolation is enough.
    auto fixedInterpolator  = itk::NearestNeighborInterpolateImageFunction<RegisteredImageType, RealType>::New();
    auto movingInterpolator = itk::LinearInterpolateImageFunction<RegisteredImageType, RealType>::New();

    metric->SetFixedInterpolator(fixedInterpolator.GetPointer());
    metric->SetMovingInterpolator(movingInterpolator.GetPointer());

    // Number of registration stages
    SIGHT_ASSERT("255 is the maximum number of steps.", _multiResolutionParameters.size() < 256);
    const auto numberOfLevels = std::uint8_t(_multiResolutionParameters.size());

    RegistrationMethodType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
    shrinkFactorsPerLevel.SetSize(numberOfLevels);
    RegistrationMethodType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
    smoothingSigmasPerLevel.SetSize(numberOfLevels);

    // We set the shrink factor and smoothing Sigma for each stage.
    for(std::uint8_t i = 0 ; i < numberOfLevels ; ++i)
    {
        const auto& stageParameters = _multiResolutionParameters[i];
        shrinkFactorsPerLevel[i]   = stageParameters.first;
        smoothingSigmasPerLevel[i] = stageParameters.second;
    }

    m_pimpl->m_registrator->SetInitialTransform(itkTransform);
    m_pimpl->m_registrator->SetFixedImage(target);
    m_pimpl->m_registrator->SetMovingImage(reference);

    m_pimpl->m_registrator->SetMetricSamplingPercentage(_samplingPercentage);

    const auto samplingStrategy = _samplingPercentage < 1.0
                                  ? RegistrationMethodType::REGULAR : RegistrationMethodType::NONE;

    m_pimpl->m_registrator->SetMetricSamplingStrategy(samplingStrategy);
    m_pimpl->m_registrator->SetNumberOfLevels(itk::SizeValueType(numberOfLevels));
    m_pimpl->m_registrator->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
    m_pimpl->m_registrator->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);
    m_pimpl->m_registrator->SetSmoothingSigmasAreSpecifiedInPhysicalUnits(true);

    RegistrationObserver::Pointer observer = RegistrationObserver::New();

    if(_callback)
    {
        observer->setCallback(_callback);
        m_pimpl->m_optimizer->AddObserver(itk::IterationEvent(), observer);
    }

    try
    {
        // Time for lift-off.
        m_pimpl->m_registrator->Update();
        this->getCurrentMatrix(_trf);
    }
    catch(itk::ExceptionObject& err)
    {
        SIGHT_ERROR("Error while registering : " << err);
    }
}

//------------------------------------------------------------------------------

void automatic_registration::stopRegistration()
{
    if((m_pimpl->m_optimizer != nullptr) && (m_pimpl->m_registrator != nullptr))
    {
        // Stop registration by removing all levels.
        m_pimpl->m_registrator->SetNumberOfLevels(0);
        m_pimpl->m_optimizer->StopOptimization();
    }
}

//------------------------------------------------------------------------------

automatic_registration::RealType automatic_registration::getCurrentMetricValue() const
{
    SIGHT_ASSERT("No optimization process running.", m_pimpl->m_optimizer);
    return m_pimpl->m_optimizer->GetCurrentMetricValue();
}

//------------------------------------------------------------------------------

const automatic_registration::OptimizerType::ParametersType& automatic_registration::getCurrentParameters() const
{
    SIGHT_ASSERT("No optimization process running.", m_pimpl->m_optimizer);
    return m_pimpl->m_optimizer->GetCurrentPosition();
}

//------------------------------------------------------------------------------

automatic_registration::RealType automatic_registration::getRelaxationFactor() const
{
    SIGHT_ASSERT("No optimization process running.", m_pimpl->m_optimizer);
    return m_pimpl->m_optimizer->GetRelaxationFactor();
}

//------------------------------------------------------------------------------

automatic_registration::RealType automatic_registration::getLearningRate() const
{
    SIGHT_ASSERT("No optimization process running.", m_pimpl->m_optimizer);
    return m_pimpl->m_optimizer->GetLearningRate();
}

//------------------------------------------------------------------------------

automatic_registration::RealType automatic_registration::getGradientMagnitudeTolerance() const
{
    SIGHT_ASSERT("No optimization process running.", m_pimpl->m_optimizer);
    return m_pimpl->m_optimizer->GetGradientMagnitudeTolerance();
}

//------------------------------------------------------------------------------

itk::SizeValueType automatic_registration::getCurrentIteration() const
{
    SIGHT_ASSERT("No optimization process running.", m_pimpl->m_optimizer);
    return m_pimpl->m_optimizer->GetCurrentIteration();
}

//------------------------------------------------------------------------------

itk::SizeValueType filter::image::automatic_registration::getCurrentLevel() const
{
    SIGHT_ASSERT("No registration process running.", m_pimpl->m_registrator);
    return m_pimpl->m_registrator->GetCurrentLevel();
}

//------------------------------------------------------------------------------

void automatic_registration::getCurrentMatrix(const data::matrix4::sptr& _trf) const
{
    SIGHT_ASSERT("No registration process running.", m_pimpl->m_registrator);
    const auto* itkMatrix = m_pimpl->m_registrator->GetTransform();
    m_pimpl->convertToF4sMatrix(itkMatrix, _trf);
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image
