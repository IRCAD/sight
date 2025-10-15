/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
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

#include "filter/image/detail/itk_image_caster.hpp"

#include <io/itk/helper/transform.hpp>
#include <io/itk/itk.hpp>

#include <itkCommand.h>
#include <itkCorrelationImageToImageMetricv4.h>
#include <itkEuler3DTransform.h>
#include <itkImage.h>
#include <itkImageMomentsCalculator.h>
#include <itkImageRegistrationMethodv4.h>
#include <itkImageToImageMetricv4.h>
#include <itkIntTypes.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMattesMutualInformationImageToImageMetricv4.h>
#include <itkMeanSquaresImageToImageMetricv4.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkRegularStepGradientDescentOptimizerv4.h>

#include <algorithm>
#include <numeric>

namespace sight::filter::image
{

using registered_image_t = itk::Image<float, 3>;

/// Numeric type used for internal computations.
using real_t      = double;
using optimizer_t = itk::RegularStepGradientDescentOptimizerv4<real_t>;

/// Shrink factors per level and smoothing sigmas per level
using multi_resolution_parameters_t = std::vector<std::pair<itk::SizeValueType, real_t> >;

using transform_t = itk::Euler3DTransform<real_t>;

using registration_method_t = itk::ImageRegistrationMethodv4<registered_image_t, registered_image_t, transform_t>;

//------------------------------------------------------------------------------

static inline double compute_volume(const data::image& _img)
{
    const auto& spacing = _img.spacing();
    const auto& size    = _img.size();

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

//------------------------------------------------------------------------------

class registration_observer : public itk::Command
{
public:

    using self_t     = registration_observer;
    using superclass = itk::Command;
    using Pointer    = itk::SmartPointer<self_t>;
    itkNewMacro(self_t)

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
                m_iteration_callback();
            }
        }
    }

    //------------------------------------------------------------------------------

    void set_callback(std::function<void()> _callback)
    {
        m_iteration_callback = _callback;
    }

private:

    /// Constructor, initializes progress dialog and sets the user cancel callback.
    registration_observer() = default;

    std::function<void()> m_iteration_callback;
};

/**
 * @brief Static class for automatic image registration. Uses the newer ITKv4 registration framework.
 */
class automatic_registration
{
public:

    using iteration_callback_t = std::function<void ()>;

    automatic_registration() noexcept          = default;
    virtual ~automatic_registration() noexcept = default;

    /**
     * @brief find a rigid transform matching the reference image with the target image.
     * @param[in] _target target, i.e. the static image.
     * @param[in] _reference reference, i.e. the image that will be transformed into the target.
     * @param[out] _trf the resulting rigid transform.
     * @param[in] _metric the metric to be used for registration.
     * @param[in] _multi_resolution_parameters Shrink factors per level and smoothing sigmas per level
     * @param[in] _sampling_percentage the percentage of sample to use for registration
     * @param[in] _min_step minimum step for used by optimizer for each iteration.
     * @param[in] _max_iterations the maximum number of iterations
     */
    void register_image(
        const data::image& _target,
        const data::image& _reference,
        data::matrix4& _trf,
        metric_t _metric,
        const multi_resolution_parameters_t& _multi_resolution_parameters,
        real_t _sampling_percentage    = 1.0,
        double _min_step               = 0.0001,
        std::uint64_t _max_iterations  = 200,
        iteration_callback_t _callback = nullptr
    );

    void stop_registration() const;

    /// Current metric evaluated by the optimizer.
    [[nodiscard]] real_t get_current_metric_value() const;

    /// Current set of parameters used to evaluate the metric in the optimizer.
    [[nodiscard]] const optimizer_t::ParametersType& get_current_parameters() const;

    /// Gradient descent relaxation factor.
    [[nodiscard]] real_t get_relaxation_factor() const;

    /// Gradient descent learning rate.
    [[nodiscard]] real_t get_learning_rate() const;

    /// Gradient magnitude tolerance.
    [[nodiscard]] real_t get_gradient_magnitude_tolerance() const;

    /// Current optimizer iteration.
    [[nodiscard]] itk::SizeValueType get_current_iteration() const;

    /// Current multi-resolution level.
    [[nodiscard]] itk::SizeValueType get_current_level() const;

    /// Current registration result.
    void get_current_matrix(data::matrix4& _trf) const;

    void convert_from_eigen_matrix(const transform_t* _itk_mat, data::matrix4& _f4s_mat) const;

    optimizer_t::Pointer m_optimizer {nullptr};
    registration_method_t::Pointer m_registrator {nullptr};
    bool m_invert {false};
};

//------------------------------------------------------------------------------

