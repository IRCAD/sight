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

#include <viz/scene2d/IAdaptor.hpp>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

/**
 * @brief Render grid on the scene2d.
 *
 * @section Slots Slots
 * - \b setGridSpacing(double, double, std::string) : set the grid spacing in x and y. It is intended to be connected to
 * a double2Changed signal of a SParameters service; the string key parameter should be set to "spacing".
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="grid1" type="sight::module::viz::scene2d::adaptor::SGrid2D">
    <config xMin="-1100" xMax="750" yMin="-0.7" yMax="1.7" xSpacing="100" ySpacing="0.1"
        color="lightGray" xAxis="xAxis" yAxis="yAxis" zValue="1"/>
   </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b opacity (optional, default=1.0): adaptor opacity (float)
 *    - \b color (optional, default black): color of the axis
 *    - \b xMin (optional): Set the minimum x value of the grid.
 *    - \b xMax (optional): Set the maximum x value of the grid.
 *    - \b yMin (optional): Set the minimum y value of the grid.
 *    - \b yMax (optional): Set the maximum y value of the grid.
 *    - \b xSpacing (optional, default value: 10): Set the grid spacing (space between 2 consecutive lines) in x.
 *    - \b ySpacing (optional, default value: 10): Set the grid spacing (space between 2 consecutive lines) in y.
 */
class MODULE_VIZ_SCENE2D_CLASS_API SGrid2D : public sight::viz::scene2d::IAdaptor
{
public:

    SIGHT_DECLARE_SERVICE(SGrid2D, ::sight::viz::scene2d::IAdaptor);

    /**
     * @name Slots API
     * @{
     */
    MODULE_VIZ_SCENE2D_API static const core::com::Slots::SlotKeyType s_SET_GRID_SPACING_SLOT;
    /** @} */

    /// Constructor, set the x and y spacing to 10
    MODULE_VIZ_SCENE2D_API SGrid2D() noexcept;

    /// Basic destructor, do nothing
    MODULE_VIZ_SCENE2D_API virtual ~SGrid2D() noexcept;

protected:

    MODULE_VIZ_SCENE2D_API void configuring() override;

    /// Initialize the layer, set the pen style to DashLine and call the draw() function.
    MODULE_VIZ_SCENE2D_API void starting() override;

    MODULE_VIZ_SCENE2D_API void updating() override;

    /// Clean the lines vector and remove the layer from the scene.
    MODULE_VIZ_SCENE2D_API void stopping() override;

    /// Manage the given events
    MODULE_VIZ_SCENE2D_API void processInteraction(sight::viz::scene2d::data::Event& _event) override;

private:

    /// Calculate the x/y start/end values, create the lines, set'em m_pen, push'em back in
    /// the m_lines vector, add'em to the layer, set the layer position and zValue and add it
    /// to the scene.
    void draw();

    /// Returns x-axis starting value
    float getXStartVal();

    /// Returns x-axis ending value
    float getXEndVal();

    /// Returns y-axis starting value
    float getYStartVal();

    /// Returns y-axis ending value
    float getYEndVal();

    /// Slot: This method is used to set the grid spacing from two double parameters.
    void setGridSpacing(double _x, double _y, std::string _key);

    /// Bounds of the grid and spacing values for each axis.
    float m_xMin, m_xMax, m_yMin, m_yMax, m_xSpacing, m_ySpacing;

    /// The pen.
    QPen m_pen;

    /// A vector containing QGraphicsItems representing the lines of the grid.
    std::vector<QGraphicsItem*> m_lines;

    /// The layer.
    QGraphicsItemGroup* m_layer;
};

} // namespace adaptor

} // namespace sight::module::viz::scene2d
