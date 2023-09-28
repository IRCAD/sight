/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "viz/scene2d/config.hpp"
#include "viz/scene2d/data/Axis.hpp"
#include "viz/scene2d/data/Event.hpp"
#include "viz/scene2d/SRender.hpp"

#include <service/base.hpp>

namespace sight::viz::scene2d
{

/**
 * @brief Root class for all scene2d adaptors.
 */
class VIZ_SCENE2D_CLASS_API adaptor : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(adaptor, service::base);

    /// Set the zValue.
    VIZ_SCENE2D_API void setZValue(float _zValue);

    /// Get the zValue.
    VIZ_SCENE2D_API float getZValue() const;

    /// Get the render that manages the adaptor.
    VIZ_SCENE2D_API SPTR(viz::scene2d::SRender) getScene2DRender() const;

    /// Interact with the mouse events catched on the adaptor
    /// (virtual function, its behavior is only defined in the specific adaptors).
    VIZ_SCENE2D_API virtual void processInteraction(scene2d::data::Event& _event);

protected:

    /// Constructor, set the zValue to 0.
    VIZ_SCENE2D_API adaptor() noexcept;

    /// Basic destructor, do nothing.
    VIZ_SCENE2D_API ~adaptor() noexcept override;

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
    VIZ_SCENE2D_API void configureParams();

    /// Get a pair of doubles (a point), two axis, and convert the pair of doubles values from adaptor
    /// coordinates to scene coordinates
    VIZ_SCENE2D_API vec2d_t mapAdaptorToScene(const vec2d_t& _xy) const;

    /// Get a pair of doubles (a point), two axis, and convert the pair of doubles values from scene
    /// coordinates to adaptor coordinates
    VIZ_SCENE2D_API vec2d_t mapSceneToAdaptor(const vec2d_t& _xy) const;

    /// Return the ratio between view's initial size and its current size
    VIZ_SCENE2D_API double getViewSizeRatio() const;

    /// Converts a point in pixel units into a viewport coordinates
    VIZ_SCENE2D_API vec2d_t viewToViewport(const scene2d::data::Viewport& viewport) const;

    /// The x Axis.
    scene2d::data::Axis::sptr m_xAxis;

    /// The y Axis.
    scene2d::data::Axis::sptr m_yAxis;

    /// The adaptor zValue (depth within the scene).
    /// The adaptor with the highest zValue is displayed on top of all adaptors.
    float m_zValue {0.F};

    /// Opacity of the adaptor. Default value set to 1 (opaque).
    float m_opacity {1.F};
};

//------------------------------------------------------------------------------

} // namespace sight::viz::scene2d
