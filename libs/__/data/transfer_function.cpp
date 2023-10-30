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

#include "data/transfer_function.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/signal.hxx>
#include <core/type.hpp>

#include <glm/common.hpp>

SIGHT_REGISTER_DATA(sight::data::transfer_function)

namespace sight::data
{

//------------------------------------------------------------------------------

const std::string transfer_function::DEFAULT_TF_NAME = "GreyLevel";

const core::com::signals::key_t transfer_function::POINTS_MODIFIED_SIG    = "pointsModified";
const core::com::signals::key_t transfer_function::WINDOWING_MODIFIED_SIG = "windowingModified";

//------------------------------------------------------------------------------

transfer_function_base::value_t transfer_function_base::map_value_to_window(value_t _value) const
{
    const min_max_t min_max        = this->min_max();
    const min_max_t window_min_max = this->window_min_max();

    const value_t scale = window() / (min_max.second - min_max.first);
    const value_t value = (_value - min_max.first) * scale + window_min_max.first;

    return value;
}

//------------------------------------------------------------------------------

transfer_function_base::value_t transfer_function_base::map_value_from_window(value_t _value) const
{
    const min_max_t min_max        = this->min_max();
    const min_max_t window_min_max = this->window_min_max();

    const value_t scale = (min_max.second - min_max.first) / window();
    const value_t value = (_value - window_min_max.first) * scale + min_max.first;

    return value;
}

//------------------------------------------------------------------------------

transfer_function_base::color_t transfer_function_base::sample_nearest(value_t _value) const
{
    return sample(_value, interpolation_mode::nearest);
}

//------------------------------------------------------------------------------

transfer_function_base::color_t transfer_function_base::sample_linear(value_t _value) const
{
    return sample(_value, interpolation_mode::linear);
}

//------------------------------------------------------------------------------

transfer_function_piece::min_max_t transfer_function_base::window_min_max() const
{
    min_max_t min_max;
    const value_t half_window = this->window() / 2.;

    const value_t level = this->level();
    min_max.first  = level - half_window;
    min_max.second = level + half_window;
    return min_max;
}

//------------------------------------------------------------------------------

void transfer_function_base::set_window_min_max(const min_max_t& _min_max)
{
    this->set_window(
        _min_max.second
        >= _min_max.first ? std::max(1., _min_max.second - _min_max.first) : std::min(
            -1.,
            _min_max.second - _min_max.first
        )
    );

    const value_t half_window = window() * 0.5;
    this->set_level(half_window + _min_max.first);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

transfer_function_piece::min_max_t transfer_function_piece::min_max() const
{
    SIGHT_ASSERT("It must have at least one value.", !this->empty());
    min_max_t min_max;
    min_max.first  = this->begin()->first;
    min_max.second = (this->rbegin())->first;
    return min_max;
}

//------------------------------------------------------------------------------

transfer_function_piece::color_t transfer_function_piece::sample(
    value_t _value,
    std::optional<enum interpolation_mode> _mode
) const
{
    SIGHT_ASSERT("It must have at least one value.", !empty());

    const value_t value = this->map_value_from_window(_value);

    const color_t black_color(0.0);

    value_t previous_value = value;
    color_t previous_color = m_clamped ? black_color : this->begin()->second;

    value_t next_value = value;
    color_t next_color = m_clamped ? black_color : this->rbegin()->second;

    color_t color(0.0);

    bool found = false;
    for(const auto& data : *this)
    {
        next_value = data.first;
        next_color = data.second;

        if(value < data.first)
        {
            found = true;
            break;
        }

        previous_value = next_value;
        previous_color = next_color;
    }

    if(!found)
    {
        next_value = value;
        next_color = m_clamped ? black_color : this->rbegin()->second;
    }

    const enum interpolation_mode mode = _mode == std::nullopt ? m_interpolation_mode : _mode.value();
    switch(mode)
    {
        case transfer_function_piece::interpolation_mode::linear:
        {
            const value_t distance    = next_value - previous_value;
            const value_t interpolant = distance < 1e-5 ? 0. : (value - previous_value) / distance;
            color = glm::mix(previous_color, next_color, interpolant);
            break;
        }

        case transfer_function_piece::interpolation_mode::nearest:
        {
            if((value - previous_value) <= (next_value - value))
            {
                color = previous_color;
            }
            else
            {
                color = next_color;
            }

            break;
        }

        default:
            SIGHT_ASSERT("Unreachable code, undefined interpolation mode", false);
    }

    return color;
}

//-----------------------------------------------------------------------------

void transfer_function_piece::set_level(transfer_function_piece::value_t _value)
{
    m_level = _value;
}

//-----------------------------------------------------------------------------

void transfer_function_piece::set_window(transfer_function_piece::value_t _value)
{
    m_window = _value;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

data::transfer_function::sptr transfer_function::create_default_tf()
{
    transfer_function::sptr tf = std::make_shared<transfer_function>();

    tf->set_name(transfer_function::DEFAULT_TF_NAME);

    auto tf_data = tf->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());
    tf_data->insert({0.0, color_t()});
    tf_data->insert({1.0, color_t(1.0, 1.0, 1.0, 1.0)});
    tf_data->set_clamped(false);
    tf_data->set_window(500.);
    tf_data->set_level(50.);

    tf->fit_window();

    return tf;
}

//------------------------------------------------------------------------------

data::transfer_function::sptr transfer_function::create_default_tf(core::type _type)
{
    transfer_function::sptr tf = std::make_shared<transfer_function>();

    tf->set_name(transfer_function::DEFAULT_TF_NAME);

    auto tf_data = tf->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());
    tf_data->insert({0.0, color_t()});
    tf_data->insert({1.0, color_t(1.0, 1.0, 1.0, 1.0)});
    tf_data->set_clamped(false);
    if(_type == core::type::INT8 || _type == core::type::UINT8)
    {
        tf_data->set_window(255.);
        tf_data->set_level(127.);
    }
    else
    {
        tf_data->set_window(500.);
        tf_data->set_level(50.);
    }

    tf->fit_window();

    return tf;
}

//------------------------------------------------------------------------------

transfer_function::transfer_function()
{
    new_signal<points_modified_signal_t>(POINTS_MODIFIED_SIG);
    new_signal<windowing_modified_signal_t>(WINDOWING_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

void transfer_function::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const transfer_function>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_name             = other->m_name;
    m_background_color = other->m_background_color;

    m_level  = other->m_level;
    m_window = other->m_window;

    m_pieces.clear();
    std::copy(other->m_pieces.cbegin(), other->m_pieces.cend(), std::back_inserter(m_pieces));

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void transfer_function::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const transfer_function>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        data::exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_name             = other->m_name;
    m_background_color = other->m_background_color;

    m_level  = other->m_level;
    m_window = other->m_window;

    m_pieces.clear();

    std::transform(
        other->m_pieces.cbegin(),
        other->m_pieces.cend(),
        std::back_inserter(this->m_pieces),
        [&](const auto& _value)
        {
            auto data = std::make_shared<transfer_function_piece>();
            *data     = *_value;
            return data;
        });

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool transfer_function::operator==(const transfer_function& _other) const noexcept
{
    if(m_name != _other.m_name
       || !core::tools::is_equal(m_background_color, _other.m_background_color)
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
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool transfer_function::operator!=(const transfer_function& _other) const noexcept
{
    return !(*this == _other);
}

//-----------------------------------------------------------------------------

void transfer_function::fit_window()
{
    min_max_t min_max {std::numeric_limits<value_t>::max(), std::numeric_limits<value_t>::lowest()};

    for(auto& piece : this->pieces())
    {
        const auto piece_min_max = piece->min_max();
        min_max.first  = std::min(piece->map_value_to_window(piece_min_max.first), min_max.first);
        min_max.second = std::max(piece->map_value_to_window(piece_min_max.second), min_max.second);
    }

    // Updates the window/level.
    m_window = min_max.second - min_max.first;
    m_level  = min_max.first + m_window * .5;
}

//------------------------------------------------------------------------------

void transfer_function::set_level(value_t _value)
{
    const double delta = _value - this->level();
    for(auto& piece : this->pieces())
    {
        piece->set_level(piece->level() + delta);
    }

    m_level = _value;
}

//-----------------------------------------------------------------------------

void transfer_function::set_window(value_t _value)
{
    SIGHT_ASSERT("Window should be non-null", std::fpclassify(_value) != FP_ZERO);

    const double scale = _value / this->window();
    for(auto& piece : this->pieces())
    {
        piece->set_window(piece->window() * scale);
    }

    m_window = _value;
}

//------------------------------------------------------------------------------

transfer_function_piece::min_max_t transfer_function::min_max() const
{
    if(empty())
    {
        return {-1., 1.};
    }

    min_max_t min_max {std::numeric_limits<value_t>::max(), std::numeric_limits<value_t>::lowest()};

    for(const auto& piece : this->pieces())
    {
        const auto piece_min_max = piece->min_max();
        min_max.first  = std::min(piece->map_value_to_window(piece_min_max.first), min_max.first);
        min_max.second = std::max(piece->map_value_to_window(piece_min_max.second), min_max.second);
    }

    return min_max;
}

//------------------------------------------------------------------------------

transfer_function_piece::color_t transfer_function::sample(
    value_t _value,
    std::optional<interpolation_mode> _mode
) const
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
