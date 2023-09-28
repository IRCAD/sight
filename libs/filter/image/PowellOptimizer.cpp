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

#include "filter/image/PowellOptimizer.hpp"

#include <core/spy_log.hpp>

namespace sight::filter::image
{

// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor): used via pointers
class PowellMetric : public PowellOptimizer::PowellOptimizerType::MetricType
{
public:

    using Self         = PowellMetric;
    using Superclass   = PowellOptimizer::PowellOptimizerType::MetricType;
    using Pointer      = itk::SmartPointer<Self>;
    using ConstPointer = itk::SmartPointer<const Self>;
    itkNewMacro(Self);

    //-----------------------------------------------------------------------------

    void Initialize() override
    {
    }

    //-----------------------------------------------------------------------------

    void setFunction(PowellOptimizer::OptimizedFunctionType _f)
    {
        m_function = _f;
    }

    //-----------------------------------------------------------------------------

    void SetParameters(ParametersType& params) override
    {
        m_parameters = params;
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

    PowellMetric()
    = default;

    ~PowellMetric() override
    = default;

    ParametersType m_parameters;

    PowellOptimizer::OptimizedFunctionType m_function;
};

//-----------------------------------------------------------------------------

PowellOptimizer::PowellOptimizer(
    const PowellOptimizer::OptimizedFunctionType& _f,
    double _stepTolerance,
    double _valueTolerance,
    double _stepLength,
    std::uint32_t _maximumIterations
) :
    m_function(_f)
{
    m_powellOptimizer = PowellOptimizerType::New();
    m_powellOptimizer->SetStepTolerance(_stepTolerance);
    m_powellOptimizer->SetValueTolerance(_valueTolerance);
    m_powellOptimizer->SetStepLength(_stepLength);
    m_powellOptimizer->SetMaximumIteration(_maximumIterations);
}

//-----------------------------------------------------------------------------

PowellOptimizer::FunctionParametersType PowellOptimizer::optimize(
    const PowellOptimizer::FunctionParametersType& _initParameters
)
{
    PowellMetric::Pointer metric = PowellMetric::New();
    metric->setFunction(m_function);
    FunctionParametersType initParams(_initParameters);
    metric->SetParameters(initParams);

    m_powellOptimizer->SetMetric(metric);

    try
    {
        m_powellOptimizer->StartOptimization();
    }
    catch(itk::ExceptionObject& e)
    {
        SIGHT_ERROR("Powell optimizer exception : " << e);
    }

    return metric->GetParameters();
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image.
