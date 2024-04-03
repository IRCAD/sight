/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include <sight/viz/scene2d/config.hpp>

#include "viz/scene2d/data/axis.hpp"
#include "viz/scene2d/data/event.hpp"
#include "viz/scene2d/render.hpp"

#include <service/base.hpp>

namespace sight::viz::scene2d
{

/**
 * @brief Root class for all scene2d adaptors.
 */
class SIGHT_VIZ_SCENE2D_CLASS_API adaptor : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(adaptor, service::base);

    /// Set the zValue.
    SIGHT_VIZ_SCENE2D_API void set_z_value(float _z_value);

    /// Get the zValue.
    SIGHT_VIZ_SCENE2D_API float get_z_value() const;

    /// Get the render that manages the adaptor.
    SIGHT_VIZ_SCENE2D_API SPTR(viz::scene2d::render) get_scene_2d_render() const;

    /// Interact with the mouse events catched on the adaptor
    /// (virtual function, its behavior is only defined in the specific adaptors).
    SIGHT_VIZ_SCENE2D_API virtual void process_interaction(scene2d::data::event& _event);

protected:

    /// Constructor, set the zValue to 0.
    SIGHT_VIZ_SCENE2D_API adaptor() noexcept;

    /// Basic destructor, do nothing.
    SIGHT_VIZ_SCENE2D_API ~adaptor() noexcept override;

    /**
     * @brief Parse the xml configuration for Axis, z value and opacity
     * @code{.xml}
       <config xAxis="x" yAxis="y" zValue="1" opacity="1.0" />
       @endcode
     * @subsection Configuration Configuration:
     * - \b xAxis (optional): x axis associated to the adaptor
     * - \b yAxis (optional): y axis associated to the adaptor
     * - \b zValue (optional, default=0): z value of the layer
     * - \b opacity (optional, default=1.0): adaptor opacity (float)
     */
    SIGHT_VIZ_SCENE2D_API void configure_params();

    /// Get a pair of doubles (a point), two axis, and convert the pair of doubles values from adaptor
    /// coordinates to scene coordinates
    SIGHT_VIZ_SCENE2D_API vec2d_t map_adaptor_to_scene(const vec2d_t& _xy) const;

    /// Get a pair of doubles (a point), two axis, and convert the pair of doubles values from scene
    /// coordinates to adaptor coordinates
    SIGHT_VIZ_SCENE2D_API vec2d_t map_scene_to_adaptor(const vec2d_t& _xy) const;

    /// Return the ratio between view's initial size and its current size
    SIGHT_VIZ_SCENE2D_API double get_view_size_ratio() const;

    /// Converts a point in pixel units into a viewport coordinates
    SIGHT_VIZ_SCENE2D_API vec2d_t view_to_viewport(const scene2d::data::viewport& _viewport) const;

    /// The x Axis.
    scene2d::data::axis::sptr m_x_axis;

    /// The y Axis.
    scene2d::data::axis::sptr m_y_axis;

    /// The adaptor zValue (depth within the scene).
    /// The adaptor with the highest zValue is displayed on top of all adaptors.
    float m_z_value {0.F};

    /// Opacity of the adaptor. Default value set to 1 (opaque).
    float m_opacity {1.F};
};

//------------------------------------------------------------------------------

} // namespace sight::viz::scene2d
