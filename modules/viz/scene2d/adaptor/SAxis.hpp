/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "modules/viz/scene2d/config.hpp"

#include <viz/scene2d/IAdaptor.hpp>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

/**
 * @brief Render an axis on the scene2d.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="axis" type="sight::module::viz::scene2d::adaptor::SAxis" autoConnect="yes">
      <in key="viewport" uid="..." />
      <config xAxis="xAxis" yAxis="yAxis" zValue="4" color="cyan" align="left" min="0" max="1" step="0.1" />
   </service>
   @endcode
 *
 * @subsection In In
 * - \b viewport [sight::viz::scene2d::data::Viewport]: object listened to update axis.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b align (mandatory): axis alignment, left', 'right', 'top' or 'bottom'
 *    - \b min (mandatory): min axis bound
 *    - \b max (mandatory): max axis bound
 *    - \b tickSize (optional, default=1.0): axis tick size
 *    - \b interval (optional, default=1.0): axis interval
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b opacity (optional, default=1.0): adaptor opacity (float)
 *    - \b color (optional, default black): color of the axis
 */
class MODULE_VIZ_SCENE2D_CLASS_API SAxis : public sight::viz::scene2d::IAdaptor
{

public:
    SIGHT_DECLARE_SERVICE(SAxis, sight::viz::scene2d::IAdaptor)

    MODULE_VIZ_SCENE2D_API SAxis() noexcept;
    MODULE_VIZ_SCENE2D_API virtual ~SAxis() noexcept;

protected:

    void starting() override;
    void stopping() override;
    void updating() override;
    void configuring() override;

    /// Manage the given events
    MODULE_VIZ_SCENE2D_API void processInteraction( sight::viz::scene2d::data::Event& _event ) override;

    MODULE_VIZ_SCENE2D_API KeyConnectionsMap getAutoConnections() const override;
private:

    /// Builds axis graphic items.
    void buildAxis();

    ///
    double getStartVal();

    ///
    double getEndVal();

    // Specify where the axis must be aligned: left, right, top or bottom.
    // Left and right side axis are aligned/floating relatively to the view.
    std::string m_align;

    /// The required interval between two consecutive values of the axis.
    float m_interval{1.f};

    /// Minimal value of the axis.
    float m_min{0.f};

    /// Maximal value of the axis.
    float m_max{0.f};

    /// Size of a tick.
    float m_tickSize{1.0f};

    /// Color.
    QPen m_color{Qt::white};

    /// Tells if the line of the axis must be displayed in addition to ticks.
    bool m_showLine{true};

    /// A layer that gathers all the graphic items.
    QGraphicsItemGroup* m_layer{nullptr};

    /// The line of the axis.
    QGraphicsLineItem* m_line{nullptr};

    /// The graphic items that refer to ticks of the axis.
    std::vector< QGraphicsLineItem* > m_ticks;
};

}   // namespace adaptor
}   // namespace sight::module::viz::scene2d
