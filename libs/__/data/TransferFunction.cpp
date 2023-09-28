/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/TransferFunction.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/signal.hxx>
#include <core/type.hpp>

#include <glm/common.hpp>

SIGHT_REGISTER_DATA(sight::data::TransferFunction)

namespace sight::data
{

//------------------------------------------------------------------------------

const std::string TransferFunction::s_DEFAULT_TF_NAME = "GreyLevel";

const core::com::signals::key_t TransferFunction::POINTS_MODIFIED_SIG    = "pointsModified";
const core::com::signals::key_t TransferFunction::WINDOWING_MODIFIED_SIG = "windowingModified";

//------------------------------------------------------------------------------

transfer_function_base::value_t transfer_function_base::mapValueToWindow(value_t _value) const
{
    const min_max_t minMax       = this->minMax();
    const min_max_t windowMinMax = this->windowMinMax();

    const value_t scale = window() / (minMax.second - minMax.first);
    const value_t value = (_value - minMax.first) * scale + windowMinMax.first;

    return value;
}

//------------------------------------------------------------------------------

transfer_function_base::value_t transfer_function_base::mapValueFromWindow(value_t _value) const
{
    const min_max_t minMax       = this->minMax();
    const min_max_t windowMinMax = this->windowMinMax();

    const value_t scale = (minMax.second - minMax.first) / window();
    const value_t value = (_value - windowMinMax.first) * scale + minMax.first;

    return value;
}

//------------------------------------------------------------------------------

transfer_function_base::color_t transfer_function_base::sampleNearest(value_t _value) const
{
    return sample(_value, InterpolationMode::NEAREST);
}

//------------------------------------------------------------------------------

transfer_function_base::color_t transfer_function_base::sampleLinear(value_t _value) const
{
    return sample(_value, InterpolationMode::LINEAR);
}

//------------------------------------------------------------------------------

TransferFunctionPiece::min_max_t transfer_function_base::windowMinMax() const
{
    min_max_t minMax;
    const value_t halfWindow = this->window() / 2.;

    const value_t level = this->level();
    minMax.first  = level - halfWindow;
    minMax.second = level + halfWindow;
    return minMax;
}

//------------------------------------------------------------------------------

void transfer_function_base::setWindowMinMax(const min_max_t& _minMax)
{
    this->setWindow(
        _minMax.second
        >= _minMax.first ? std::max(1., _minMax.second - _minMax.first) : std::min(
            -1.,
            _minMax.second - _minMax.first
        )
    );

    const value_t halfWindow = window() * 0.5;
    this->setLevel(halfWindow + _minMax.first);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

TransferFunctionPiece::min_max_t TransferFunctionPiece::minMax() const
{
    SIGHT_ASSERT("It must have at least one value.", !this->empty());
    min_max_t minMax;
    minMax.first  = this->begin()->first;
    minMax.second = (this->rbegin())->first;
    return minMax;
}

//------------------------------------------------------------------------------

TransferFunctionPiece::color_t TransferFunctionPiece::sample(
    value_t _value,
    std::optional<InterpolationMode> _mode
) const
{
    SIGHT_ASSERT("It must have at least one value.", !empty());

    const value_t value = this->mapValueFromWindow(_value);

    const color_t blackColor(0.0);

    value_t previousValue = value;
    color_t previousColor = m_clamped ? blackColor : this->begin()->second;

    value_t nextValue = value;
    color_t nextColor = m_clamped ? blackColor : this->rbegin()->second;

    color_t color(0.0);

    bool found = false;
    for(const data_t::value_type& data : *this)
    {
        nextValue = data.first;
        nextColor = data.second;

        if(value < data.first)
        {
            found = true;
            break;
        }

        previousValue = nextValue;
        previousColor = nextColor;
    }

    if(!found)
    {
        nextValue = value;
        nextColor = m_clamped ? blackColor : this->rbegin()->second;
    }

    const InterpolationMode mode = _mode == std::nullopt ? m_interpolationMode : _mode.value();
    switch(mode)
    {
        case TransferFunctionPiece::InterpolationMode::LINEAR:
        {
            const value_t distance    = nextValue - previousValue;
            const value_t interpolant = distance < 1e-5 ? 0. : (value - previousValue) / distance;
            color = glm::mix(previousColor, nextColor, interpolant);
            break;
        }

        case TransferFunctionPiece::InterpolationMode::NEAREST:
        {
            if((value - previousValue) <= (nextValue - value))
            {
                color = previousColor;
            }
            else
            {
                color = nextColor;
            }

            break;
        }

        default:
            SIGHT_ASSERT("Unreachable code, undefined interpolation mode", false);
    }

    return color;
}

//-----------------------------------------------------------------------------

void TransferFunctionPiece::setLevel(TransferFunctionPiece::value_t _value)
{
    m_level = _value;
}

//-----------------------------------------------------------------------------

void TransferFunctionPiece::setWindow(TransferFunctionPiece::value_t _value)
{
    m_window = _value;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

data::TransferFunction::sptr TransferFunction::createDefaultTF()
{
    TransferFunction::sptr tf = std::make_shared<TransferFunction>();

    tf->setName(TransferFunction::s_DEFAULT_TF_NAME);

    auto tfData = tf->pieces().emplace_back(std::make_shared<data::TransferFunctionPiece>());
    tfData->insert({0.0, color_t()});
    tfData->insert({1.0, color_t(1.0, 1.0, 1.0, 1.0)});
    tfData->setClamped(false);
    tfData->setWindow(500.);
    tfData->setLevel(50.);

    tf->fitWindow();

    return tf;
}

//------------------------------------------------------------------------------

data::TransferFunction::sptr TransferFunction::createDefaultTF(core::type _type)
{
    TransferFunction::sptr tf = std::make_shared<TransferFunction>();

    tf->setName(TransferFunction::s_DEFAULT_TF_NAME);

    auto tfData = tf->pieces().emplace_back(std::make_shared<data::TransferFunctionPiece>());
    tfData->insert({0.0, color_t()});
    tfData->insert({1.0, color_t(1.0, 1.0, 1.0, 1.0)});
    tfData->setClamped(false);
    if(_type == core::type::INT8 || _type == core::type::UINT8)
    {
        tfData->setWindow(255.);
        tfData->setLevel(127.);
    }
    else
    {
        tfData->setWindow(500.);
        tfData->setLevel(50.);
    }

    tf->fitWindow();

    return tf;
}

//------------------------------------------------------------------------------

TransferFunction::TransferFunction()
{
    new_signal<PointsModifiedSignalType>(POINTS_MODIFIED_SIG);
    new_signal<WindowingModifiedSignalType>(WINDOWING_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

void TransferFunction::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const TransferFunction>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_name            = other->m_name;
    m_backgroundColor = other->m_backgroundColor;

    m_level  = other->m_level;
    m_window = other->m_window;

    m_pieces.clear();
    std::copy(other->m_pieces.cbegin(), other->m_pieces.cend(), std::back_inserter(m_pieces));

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void TransferFunction::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const TransferFunction>(source);

    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_name            = other->m_name;
    m_backgroundColor = other->m_backgroundColor;

    m_level  = other->m_level;
    m_window = other->m_window;

    m_pieces.clear();

    std::transform(
        other->m_pieces.cbegin(),
        other->m_pieces.cend(),
        std::back_inserter(this->m_pieces),
        [&](const auto& value)
        {
            auto data = std::make_shared<TransferFunctionPiece>();
            *data     = *value;
            return data;
        });

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

bool TransferFunction::operator==(const TransferFunction& _other) const noexcept
{
    if(m_name != _other.m_name
       || !core::tools::is_equal(m_backgroundColor, _other.m_backgroundColor)
       || m_pieces.size() != _other.m_pieces.size())
    {
        return false;
    }

    // test each piece in m_pieces

    auto a_it = m_pieces.cbegin();
    auto b_it = _other.m_pieces.cbegin();

    const auto a_end = m_pieces.cend();

    while(a_it != a_end)
    {
        if(**a_it != **b_it)
        {
            return false;
        }

        ++a_it;
        ++b_it;
    }

    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool TransferFunction::operator!=(const TransferFunction& _other) const noexcept
{
    return !(*this == _other);
}

//-----------------------------------------------------------------------------

void TransferFunction::fitWindow()
{
    min_max_t minMax {std::numeric_limits<value_t>::max(), std::numeric_limits<value_t>::lowest()};

    for(auto& piece : this->pieces())
    {
        const auto pieceMinMax = piece->minMax();
        minMax.first  = std::min(piece->mapValueToWindow(pieceMinMax.first), minMax.first);
        minMax.second = std::max(piece->mapValueToWindow(pieceMinMax.second), minMax.second);
    }

    // Updates the window/level.
    m_window = minMax.second - minMax.first;
    m_level  = minMax.first + m_window * .5;
}

//------------------------------------------------------------------------------

void TransferFunction::setLevel(value_t _value)
{
    const double delta = _value - this->level();
    for(auto& piece : this->pieces())
    {
        piece->setLevel(piece->level() + delta);
    }

    m_level = _value;
}

//-----------------------------------------------------------------------------

void TransferFunction::setWindow(value_t _value)
{
    SIGHT_ASSERT("Window should be non-null", std::fpclassify(_value) != FP_ZERO);

    const double scale = _value / this->window();
    for(auto& piece : this->pieces())
    {
        piece->setWindow(piece->window() * scale);
    }

    m_window = _value;
}

//------------------------------------------------------------------------------

TransferFunctionPiece::min_max_t TransferFunction::minMax() const
{
    if(empty())
    {
        return {-1., 1.};
    }

    min_max_t minMax {std::numeric_limits<value_t>::max(), std::numeric_limits<value_t>::lowest()};

    for(const auto& piece : this->pieces())
    {
        const auto pieceMinMax = piece->minMax();
        minMax.first  = std::min(piece->mapValueToWindow(pieceMinMax.first), minMax.first);
        minMax.second = std::max(piece->mapValueToWindow(pieceMinMax.second), minMax.second);
    }

    return minMax;
}

//------------------------------------------------------------------------------

TransferFunctionPiece::color_t TransferFunction::sample(value_t _value, std::optional<InterpolationMode> _mode) const
{
    SIGHT_ASSERT("It must have at least one value.", !empty());

    transfer_function_base::color_t result(0.);
    std::vector<transfer_function_base::color_t> colors;
    colors.reserve(this->pieces().size());

    // 1. Determine the maximum of opacity for this value
    for(const auto& piece : this->pieces())
    {
        transfer_function_base::color_t color = piece->sample(_value, _mode);
        result.a = std::max(result.a, color.a);
        colors.push_back(color);
    }

    result.a = std::min(result.a, 1.);

    // 2. Mix the colors by the ratio of the opacity and the maximum opacity
    for(const auto& color : colors)
    {
        const double ratio = result.a == 0. ? 1.0 : color.a / result.a;
        result.r += color.r * ratio;
        result.g += color.g * ratio;
        result.b += color.b * ratio;
    }

    return glm::min(result, glm::dvec4(1.));
}

//------------------------------------------------------------------------------

} // end namespace sight::data
