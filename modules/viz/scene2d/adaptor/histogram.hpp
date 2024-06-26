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

#include <data/helper/histogram.hpp>
#include <data/image.hpp>

#include <viz/scene2d/adaptor.hpp>

namespace sight::module::viz::scene2d::adaptor
{

/**
 * @brief   adaptor implementation for histogram data.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="histogram" type="sight::module::viz::scene2d::adaptor::histogram" auto_connect="true">
       <in key="image" uid="..." />
       <inout key="point" uid="..." />
       <config xAxis="xAxis" yAxis="axeHistogramY" color="gray" cursor="true" cursorColor="#E5FFFFFF" zValue="5"/>
   </service>
   @endcode
 *
 * @subsection In In
 * - \b histogram [sight::data::image]: source image.
 *
 * @subsection In-Out In-Out
 * - \b point [sight::data::point](optional): histogram point, used to show information at the current histogram index
 * pointed by the mouse.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b color (optional, default black) : the background color of the histogram
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b cursor (optional): enable the display of a cursor shown on mouse hover
 *      - \b color (optional, default black): inner color of the cursor shown on mouse hover
 *      - \b borderColor (optional, default black): border color of the cursor shown on mouse hover
 *      - \b size (optional, default 6.0): cursor size
 *      - \b labelColor (optional, default="#FFFFFFFF"): color of the cursor label
 *      - \b fontSize (optional, default="8"): size of the font used to display the current cursor value.
 */
class histogram : public sight::viz::scene2d::adaptor
{
public:

    SIGHT_DECLARE_SERVICE(histogram, sight::viz::scene2d::adaptor);

    histogram() noexcept;
    ~histogram() noexcept override;

protected:

    void configuring() override;
    void starting() override;
    void updating() override;
    void stopping() override;

    void process_interaction(sight::viz::scene2d::data::event& _event) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect histogram::MODIFIED_SIG to this::service::slots::UPDATE
     */
    service::connections_t auto_connections() const override;

    /// Ratio used for vertical scaling
    static constexpr double SCALE      = 1.1;
    static constexpr double FAST_SCALE = 2.2;

private:

    /// Update the value of m_ordinateValueUID according to the value pointed by mouse cursor.
    void update_current_point(sight::viz::scene2d::data::event& _event);

    /// Update image related properties when it changes
    void on_image_change();

    /// Color used for graphic item's inner and border color
    QPen m_color {Qt::green};

    /// Opacity
    double m_opacity {0.8};

    /// Current vertical scaling ratio
    double m_scale {1.};

    /// Width of histogram bins
    std::size_t m_histogram_bins_width {5};

    // layer for the histogram, may be rescaled on mouse wheel event
    QGraphicsItemGroup* m_layer {nullptr};

    // layer for the cursor, never rescaled
    QGraphicsItemGroup* m_cursor_layer {nullptr};

    /// Enables the display of a cursor
    bool m_cursor_enabled {false};

    /// Color used for the inner color of the cursor shown on mouse hover
    QPen m_cursor_color;

    /// Color used for the border color of the cursor shown on mouse hover
    QPen m_cursor_border_color;

    /// Point size of the cursor shown on mouse hover
    double m_cursor_size {6.F};

    /// Defines the color used for graphic item's inner color.
    QPen m_cursor_label_color;

    /// Stores the item which display the current values of the associated histogram pointed by this cursor.
    QGraphicsSimpleTextItem* m_cursor_label {nullptr};

    /// Defines the size of the font used for rendering the current value of this tracker.
    int m_font_size {8};

    /// Sets the display status.
    bool m_is_interacting {false};

    /// True when the mouse is hover the widget.
    bool m_entered {false};

    // A graphics item that is located onto histogram's upper border and moves along this border
    // according to the position of mouse's cursor. The goal of this graphical index is to show
    // the associated value within the histogram pointed buy this index.
    QGraphicsEllipseItem* m_cursor_item {nullptr};

    /// Helper to compute the image histogram
    std::unique_ptr<data::helper::histogram> m_histogram;

    static constexpr std::string_view IMAGE_INPUT    = "image";
    static constexpr std::string_view VIEWPORT_INOUT = "viewport";

    data::ptr<sight::data::image, sight::data::access::in> m_image {this, IMAGE_INPUT};
    data::ptr<sight::viz::scene2d::data::viewport, sight::data::access::inout> m_viewport {this, VIEWPORT_INOUT};
};

} // namespace sight::module::viz::scene2d::adaptor
