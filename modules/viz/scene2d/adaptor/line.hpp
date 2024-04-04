/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <viz/scene2d/adaptor.hpp>

namespace sight::module::viz::scene2d::adaptor
{

/**
 * @brief Line adaptor. Draw a line on the scene2D
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service id="abscissa" class="sight::module::viz::scene2d::adaptor::line" >
    <config x1="-500" x2="500" y1="0" y2="0" color="white" xAxis="xAxis" yAxis="yAxis" zValue="2" />
   </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b x1 (mandatory): Set the line beginning coordinate x value.
 *    - \b x2 (mandatory): Set the line ending coordinate x value.
 *    - \b y1 (mandatory): Set the line beginning coordinate y value.
 *    - \b y2 (mandatory): Set the line ending coordinate y value.
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b color (optional, default black): color of the line
 */
class line : public sight::viz::scene2d::adaptor
{
public:

    SIGHT_DECLARE_SERVICE(line, sight::viz::scene2d::adaptor);

    /// Basic constructor, do nothing.
    line() noexcept;

    /// Basic destructor, do nothing.
    ~line() noexcept override;

protected:

    void configuring() override;

    /// Initialize the layer and call the draw() function.
    void starting() override;

    /// Do nothing.
    void updating() override;

    /// Remove the layer from the scene.
    void stopping() override;

private:

    /// Create the line, set it m_pen, add it to the layer, set the layer position and zValue and add it to the scene.
    void draw();

    /// The coordinates of the line.
    float m_x1 {0.F}, m_x2 {0.F}, m_y1 {0.F}, m_y2 {0.F};

    /// The pen.
    QPen m_pen;

    /// The layer.
    QGraphicsItemGroup* m_layer {nullptr};
};

} // namespace sight::module::viz::scene2d::adaptor
