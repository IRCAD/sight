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

using transform_t = itk::Euler3DTransform<automatic_registration::real_t>;

using registration_method_t = itk::ImageRegistrationMethodv4<
    automatic_registration::registered_image_t,
    automatic_registration::registered_image_t,
    transform_t
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
    inline explicit automatic_registrationImpl(automatic_registration* const _automatic_registration) :
        m_automaticRegistration(_automatic_registration)
    {
    }

    /// Default destructor
    inline ~automatic_registrationImpl() noexcept = default;

    automatic_registration* m_automaticRegistration {nullptr};

    optimizer_t::Pointer m_optimizer {nullptr};

    registration_method_t::Pointer m_registrator {nullptr};

    bool m_invert {false};

    //------------------------------------------------------------------------------

    inline void convertfrom_eigenMatrix(const transform_t* _itk_mat, const data::matrix4::sptr& _f4s_mat) const
    {
        itk::Matrix<real_t, 3, 3> rigid_mat = _itk_mat->GetMatrix();
        itk::Vector<real_t, 3> offset       = _itk_mat->GetOffset();

        if(m_invert)
        {
            rigid_mat = itk::Matrix<real_t, 3, 3>(_itk_mat->GetMatrix().GetTranspose());
            offset    = -(rigid_mat * offset);
        }

        // Convert itk::RigidTransform to f4s matrix.
        for(std::uint8_t i = 0 ; i < 3 ; ++i)
        {
            (*_f4s_mat)(i, 3) = offset[i];
            for(std::uint8_t j = 0 ; j < 3 ; ++j)
            {
                (*_f4s_mat)(i, j) = rigid_mat(i, j);
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

        const double voxel_volume   = std::accumulate(spacing.begin(), spacing.end(), 1., std::multiplies<>());
        const std::size_t nb_voxels = std::accumulate(
            size.begin(),
            size.end(),
            std::size_t(1),
            std::multiplies<>()
        );

        return voxel_volume * static_cast<double>(nb_voxels);
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
    void Execute(itk::Object* _caller, const itk::EventObject& _event) override
    {
        const itk::Object* const_caller = _caller;
        Execute(const_caller, _event);
    }

    /// Const overload of the above method.
    void Execute(const itk::Object* /*caller*/, const itk::EventObject& _event) override
    {
        {
            if(itk::IterationEvent().CheckEvent(&_event))
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
    const multi_resolution_parameters_t& _multi_resolution_parameters,
    real_t _sampling_percentage,
    double _min_step,
    std::uint64_t _max_iterations,
    iteration_callback_t _callback
)
{
    typename itk::ImageToImageMetricv4<registered_image_t, registered_image_t, registered_image_t,
                                       real_t>::Pointer metric;

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
    registered_image_t::Pointer target    = cast_to<float>(tgt);
    registered_image_t::Pointer reference = cast_to<float>(ref);

    // Choose a metric.
    switch(_metric)
    {
        case MEAN_SQUARES:
            metric =
                itk::MeanSquaresImageToImageMetricv4<registered_image_t, registered_image_t, registered_image_t,
                                                     real_t>::New();
            break;

        case NORMALIZED_CORRELATION:
            metric =
                itk::CorrelationImageToImageMetricv4<registered_image_t, registered_image_t, registered_image_t,
                                                     real_t>::New();
            break;

        case MUTUAL_INFORMATION:
        {
            auto mut_info_metric =
                itk::MattesMutualInformationImageToImageMetricv4<registered_image_t, registered_image_t,
                                                                 registered_image_t,
                                                                 real_t>::New();
            // TODO: find a strategy to compute the appropriate number of bins or let the user set it.
            // More bins means better precision but longer evaluation.
            mut_info_metric->SetNumberOfHistogramBins(20);
            metric = mut_info_metric;
            break;
        }

        default:
            SIGHT_FATAL("Unknown metric");
    }

    transform_t::Pointer itk_transform = transform_t::New();

    itk::Matrix<real_t, 3, 3> m;
    itk::Vector<real_t, 3> t;

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
        m = itk::Matrix<real_t, 3, 3>(m.GetTranspose());
        t = -(m * t);
    }

    // Initialize the transform.
    itk::ImageMomentsCalculator<registered_image_t>::Pointer moments_calculator =
        itk::ImageMomentsCalculator<registered_image_t>::New();

    moments_calculator->SetImage(target);
    moments_calculator->Compute();

    // Set the rigid transform center to the center of mass of the target image.
    // This truly helps the registration algorithm.
    itk_transform->SetCenter(moments_calculator->GetCenterOfGravity());

    // Setting the offset also recomputes the translation using the offset, rotation and center
    // so the matrix needs to be set first.
    itk_transform->SetMatrix(m);
    itk_transform->SetOffset(t);

    // Registration.
    m_pimpl->m_registrator = registration_method_t::New();
    m_pimpl->m_optimizer   = optimizer_t::New();

    m_pimpl->m_registrator->SetMetric(metric);
    m_pimpl->m_registrator->SetOptimizer(m_pimpl->m_optimizer);

    optimizer_t::ScalesType optimizer_scales(static_cast<unsigned int>(itk_transform->GetNumberOfParameters()));
    const double translation_scale = 1.0 / 1000.0;
    optimizer_scales[0] = 1.0;
    optimizer_scales[1] = 1.0;
    optimizer_scales[2] = 1.0;
    optimizer_scales[3] = translation_scale;
    optimizer_scales[4] = translation_scale;
    optimizer_scales[5] = translation_scale;

    m_pimpl->m_optimizer->SetScales(optimizer_scales);
    m_pimpl->m_optimizer->SetDoEstimateLearningRateAtEachIteration(true);
    m_pimpl->m_optimizer->SetMinimumStepLength(_min_step);

    // The solution is the transform returned when optimization ends.
    m_pimpl->m_optimizer->SetReturnBestParametersAndValue(false);
    m_pimpl->m_optimizer->SetNumberOfIterations(_max_iterations);

    // The fixed image isn't transformed, nearest neighbor interpolation is enough.
    auto fixed_interpolator  = itk::NearestNeighborInterpolateImageFunction<registered_image_t, real_t>::New();
    auto moving_interpolator = itk::LinearInterpolateImageFunction<registered_image_t, real_t>::New();

    metric->SetFixedInterpolator(fixed_interpolator.GetPointer());
    metric->SetMovingInterpolator(moving_interpolator.GetPointer());

    // Number of registration stages
    SIGHT_ASSERT("255 is the maximum number of steps.", _multi_resolution_parameters.size() < 256);
    const auto number_of_levels = std::uint8_t(_multi_resolution_parameters.size());

    registration_method_t::ShrinkFactorsArrayType shrink_factors_per_level;
    shrink_factors_per_level.SetSize(number_of_levels);
    registration_method_t::SmoothingSigmasArrayType smoothing_sigmas_per_level;
    smoothing_sigmas_per_level.SetSize(number_of_levels);

    // We set the shrink factor and smoothing Sigma for each stage.
    for(std::uint8_t i = 0 ; i < number_of_levels ; ++i)
    {
        const auto& stage_parameters = _multi_resolution_parameters[i];
        shrink_factors_per_level[i]   = stage_parameters.first;
        smoothing_sigmas_per_level[i] = stage_parameters.second;
    }

    m_pimpl->m_registrator->SetInitialTransform(itk_transform);
    m_pimpl->m_registrator->SetFixedImage(target);
    m_pimpl->m_registrator->SetMovingImage(reference);

    m_pimpl->m_registrator->SetMetricSamplingPercentage(_sampling_percentage);

    const auto sampling_strategy = _sampling_percentage < 1.0
                                   ? registration_method_t::REGULAR : registration_method_t::NONE;

    m_pimpl->m_registrator->SetMetricSamplingStrategy(sampling_strategy);
    m_pimpl->m_registrator->SetNumberOfLevels(itk::SizeValueType(number_of_levels));
    m_pimpl->m_registrator->SetSmoothingSigmasPerLevel(smoothing_sigmas_per_level);
    m_pimpl->m_registrator->SetShrinkFactorsPerLevel(shrink_factors_per_level);
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

automatic_registration::real_t automatic_registration::getCurrentMetricValue() const
{
    SIGHT_ASSERT("No optimization process running.", m_pimpl->m_optimizer);
    return m_pimpl->m_optimizer->GetCurrentMetricValue();
}

//------------------------------------------------------------------------------

const automatic_registration::optimizer_t::ParametersType& automatic_registration::getCurrentParameters() const
{
    SIGHT_ASSERT("No optimization process running.", m_pimpl->m_optimizer);
    return m_pimpl->m_optimizer->GetCurrentPosition();
}

//------------------------------------------------------------------------------

automatic_registration::real_t automatic_registration::getRelaxationFactor() const
{
    SIGHT_ASSERT("No optimization process running.", m_pimpl->m_optimizer);
    return m_pimpl->m_optimizer->GetRelaxationFactor();
}

//------------------------------------------------------------------------------

automatic_registration::real_t automatic_registration::getLearningRate() const
{
    SIGHT_ASSERT("No optimization process running.", m_pimpl->m_optimizer);
    return m_pimpl->m_optimizer->GetLearningRate();
}

//------------------------------------------------------------------------------

automatic_registration::real_t automatic_registration::getGradientMagnitudeTolerance() const
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
    const auto* itk_matrix = m_pimpl->m_registrator->GetTransform();
    m_pimpl->convertfrom_eigenMatrix(itk_matrix, _trf);
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image
