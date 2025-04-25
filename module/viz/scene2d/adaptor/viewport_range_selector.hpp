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

#include <data/image.hpp>
#include <data/transfer_function.hpp>

#include <viz/scene2d/adaptor.hpp>

namespace s2d = sight::viz::scene2d;

namespace sight::module::viz::scene2d::adaptor
{

/**
 * @brief The viewport range selector adaptor allows to select a delimited range of a viewport.
 * It uses a graphical delimiter (called shutter) that can be moved from both left to right
 * and right to left directions (in those cases, shutter's width is changing).
 *
 * Clicking onto the approximate center of the shutter allows the user to change its position,
 * according to mouse's cursor position (width won't change).
 *
 * Clicking onto the approximate left/right (respectively) border of the shutter allows the
 * user to change the width of the shutter: the right/left (respectively) border doesn't move
 * during resizing.
 *
 * Each change onto the shutter will cause this adaptor to update the managed
 * sight::viz::scene2d::data::viewport object.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="viewportRange" type="sight::module::viz::scene2d::adaptor::viewport_range_selector"
 * auto_connect="true">
       <inout key="viewport"  uid="..." />
       <inout key="selectedviewport"  uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" zValue="5" initialWidth="1000" initialPos="-100" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b viewport [sight::viz::scene2d::data::viewport]: viewport object used to display this adaptor. If the viewport
 * is not initialized, it will be updated to fit the scene size.
 * - \b selectedviewport [sight::viz::scene2d::data::viewport]: viewport object whose range is modified.
 * - \b image [sight::data::image] (optional): if specified, computes \b selectedviewport from the image range instead
 * of the
 * initialPos and initialWidth parameters.
 * * @subsection In In
 * - \b tfPool [sight::data::map]: if specified, computes \b viewport viewport from the transfer function range.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration.
 *    - \b xAxis (optional): x axis associated to the adaptor.
 *    - \b yAxis (optional): y axis associated to the adaptor.
 *    - \b zValue (optional, default="0"): z value of the layer.
 *    - \b initialPos (optional, default="0."): initial position of the shutter on the X axis.
 *    - \b initialWidth (optional, default="0"."): initial width of the shutter.
 *    - \b color (optional, default="#FFFFFF"): inner color.
 *    - \b opacity (optional, default="1.0"): opacity of the gradient.
 *
 * @pre This adaptor is intended to be used with a module::viz::scene2d::adaptor::ViewportUpdater adaptor.
 */
class viewport_range_selector : public sight::viz::scene2d::adaptor
{
public:

    SIGHT_DECLARE_SERVICE(viewport_range_selector, sight::viz::scene2d::adaptor);

    viewport_range_selector();

    /// Configures the adaptor.
    void configuring() override;

    /// Creates graphic items.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect sight::viz::scene2d::data::viewport::MODIFIED_SIG of s_VIEWPORT_INPUT to
     * module::viz::scene2d::adaptor::viewport_range_selector::service::slots::UPDATE.
     */
    connections_t auto_connections() const override;

    /// Does nothing.
    void updating() override;

    /// Does nothing.
    void stopping() override;

private:

    /**
     * @brief Filters the event to call the right methods from mouse informations.
     * @param _event the 2D scene event.
     */
    void process_interaction(sight::viz::scene2d::data::event& _event) override;

    /// Recomputes the viewport. Can signal or not the data depending from where it is called.
    void update_viewport(bool _signal_selected_viewport);

    /**
     * @brief Update the viewport object according to the current state of the shutter.
     * @param _x x position of the shutter.
     * @param _y y position of the shutter.
     * @param _width width of the shutter.
     * @param _height height of the shutter.
     */
    void update_viewport_from_shutter(double _x, double _y, double _width, double _height);

    /// Tells if the mouse cursor is at the good position to start interacting on shutter's left border.
    bool mouse_on_shutter_left(glm::dvec2 _coord);

    /// Tells if the mouse cursor is at the good position to start interacting on shutter's right border.
    bool mouse_on_shutter_right(glm::dvec2 _coord);

    /// Tells if the mouse cursor is at the good position to start interacting on shutter's middle part.
    bool mouse_on_shutter_middle(glm::dvec2 _coord);

    /// Stores the graphic item that represents the shutter.
    QGraphicsRectItem* m_shutter {nullptr};

    /// Sets if there is interaction onto shutter's left border.
    bool m_is_left_interacting {false};

    /// Sets if there is interaction onto shutter's right border.
    bool m_is_right_interacting {false};

    /// Sets if there is interaction onto the whole shutter.
    bool m_is_interacting {false};

    /// Sets if there is a dragging interaction.
    glm::dvec2 m_drag_start_point {0., 0.};

    ///  Defines the shutter position when dragging starts.
    glm::dvec2 m_drag_start_shutter_pos {0., 0.};

    /// Sets the spacing value for an easier picking onto shutter borders.
    int m_click_catch_range {1};

    /// Stores the main layer.
    QGraphicsItemGroup* m_layer {nullptr};

    /// Defines the initial position of the shutter on the X axis.
    double m_initial_x {0.F};

    /// Defines the initial width of the shutter.
    double m_initial_width {1.};

    /// Defines the color used for graphic item's.
    QPen m_color;

    /// Cache the minimum intensity found in an image
    double m_image_min {std::numeric_limits<double>::max()};

    /// Cache the maximum intensity found in an image
    double m_image_max {std::numeric_limits<double>::lowest()};

    /// Cache the minimum intensity found in an image or in the transfer function
    double m_min {std::numeric_limits<double>::max()};

    /// Cache the maximum intensity found in an image or in the transfer function
    double m_max {std::numeric_limits<double>::lowest()};

    static constexpr std::string_view VIEWPORT_INOUT          = "viewport";
    static constexpr std::string_view SELECTED_VIEWPORT_INOUT = "selectedViewport";
    static constexpr std::string_view IMAGE_INPUT             = "image";
    static constexpr std::string_view TF_INPUT                = "tf";

    data::ptr<s2d::data::viewport, sight::data::access::inout> m_viewport {this, VIEWPORT_INOUT};
    data::ptr<s2d::data::viewport, sight::data::access::inout> m_selected_viewport {this, SELECTED_VIEWPORT_INOUT};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_image {this, IMAGE_INPUT, true};
    data::ptr<sight::data::transfer_function, sight::data::access::in> m_tf {this, TF_INPUT, true};
};

} // namespace sight::module::viz::scene2d::adaptor
