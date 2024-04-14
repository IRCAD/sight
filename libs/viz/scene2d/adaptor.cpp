/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "adaptor.hpp"

#include "viz/scene2d/registry/adaptor.hpp"

#include "viz/scene2d/graphics_view.hpp"

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <QGraphicsItemGroup>

#include <cmath>

namespace sight::viz::scene2d
{

adaptor::adaptor() noexcept =
    default;

//-----------------------------------------------------------------------------

adaptor::~adaptor() noexcept =
    default;

//-----------------------------------------------------------------------------

void adaptor::set_z_value(float _z_value)
{
    m_z_value = _z_value;
}

//-----------------------------------------------------------------------------

float adaptor::get_z_value() const
{
    return m_z_value;
}

//-----------------------------------------------------------------------------

viz::scene2d::render::sptr adaptor::get_scene_2d_render() const
{
    const auto& registry = viz::scene2d::registry::get_adaptor_registry();
    const auto& iter     = registry.find(this->get_id());
    SIGHT_ASSERT("adaptor " + this->get_id() + " not registered", iter != registry.end());

    viz::scene2d::render::sptr render =
        std::dynamic_pointer_cast<viz::scene2d::render>(core::tools::id::get_object(iter->second));
    SIGHT_ASSERT("Service render " + iter->second + " not instanced", render);
    return render;
}

//-----------------------------------------------------------------------------

double adaptor::get_view_size_ratio() const
{
    SIGHT_ASSERT("Height should be greater than 0.", this->get_scene_2d_render()->get_view()->height() > 0);

    return static_cast<double>(this->get_scene_2d_render()->get_view()->width())
           / static_cast<double>(this->get_scene_2d_render()->get_view()->height());
}

//-----------------------------------------------------------------------------

vec2d_t adaptor::view_to_viewport(const scene2d::data::viewport& _viewport) const
{
    auto* view = this->get_scene_2d_render()->get_view();

    const double viewport_height = _viewport.height();
    const double viewport_width  = _viewport.width();

    const double viewport_view_ratio = viewport_width / view->width();
    const double viewport_size_ratio = viewport_height / viewport_width;
    const double view_size_ratio     = static_cast<double>(view->width())
                                       / static_cast<double>(view->height());

    return {viewport_view_ratio, viewport_size_ratio* view_size_ratio* viewport_view_ratio};
}

//-----------------------------------------------------------------------------

vec2d_t adaptor::map_adaptor_to_scene(
    const vec2d_t& _xy
) const
{
    double x = NAN;
    double y = NAN;

    if(m_x_axis->get_scale_type() == scene2d::data::axis::log)
    {
        // Logarithm 10 cannot get negative values
        if(_xy.x <= 0.)
        {
            x = 0.;
        }
        else
        {
            // Apply the x scale and the log to the x value
            x = m_x_axis->get_scale() * log10(_xy.x);
        }
    }
    else
    {
        // Apply just the x scale to the x value
        x = m_x_axis->get_scale() * _xy.x;
    }

    if(m_y_axis->get_scale_type() == scene2d::data::axis::log)
    {
        // Logarithm 10 cannot get negative values
        if(_xy.y <= 0.)
        {
            y = 0.;
        }
        else
        {
            // Apply the y scale and the log to the y value
            y = m_y_axis->get_scale() * log10(_xy.y);
        }
    }
    else
    {
        // Apply just the y scale to the y value
        y = m_y_axis->get_scale() * _xy.y;
    }

    return {x, y};
}

//-----------------------------------------------------------------------------

vec2d_t adaptor::map_scene_to_adaptor(const vec2d_t& _xy) const
{
    // Do the reverse operation of the mapAdaptorToScene function
    double x = NAN;
    double y = NAN;
    if(m_x_axis->get_scale_type() == scene2d::data::axis::log)
    {
        x = 10. * std::exp(_xy.x) / m_x_axis->get_scale();
    }
    else
    {
        x = (_xy.x) / m_x_axis->get_scale();
    }

    if(m_y_axis->get_scale_type() == scene2d::data::axis::log)
    {
        y = 10. * std::exp(_xy.y) / m_y_axis->get_scale();
    }
    else
    {
        y = _xy.y / m_y_axis->get_scale();
    }

    return {x, y};
}

//-----------------------------------------------------------------------------

void adaptor::configure_params()
{
    const config_t config = this->get_config().get_child("config.<xmlattr>");

    // If the corresponding attributes are present in the config, set the xAxis, yAxis and the adaptor zValue
    if(config.count("xAxis") != 0U)
    {
        m_x_axis = this->get_scene_2d_render()->get_axis(config.get<std::string>("xAxis"));
        SIGHT_ASSERT("xAxis not found", m_x_axis);
    }
    else
    {
        m_x_axis = std::make_shared<scene2d::data::axis>();
    }

    if(config.count("yAxis") != 0U)
    {
        m_y_axis = this->get_scene_2d_render()->get_axis(config.get<std::string>("yAxis"));
        SIGHT_ASSERT("yAxis not found", m_x_axis);
    }
    else
    {
        m_y_axis = std::make_shared<scene2d::data::axis>();
    }

    if(config.count("zValue") != 0U)
    {
        m_z_value = config.get<float>("zValue");
    }

    if(config.count("opacity") != 0U)
    {
        m_opacity = config.get<float>("opacity");
    }
}

//-----------------------------------------------------------------------------

void adaptor::process_interaction(scene2d::data::event& /*unused*/)
{
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d
