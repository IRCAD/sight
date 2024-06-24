/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include "automatic_registration.hpp"

#include <service/macros.hpp>

#include <ui/__/dialog/progress.hpp>

#include <chrono>
#include <fstream>
#include <iomanip>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

automatic_registration::automatic_registration() :
    filter(m_signals),
    has_parameters(m_slots)
{
}

//------------------------------------------------------------------------------

automatic_registration::~automatic_registration()
= default;

//------------------------------------------------------------------------------

void automatic_registration::configuring()
{
    service::config_t config = this->get_config();

    m_min_step = config.get<double>("minStep", -1.);

    SIGHT_FATAL_IF("Invalid or missing minStep.", m_min_step <= 0);

    m_max_iterations = config.get<std::uint64_t>("maxIterations", 0);

    SIGHT_FATAL_IF("Invalid or missing number of iterations.", m_max_iterations == 0);

    const std::string metric = config.get<std::string>("metric", "");
    this->set_metric(metric);

    const std::string shrink_list = config.get<std::string>("levels", "");
    std::string sigma_shrink_pair;

    std::istringstream shrinks_stream(shrink_list);
    while(std::getline(shrinks_stream, sigma_shrink_pair, ';'))
    {
        std::istringstream sigma_shrink_stream(sigma_shrink_pair);
        std::vector<std::string> parameters;
        std::string token;

        while(std::getline(sigma_shrink_stream, token, ':'))
        {
            parameters.push_back(token);
        }

        SIGHT_ASSERT("There must be two parameters: shrink and sigma.", parameters.size() == 2);

        const std::uint64_t shrink = std::stoul(parameters[0]);
        const double sigma         = std::stod(parameters[1]);

        m_multi_resolution_parameters.emplace_back(shrink, sigma);
    }

    if(m_multi_resolution_parameters.empty())
    {
        // By default, no multi-resolution
        m_multi_resolution_parameters.emplace_back(1, 0.0);
    }

    m_sampling_percentage = config.get<double>("samplingPercentage", 1.);

    m_log = config.get<bool>("log", false);
}

//------------------------------------------------------------------------------

void automatic_registration::starting()
{
}

//------------------------------------------------------------------------------

