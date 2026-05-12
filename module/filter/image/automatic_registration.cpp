/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
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

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

automatic_registration::automatic_registration() :
    filter(has_signals::signals()),
    has_parameters(has_slots::slots())
{
}

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
    const auto target    = m_target.lock();
    const auto reference = m_reference.lock();

    auto transform = m_transform.lock();

    SIGHT_ASSERT("No " << TARGET_IN << " found !", target);
    SIGHT_ASSERT("No " << REFERENCE_IN << " found !", reference);
    SIGHT_ASSERT("No " << TRANSFORM_INOUT << " found !", transform);

    sight::filter::image::registration_params_t params =
    {
        .multi_resolution_parameters = m_multi_resolution_parameters,
        .max_iterations              = m_max_iterations,
        .min_step                    = m_min_step,
        .sampling_percentage         = m_sampling_percentage,
        .metric                      = m_metric,
        .enable_logging              = m_log
    };
    sight::filter::image::perform_automatic_registration(*target, *reference, *transform, params);

    this->signal<signals::computed_t>(signals::SUCCEEDED)->async_emit();
    transform->async_emit(sight::data::signals::MODIFIED);
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
        m_multi_resolution_parameters[level].first = static_cast<std::size_t>(_val);
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
