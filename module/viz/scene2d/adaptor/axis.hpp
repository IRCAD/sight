/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <viz/scene2d/adaptor.hpp>

namespace sight::module::viz::scene2d::adaptor
{

/**
 * @brief Render an axis on the scene2d.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="axis" type="sight::module::viz::scene2d::adaptor::axis" auto_connect="true">
      <in key="viewport" uid="..." />
      <config xAxis="xAxis" yAxis="yAxis" zValue="4" color="cyan" align="left" min="0" max="1" step="0.1" />
   </service>
   @endcode
 *
 * @subsection In In
 * - \b viewport [sight::viz::scene2d::data::viewport]: object listened to update axis.
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
class axis : public sight::viz::scene2d::adaptor
{
public:

    SIGHT_DECLARE_SERVICE(axis, sight::viz::scene2d::adaptor);

    axis() noexcept;
    ~axis() noexcept override;

protected:

    void starting() override;
    void stopping() override;
    void updating() override;
    void configuring() override;

    /// Manage the given events
    void process_interaction(sight::viz::scene2d::data::event& _event) override;

    connections_t auto_connections() const override;

private:

    /// Builds axis graphic items.
    void build_axis();

    /// Builds labels graphic items.
    void build_labels();

    ///
    double get_start_val() const;

    ///
    double get_end_val() const;

    /// Displays axis graphic items.
    void update_axis();

    /// Displays labels graphic items.
    void update_labels();

    // Specify where the axis must be aligned: left, right, top or bottom.
    // Left and right side axis are aligned/floating relatively to the view.
    std::string m_align;

    /// A layer that gathers all the graphic items.
    QGraphicsItemGroup* m_layer {nullptr};

    /// Minimal value of the axis.
    double m_min {0.};

    /// Maximal value of the axis.
    double m_max {0.};

    /// The required interval between two consecutive values of the axis.
    double m_interval {1.};

    struct line_t
    {
        /// Size of a tick.
        double tick_size {1.0};

        /// Color.
        QPen color {Qt::white};

        /// The line of the axis.
        QGraphicsLineItem* line {nullptr};

        /// The graphic items that refer to ticks of the axis.
        std::vector<QGraphicsLineItem*> ticks;
    };
    line_t m_line;

    struct labels
    {
        /// The unit of this values as text.
        QGraphicsSimpleTextItem* unit {nullptr};

        /// Tells if the unit of the axis must be displayed.
        bool show_unit {true};

        /// The unit  of the axis (as text) that will be displayed.
        std::string displayed_unit;

        /// Specify where the axis must be aligned: left, right, top or bottom.
        /// Left and right side axis are aligned/floating relatively to the view.
        std::string align;

        /// A vector containing QGraphicsItem-s representing the scale values of the axis.
        std::vector<QGraphicsItem*> values;

        /// The font applied to grid's values.
        QFont font;

        /// The color applied to grid's values.
        QBrush brush;

        /// The pen.
        QPen pen {Qt::white};

        /// The step which defines when a value of the axis should be displayed if there is not
        /// enough space to display all the values.
        int step {1};

        /// Size of the font used for rendering grid values.
        int font_size {8};

        /// Extra scale factor to force a minimum size of the font - platform dependent (set in the constructor)
        double extra_scale {10.};
    };

    labels m_labels;

    static constexpr std::string_view VIEWPORT_INPUT = "viewport";
    data::ptr<sight::viz::scene2d::data::viewport, sight::data::access::in> m_viewport {this, VIEWPORT_INPUT};
};

} // namespace sight::module::viz::scene2d::adaptor