void automatic_registration::register_image(
    const data::image& _target,
    const data::image& _reference,
    data::matrix4& _trf,
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

    m_invert = compute_volume(_target) < compute_volume(_reference);

    std::reference_wrapper<const data::image> ref = _reference;
    std::reference_wrapper<const data::image> tgt = _target;

    // Always register images with the largest one being fixed.
    // Otherwise, our metric may not find any matching points between them.
    if(m_invert)
    {
        std::swap(ref, tgt);
    }

    // Convert input images to float. Integer images aren't supported yet.
    registered_image_t::Pointer target    = sight::filter::image::detail::cast_to<float>(tgt);
    registered_image_t::Pointer reference = sight::filter::image::detail::cast_to<float>(ref);

    // Choose a metric.
    switch(_metric)
    {
        case mean_squares:
            metric =
                itk::MeanSquaresImageToImageMetricv4<registered_image_t, registered_image_t, registered_image_t,
                                                     real_t>::New();
            break;

        case normalized_correlation:
            metric =
                itk::CorrelationImageToImageMetricv4<registered_image_t, registered_image_t, registered_image_t,
                                                     real_t>::New();
            break;

        case mutual_information:
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
        t[i] = _trf(i, 3);
        for(std::uint8_t j = 0 ; j < 3 ; ++j)
        {
            m(i, j) = _trf(i, j);
        }
    }

    if(m_invert)
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
    m_registrator = registration_method_t::New();
    m_optimizer   = optimizer_t::New();

    m_registrator->SetMetric(metric);
    m_registrator->SetOptimizer(m_optimizer);

    optimizer_t::ScalesType optimizer_scales(static_cast<unsigned int>(itk_transform->GetNumberOfParameters()));
    const double translation_scale = 1.0 / 1000.0;
    optimizer_scales[0] = 1.0;
    optimizer_scales[1] = 1.0;
    optimizer_scales[2] = 1.0;
    optimizer_scales[3] = translation_scale;
    optimizer_scales[4] = translation_scale;
    optimizer_scales[5] = translation_scale;

    m_optimizer->SetScales(optimizer_scales);
    m_optimizer->SetDoEstimateLearningRateAtEachIteration(true);
    m_optimizer->SetMinimumStepLength(_min_step);

    // The solution is the transform returned when optimization ends.
    m_optimizer->SetReturnBestParametersAndValue(false);
    m_optimizer->SetNumberOfIterations(_max_iterations);

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

    m_registrator->SetInitialTransform(itk_transform);
    m_registrator->SetFixedImage(target);
    m_registrator->SetMovingImage(reference);

    m_registrator->SetMetricSamplingPercentage(_sampling_percentage);

    const auto sampling_strategy = _sampling_percentage < 1.0
                                   ? registration_method_t::REGULAR : registration_method_t::NONE;

    m_registrator->SetMetricSamplingStrategy(sampling_strategy);
    m_registrator->SetNumberOfLevels(itk::SizeValueType(number_of_levels));
    m_registrator->SetSmoothingSigmasPerLevel(smoothing_sigmas_per_level);
    m_registrator->SetShrinkFactorsPerLevel(shrink_factors_per_level);
    m_registrator->SetSmoothingSigmasAreSpecifiedInPhysicalUnits(true);

    auto observer = registration_observer::New();

    if(_callback)
    {
        observer->set_callback(_callback);
        m_optimizer->AddObserver(itk::IterationEvent(), observer);
    }

    try
    {
        // Time for lift-off.
        m_registrator->Update();
        this->get_current_matrix(_trf);
    }
    catch(itk::ExceptionObject& err)
    {
        SIGHT_ERROR("Error while registering : " << err);
    }
}

//------------------------------------------------------------------------------

void automatic_registration::stop_registration() const
{
    if((m_optimizer != nullptr) && (m_registrator != nullptr))
    {
        // Stop registration by removing all levels.
        m_registrator->SetNumberOfLevels(0);
        m_optimizer->StopOptimization();
    }
}

//------------------------------------------------------------------------------

real_t automatic_registration::get_current_metric_value() const
{
    SIGHT_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetCurrentMetricValue();
}

//------------------------------------------------------------------------------

const optimizer_t::ParametersType& automatic_registration::get_current_parameters() const
{
    SIGHT_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetCurrentPosition();
}

//------------------------------------------------------------------------------

real_t automatic_registration::get_relaxation_factor() const
{
    SIGHT_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetRelaxationFactor();
}

//------------------------------------------------------------------------------

real_t automatic_registration::get_learning_rate() const
{
    SIGHT_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetLearningRate();
}

//------------------------------------------------------------------------------

real_t automatic_registration::get_gradient_magnitude_tolerance() const
{
    SIGHT_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetGradientMagnitudeTolerance();
}

//------------------------------------------------------------------------------

itk::SizeValueType automatic_registration::get_current_iteration() const
{
    SIGHT_ASSERT("No optimization process running.", m_optimizer);
    return m_optimizer->GetCurrentIteration();
}

//------------------------------------------------------------------------------

itk::SizeValueType filter::image::automatic_registration::get_current_level() const
{
    SIGHT_ASSERT("No registration process running.", m_registrator);
    return m_registrator->GetCurrentLevel();
}