void automatic_registration::updating()
{
    using sight::filter::image::automatic_registration;

    const auto target    = m_target.lock();
    const auto reference = m_reference.lock();

    auto transform = m_transform.lock();

    SIGHT_ASSERT("No " << TARGET_IN << " found !", target);
    SIGHT_ASSERT("No " << REFERENCE_IN << " found !", reference);
    SIGHT_ASSERT("No " << TRANSFORM_INOUT << " found !", transform);

    // Create a copy of m_multiResolutionParameters without empty values
    automatic_registration::multi_resolution_parameters_t
        multi_resolution_parameters(m_multi_resolution_parameters.size());

    using param_pair_t = automatic_registration::multi_resolution_parameters_t::value_type;

    auto last_elt = std::remove_copy_if(
        m_multi_resolution_parameters.begin(),
        m_multi_resolution_parameters.end(),
        multi_resolution_parameters.begin(),
        [](const param_pair_t& _v){return _v.first == 0;});

    multi_resolution_parameters.erase(last_elt, multi_resolution_parameters.end());

    automatic_registration registrator;

    sight::ui::dialog::progress dialog("Automatic Registration", "Registering, please be patient.");

    dialog.set_cancel_callback(
        [&registrator]()
        {
            registrator.stop_registration();
        });

    std::fstream reg_log;

    if(m_log)
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

    auto transfo_modified_sig = transform->signal<data::matrix4::modified_signal_t>
                                    (data::matrix4::MODIFIED_SIG);

    std::chrono::time_point<std::chrono::system_clock> reg_start_time;
    std::size_t i = 0;

    automatic_registration::iteration_callback_t iteration_callback =
        [&]()
        {
            const itk::SizeValueType current_iteration = registrator.get_current_iteration();
            const itk::SizeValueType current_level     = registrator.get_current_level();

            const float progress = float(i++) / float(m_max_iterations * multi_resolution_parameters.size());

            std::string msg = "Number of iterations : " + std::to_string(i) + " Current level : "
                              + std::to_string(current_level);
            dialog(progress, msg);
            dialog.set_message(msg);

            registrator.get_current_matrix(transform.get_shared());

            if(m_log)
            {
                std::stringstream transform_stream;

                for(std::uint8_t j = 0 ; j < 16 ; ++j)
                {
                    transform_stream << (*transform)[j];

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
                << "'" << m_min_step << "',"
                << "'" << m_max_iterations << "',"
                << "'" << m_sampling_percentage << "',"
                << "'" << multi_resolution_parameters.size() << "'"
                << std::endl;

                reg_log.flush(); // Flush, just to be sure.
            }

            transfo_modified_sig->async_emit();
        };

    try
    {
        registrator.register_image(
            target.get_shared(),
            reference.get_shared(),
            transform.get_shared(),
            m_metric,
            multi_resolution_parameters,
            m_sampling_percentage,
            m_min_step,
            m_max_iterations,
            iteration_callback
        );
    }
    catch(itk::ExceptionObject& e)
    {
        SIGHT_ERROR("[ITK EXCEPTION]" << e.GetDescription());
    }

    this->signal<signals::computed_t>(signals::COMPUTED)->async_emit();
    transfo_modified_sig->async_emit();
}

//------------------------------------------------------------------------------

void automatic_registration::stopping()
{
}

//------------------------------------------------------------------------------

service::connections_t automatic_registration::auto_connections() const
{
    return {
        {TARGET_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {TARGET_IN, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {REFERENCE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {REFERENCE_IN, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {TRANSFORM_INOUT, data::matrix4::MODIFIED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

void automatic_registration::set_enum_parameter(std::string _val, std::string _key)
{
    if(_key == "metric")
    {
        set_metric(_val);
    }
    else
    {
        SIGHT_FATAL("Key must be 'metric', unknown key :" << _key);
    }
}

//------------------------------------------------------------------------------

void automatic_registration::set_double_parameter(double _val, std::string _key)
{
    if(_key == "minStep")
    {
        m_min_step = _val;
    }
    else if(_key.find("sigma_") != std::string::npos)
    {
        const std::uint64_t level = this->extract_level_from_parameter_name(_key);
        m_multi_resolution_parameters[level].second = _val;
    }
    else if(_key == "samplingPercentage")
    {
        m_sampling_percentage = _val;
    }
    else
    {
        SIGHT_FATAL("Unknown key : " << _key);
    }
}

//------------------------------------------------------------------------------

void automatic_registration::set_int_parameter(int _val, std::string _key)
{
    if(_key == "maxIterations")
    {
        SIGHT_FATAL_IF("The number of iterations must be greater than 0 !!", _val <= 0);
        m_max_iterations = static_cast<std::uint64_t>(_val);
    }
    else if(_key.find("shrink_") != std::string::npos)
    {
        const std::uint64_t level = this->extract_level_from_parameter_name(_key);
        m_multi_resolution_parameters[level].first = itk::SizeValueType(_val);
    }
    else
    {
        SIGHT_FATAL("Unknown key : " << _key);
    }
}

//------------------------------------------------------------------------------
std::uint64_t automatic_registration::extract_level_from_parameter_name(const std::string& _name)
{
    // find the level
    const std::string level_suffix = _name.substr(_name.find('_') + 1);
    const std::uint64_t level      = std::stoul(level_suffix);

    if(level >= m_multi_resolution_parameters.size())
    {
        m_multi_resolution_parameters.resize(level + 1, std::make_pair(0, 0.0));
    }

    return level;
}

//------------------------------------------------------------------------------

void automatic_registration::set_metric(const std::string& _metric_name)
{
    if(_metric_name == "MeanSquares")
    {
        m_metric = sight::filter::image::mean_squares;
    }
    else if(_metric_name == "NormalizedCorrelation")
    {
        m_metric = sight::filter::image::normalized_correlation;
    }
    else if(_metric_name == "MutualInformation")
    {
        m_metric = sight::filter::image::mutual_information;
    }
    else
    {
        SIGHT_FATAL("Unknown metric: " << _metric_name);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image
