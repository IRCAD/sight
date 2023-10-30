/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include "filter/image/powell_optimizer.hpp"

#include <core/spy_log.hpp>

namespace sight::filter::image
{

// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor): used via pointers
class powell_metric : public powell_optimizer::powell_optimizer_t::MetricType
{
public:

    using self_t     = powell_metric;
    using superclass = powell_optimizer::powell_optimizer_t::MetricType;
    using Pointer    = itk::SmartPointer<self_t>;
    itkNewMacro(self_t);

    //-----------------------------------------------------------------------------

    void Initialize() override
    {
    }

    //-----------------------------------------------------------------------------

    void set_function(powell_optimizer::optimized_function_t _f)
    {
        m_function = _f;
    }

    //-----------------------------------------------------------------------------

    void SetParameters(ParametersType& _params) override
    {
        m_parameters = _params;
    }

    //-----------------------------------------------------------------------------

    NumberOfParametersType GetNumberOfParameters() const override
    {
        return static_cast<NumberOfParametersType>(m_parameters.size());
    }

    //-----------------------------------------------------------------------------

    MeasureType GetValue() const override
    {
        m_Value = m_function(m_parameters);
        return m_Value;
    }

    //-----------------------------------------------------------------------------

    void GetDerivative(DerivativeType& /*unused*/) const override
    {
        // Unused in Powell's method.
    }

    //-----------------------------------------------------------------------------

    void GetValueAndDerivative(MeasureType& /*value*/, DerivativeType& /*derivative*/) const override
    {
    }

    //-----------------------------------------------------------------------------

    NumberOfParametersType GetNumberOfLocalParameters() const override
    {
        return 0;
    }

    //-----------------------------------------------------------------------------

    const ParametersType& GetParameters() const override
    {
        return m_parameters;
    }

    //-----------------------------------------------------------------------------

    bool HasLocalSupport() const override
    {
        return false;
    }

    //-----------------------------------------------------------------------------

    void UpdateTransformParameters(const DerivativeType& /*derivative*/, ParametersValueType /*factor*/) override
    {
    }

//-----------------------------------------------------------------------------

private:

    powell_metric()
    = default;

    ~powell_metric() override
    = default;

    ParametersType m_parameters;

    powell_optimizer::optimized_function_t m_function;
};

//-----------------------------------------------------------------------------

powell_optimizer::powell_optimizer(
    const powell_optimizer::optimized_function_t& _f,
    double _step_tolerance,
    double _value_tolerance,
    double _step_length,
    std::uint32_t _maximum_iterations
) :
    m_function(_f)
{
    m_powell_optimizer = powell_optimizer_t::New();
    m_powell_optimizer->SetStepTolerance(_step_tolerance);
    m_powell_optimizer->SetValueTolerance(_value_tolerance);
    m_powell_optimizer->SetStepLength(_step_length);
    m_powell_optimizer->SetMaximumIteration(_maximum_iterations);
}

//-----------------------------------------------------------------------------

powell_optimizer::function_parameters_t powell_optimizer::optimize(
    const powell_optimizer::function_parameters_t& _init_parameters
)
{
    auto metric = powell_metric::New();
    metric->set_function(m_function);
    function_parameters_t init_params(_init_parameters);
    metric->SetParameters(init_params);

    m_powell_optimizer->SetMetric(metric);

    try
    {
        m_powell_optimizer->StartOptimization();
    }
    catch(itk::ExceptionObject& e)
    {
        SIGHT_ERROR("Powell optimizer exception : " << e);
    }

    return metric->GetParameters();
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image.
