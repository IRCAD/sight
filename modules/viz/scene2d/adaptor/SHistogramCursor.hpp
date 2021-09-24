/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/viz/scene2d/config.hpp"

#include <data/Histogram.hpp>
#include <data/Point.hpp>

#include <viz/scene2d/data/Viewport.hpp>
#include <viz/scene2d/IAdaptor.hpp>

#include <QGraphicsTextItem>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

/**
 * @brief IAdaptor implementation to show a cursor on histogram pointed by mouse.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="histogram" type="sight::module::viz::scene2d::adaptor::SHistogramCursor" autoConnect="true">
       <in key="histogram" uid="..." />
       <in key="point"     uid="..." />
       <in key="viewport"  uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" color="gray" opacity="0.25" zValue="5"/>
   </service>
   @endcode
 *
 * @subsection In In
 * - \b histogram [sight::data::Histogram]: histogram to display.
 * - \b point [sight::data::Point]: histogram point, used to show information at the current histogram index pointed by
 * the
 * mouse.
 * - \b viewport [sight::viz::scene2d::data::Viewport]: object listened to update adaptor.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b opacity (optional, default=1.0): adaptor opacity (float)
 *    - \b color (optional, default black): inner color
 *    - \b borderColor (optional, default black): border color
 *    - \b pointSize (optional, default 6.0): point size
 *
 */
class SHistogramCursor : public sight::viz::scene2d::IAdaptor
{
public:

    SIGHT_DECLARE_SERVICE(SHistogramCursor, sight::viz::scene2d::IAdaptor);

    MODULE_VIZ_SCENE2D_API SHistogramCursor() noexcept;
    MODULE_VIZ_SCENE2D_API virtual ~SHistogramCursor() noexcept;

    MODULE_VIZ_SCENE2D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    MODULE_VIZ_SCENE2D_API void configuring() override;
    MODULE_VIZ_SCENE2D_API void starting() override;
    MODULE_VIZ_SCENE2D_API void updating() override;
    MODULE_VIZ_SCENE2D_API void stopping() override;

    MODULE_VIZ_SCENE2D_API void processInteraction(sight::viz::scene2d::data::Event& _event) override;

    /// Color used for graphic item's inner color
    QPen m_color;

    /// Color used for graphic item's border color
    QPen m_borderColor;

    // A graphics item that is located onto hsitogram's upper border and moves along this border
    // according to the position of mouse's cursor. The goal of this graphical index is to show
    // the associated value within the histogram pointed buy this index.
    QGraphicsEllipseItem* m_index;

    /// Coordinates of the current event.
    sight::viz::scene2d::data::Coord m_coord;

    /// Point size
    float m_pointSize;

    /// The layer.
    QGraphicsItemGroup* m_layer;

    static constexpr std::string_view s_HISTOGRAM_INPUT = "histogram";
    static constexpr std::string_view s_VIEWPORT_INPUT  = "viewport";

    sight::data::ptr<sight::data::Histogram, sight::data::Access::in> m_histogram {this, s_HISTOGRAM_INPUT};
    sight::data::ptr<sight::data::Point, sight::data::Access::inout> m_point {this, "point"};
};

} // namespace adaptor

} // namespace sight::module::viz::scene2d