//------------------------------------------------------------------------------

void automatic_registration::get_current_matrix(data::matrix4& _trf) const
{
    SIGHT_ASSERT("No registration process running.", m_registrator);
    const auto* itk_matrix = m_registrator->GetTransform();
    convert_from_eigen_matrix(itk_matrix, _trf);
}

//------------------------------------------------------------------------------

void automatic_registration::convert_from_eigen_matrix(const transform_t* _itk_mat, data::matrix4& _f4s_mat) const
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
        _f4s_mat(i, 3) = offset[i];
        for(std::uint8_t j = 0 ; j < 3 ; ++j)
        {
            _f4s_mat(i, j) = rigid_mat(i, j);
        }
    }
}

//------------------------------------------------------------------------------

void perform_automatic_registration(
    const data::image& _target,
    const data::image& _reference,
    data::matrix4& _transform,
    const registration_params_t& _params
)
{
    // Create a copy of multi_resolution_parameters without empty values
    multi_resolution_parameters_t multi_resolution_parameters(_params.multi_resolution_parameters.size());

    using param_pair_t = multi_resolution_parameters_t::value_type;

    // NOLINTNEXTLINE(modernize-use-ranges)
    auto last_elt = std::remove_copy_if(
        _params.multi_resolution_parameters.begin(),
        _params.multi_resolution_parameters.end(),
        multi_resolution_parameters.begin(),
        [](const param_pair_t& _v){return _v.first == 0;});

    multi_resolution_parameters.erase(last_elt, multi_resolution_parameters.end());

    automatic_registration registrator;
    std::fstream reg_log;

    if(_params.enable_logging)
    {
        std::stringstream file_name_stream;
        const std::time_t system_time = std::time(nullptr);
        file_name_stream << "registration_"
        << std::put_time(std::localtime(&system_time), "%Y-%m-%d_%H-%M-%S") << ".csv";

        reg_log.open(file_name_stream.str(), std::ios_base::out);
        reg_log << "'Timestamp',"
        << "'Current level',"
        << "'Current iteration',"
        << "'Shrink',"
        << "'Sigma',"
        << "'Current metric value',"
        << "'Current parameters',"
        << "'Current transform',"
        << "'Relaxation factor',"
        << "'Learning rate',"
        << "'Gradient magnitude tolerance',"
        << "'Minimum step size',"
        << "'Maximum number of iterations',"
        << "'Sampling rate',"
        << "'Number of levels'"
        << std::endl;
    }

    std::chrono::time_point<std::chrono::system_clock> reg_start_time;
    std::size_t i = 0;

    automatic_registration::iteration_callback_t iteration_callback =
        [&]()
        {
            const itk::SizeValueType current_iteration = registrator.get_current_iteration();
            const itk::SizeValueType current_level     = registrator.get_current_level();

            const float progress = float(i++) / float(_params.max_iterations * multi_resolution_parameters.size());

            const std::string msg = "Number of iterations : " + std::to_string(i) + " Current level : "
                                    + std::to_string(current_level);
            SIGHT_INFO(progress << "% " << msg);

            registrator.get_current_matrix(_transform);

            if(_params.enable_logging)
            {
                std::stringstream transform_stream;

                for(std::uint8_t j = 0 ; j < 16 ; ++j)
                {
                    transform_stream << _transform[j];

                    if(j != 15)
                    {
                        transform_stream << ";";
                    }
                }

                const std::chrono::time_point<std::chrono::system_clock> now =
                    std::chrono::system_clock::now();

                const auto duration = now - reg_start_time;

                reg_log << "'" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "',"
                << "'" << current_level << "',"
                << "'" << current_iteration << "',"
                << "'" << multi_resolution_parameters[current_level].first << "',"
                << "'" << multi_resolution_parameters[current_level].second << "',"
                << "'" << registrator.get_current_metric_value() << "',"
                << "'" << registrator.get_current_parameters() << "',"
                << "'" << transform_stream.str() << "',"
                << "'" << registrator.get_relaxation_factor() << "',"
                << "'" << registrator.get_learning_rate() << "',"
                << "'" << registrator.get_gradient_magnitude_tolerance() << "',"
                << "'" << _params.min_step << "',"
                << "'" << _params.max_iterations << "',"
                << "'" << _params.sampling_percentage << "',"
                << "'" << multi_resolution_parameters.size() << "'"
                << std::endl;

                reg_log.flush(); // Flush, just to be sure.
            }
        };

    try
    {
        registrator.register_image(
            _target,
            _reference,
            _transform,
            _params.metric,
            multi_resolution_parameters,
            _params.sampling_percentage,
            _params.min_step,
            _params.max_iterations,
            iteration_callback
        );
    }
    catch(itk::ExceptionObject& e)
    {
        SIGHT_ERROR("[ITK EXCEPTION]" << e.GetDescription());
    }
}

} // namespace sight::filter::image
