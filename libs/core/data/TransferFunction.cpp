/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/com/Signal.hxx>
#include <core/Type.hpp>

#include <glm/common.hpp>

SIGHT_REGISTER_DATA(sight::data::TransferFunction)

namespace sight::data
{

//------------------------------------------------------------------------------

const std::string TransferFunction::s_DEFAULT_TF_NAME = "GreyLevel";

const core::com::Signals::SignalKeyType TransferFunction::s_POINTS_MODIFIED_SIG    = "pointsModified";
const core::com::Signals::SignalKeyType TransferFunction::s_WINDOWING_MODIFIED_SIG = "windowingModified";

//------------------------------------------------------------------------------

TransferFunctionData::min_max_t TransferFunctionData::minMax() const
{
    SIGHT_ASSERT("It must have at least one value.", !this->empty());
    min_max_t minMax;
    minMax.first  = this->begin()->first;
    minMax.second = (this->rbegin())->first;
    return minMax;
}

//------------------------------------------------------------------------------

TransferFunctionData::min_max_t TransferFunctionData::windowMinMax() const
{
    min_max_t minMax;
    const value_t halfWindow = m_window / 2.;
    minMax.first  = m_level - halfWindow;
    minMax.second = m_level + halfWindow;
    return minMax;
}

//------------------------------------------------------------------------------

void TransferFunctionData::setWindowMinMax(const min_max_t& _minMax)
{
    this->setWindow(_minMax.second - _minMax.first);

    const value_t halfWindow = m_window / 2.;
    this->setLevel(halfWindow + _minMax.first);
}

//------------------------------------------------------------------------------

TransferFunctionData::color_t TransferFunctionData::sampleNearest(value_t _value) const
{
    return sample(_value, InterpolationMode::NEAREST);
}

//------------------------------------------------------------------------------

TransferFunctionData::color_t TransferFunctionData::sampleLinear(value_t _value) const
{
    return sample(_value, InterpolationMode::LINEAR);
}

//-----------------------------------------------------------------------------

TransferFunctionData::color_t TransferFunctionData::sample(value_t _value, std::optional<InterpolationMode> _mode) const
{
    SIGHT_ASSERT("It must have at least one value.", !empty());

    const value_t value = this->mapValueFromWindow(_value);

    constexpr value_t min = std::numeric_limits<value_t>::min();
    constexpr value_t max = std::numeric_limits<value_t>::max();
    value_t previousValue = min;
    value_t nextValue     = max;

    const color_t blackColor(0.0);
    color_t color(0.0);
    color_t previousColor = blackColor;
    color_t nextColor     = blackColor;

    for(const data_t::value_type& data : *this)
    {
        if(value < data.first)
        {
            nextValue = data.first;
            nextColor = data.second;
            break;
        }
        else
        {
            previousValue = data.first;
            previousColor = data.second;
        }
    }

    if(previousValue == min)
    {
        if(m_clamped)
        {
            color = blackColor;
        }
        else
        {
            color = nextColor;
        }
    }
    else if(nextValue == max)
    {
        if(m_clamped && !sight::core::tools::is_equal(value, previousValue))
        {
            color = blackColor;
        }
        else
        {
            color = previousColor;
        }
    }
    else
    {
        const InterpolationMode mode = _mode == std::nullopt ? m_interpolationMode : _mode.value();
        switch(mode)
        {
            case TransferFunctionData::InterpolationMode::LINEAR:
            {
                // Interpolate the color.
                const value_t distanceToNextValue     = nextValue - value;
                const value_t distanceToPreviousValue = value - previousValue;
                const value_t distance                = 1.0 / (nextValue - previousValue);
                const value_t coefPrevious            = 1.0 - (distanceToPreviousValue * distance);
                const value_t coefNext                = 1.0 - (distanceToNextValue * distance);

                color = coefPrevious * previousColor + coefNext * nextColor;
                break;
            }

            case TransferFunctionData::InterpolationMode::NEAREST:
            {
                if((value - previousValue) < (nextValue - value))
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
    }

    return color;
}

//-----------------------------------------------------------------------------

void TransferFunctionData::setLevel(TransferFunctionData::value_t _value)
{
    m_level = _value;
}

//-----------------------------------------------------------------------------

void TransferFunctionData::setWindow(TransferFunctionData::value_t _value)
{
    m_window = _value;
}

//------------------------------------------------------------------------------

data::TransferFunction::sptr TransferFunction::createDefaultTF()
{
    TransferFunction::sptr tf = TransferFunction::New();

    tf->setName(TransferFunction::s_DEFAULT_TF_NAME);
    tf->setClamped(false);

    auto& pieces = tf->pieces();
    pieces.push_back(data::TransferFunctionData::New());
    const auto& firstPiece = pieces.front();
    firstPiece->insert({0.0, color_t()});
    firstPiece->insert({1.0, color_t(1.0, 1.0, 1.0, 1.0)});
    firstPiece->setClamped(false);
    firstPiece->setWindow(500.);
    firstPiece->setLevel(50.);

    tf->mergePieces();

    return tf;
}

//------------------------------------------------------------------------------

TransferFunction::TransferFunction(data::Object::Key)
{
    newSignal<PointsModifiedSignalType>(s_POINTS_MODIFIED_SIG);
    newSignal<WindowingModifiedSignalType>(s_WINDOWING_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

void TransferFunction::shallowCopy(const Object::csptr& _source)
{
    const auto other = TransferFunction::dynamicCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>")) + " to " + getClassname()
        ),
        !bool(other)
    );

    TransferFunctionData::operator=(*other);

    this->m_name            = other->m_name;
    this->m_backgroundColor = other->m_backgroundColor;

    this->m_pieces.clear();
    std::copy(other->m_pieces.cbegin(), other->m_pieces.cend(), std::back_inserter(this->m_pieces));

    fieldShallowCopy(_source);
}

//------------------------------------------------------------------------------

void TransferFunction::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache)
{
    TransferFunction::csptr other = TransferFunction::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    TransferFunctionData::operator=(*other);

    this->m_name            = other->m_name;
    this->m_backgroundColor = other->m_backgroundColor;

    this->m_pieces.clear();
    std::copy(other->m_pieces.cbegin(), other->m_pieces.cend(), std::back_inserter(this->m_pieces));

    this->fieldDeepCopy(_source, _cache);
}

//------------------------------------------------------------------------------

bool TransferFunction::operator==(const TransferFunction& _other) const noexcept
{
    if(m_name != _other.m_name
       || !core::tools::is_equal(m_backgroundColor, _other.m_backgroundColor)
       || !TransferFunctionData::operator==(_other))
    {
        return false;
    }

    // Super class last
    return Object::operator==(_other);
}

//------------------------------------------------------------------------------

bool TransferFunction::operator!=(const TransferFunction& _other) const noexcept
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

sight::data::TransferFunction::color_t mergeColors(
    const sight::data::TransferFunction& _tf,
    sight::data::TransferFunction::value_t _value
)
{
    sight::data::TransferFunction::color_t result(0.);
    std::vector<sight::data::TransferFunction::color_t> colors;
    colors.reserve(_tf.pieces().size());

    // 1. Determine the maximum of opacity for this value
    for(const auto& piece : _tf.pieces())
    {
        sight::data::TransferFunction::color_t color = piece->sample(_value);
        result.a = std::max(result.a, color.a);
        colors.push_back(color);
    }

    result.a = std::clamp(result.a, 0., 1.);

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

void TransferFunction::mergePieces()
{
    if(this->m_pieces.empty())
    {
        return;
    }

    this->clear();

    bool clamped = false;

    for(const auto& piece : this->m_pieces)
    {
        const auto addTFPoint = [&](sight::data::TransferFunction::value_t _value, double _delta)
                                {
                                    sight::data::TransferFunction::value_t value = piece->mapValueToWindow(_value);
                                    value         += _delta;
                                    (*this)[value] = mergeColors(*this, value);
                                };

        sight::data::TransferFunction::value_t previousValue = 0.;

        // Add new TF value to the output.
        for(const auto& elt : *piece)
        {
            // If the TF interpolation mode is not linear, we create new point in the merged TF.
            if(piece->interpolationMode() == sight::data::TransferFunction::InterpolationMode::NEAREST
               && elt != *piece->begin())
            {
                sight::data::TransferFunction::value_t middleValue = previousValue + (elt.first - previousValue) / 2.;
                addTFPoint(middleValue, -.5);
                addTFPoint(middleValue, .5);
            }

            // If the TF is clamped, we create a new point in the merged TF.
            if(piece->clamped())
            {
                if(elt == *piece->begin())
                {
                    addTFPoint(elt.first, -0.5);
                }
                else if(elt == *piece->rbegin())
                {
                    addTFPoint(elt.first, 0.5);
                }
            }

            addTFPoint(elt.first, 0.);
            previousValue = elt.first;
        }
    }

    this->setClamped(clamped);

    // Iterates over each TF to merge them in the output one.
    const value_t min = this->begin()->first;
    const value_t max = this->rbegin()->first;

    // Updates the window/level.
    m_window = std::abs(max - min);
    m_level  = std::min(min, max) + m_window * .5;
}

//-----------------------------------------------------------------------------

void TransferFunction::setLevel(TransferFunctionData::value_t _value)
{
    const double delta = _value - this->level();
    for(auto& piece : this->pieces())
    {
        piece->setLevel(piece->level() + delta);
    }

    m_level = _value;
}

//-----------------------------------------------------------------------------

void TransferFunction::setWindow(TransferFunctionData::value_t _value)
{
    const double scale = _value / this->window();
    for(auto& piece : this->pieces())
    {
        piece->setWindow(piece->window() * scale);
    }

    m_window = _value;
}

//------------------------------------------------------------------------------

} // end namespace sight::data
