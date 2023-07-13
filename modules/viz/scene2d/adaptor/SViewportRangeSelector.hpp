/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/Image.hpp>
#include <data/TransferFunction.hpp>

#include <viz/scene2d/IAdaptor.hpp>

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
 * sight::viz::scene2d::data::Viewport object.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="viewportRange" type="sight::module::viz::scene2d::adaptor::SViewportRangeSelector" autoConnect="true">
       <inout key="viewport"  uid="..." />
       <inout key="selectedviewport"  uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" zValue="5" initialWidth="1000" initialPos="-100" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b viewport [sight::viz::scene2d::data::Viewport]: viewport object used to display this adaptor. If the viewport
 * is not initialized, it will be updated to fit the scene size.
 * - \b selectedviewport [sight::viz::scene2d::data::Viewport]: viewport object whose range is modified.
 * - \b image [sight::data::Image] (optional): if specified, computes \b selectedviewport from the image range instead
 * of the
 * initialPos and initialWidth parameters.
 * * @subsection In In
 * - \b tfPool [sight::data::Composite]: if specified, computes \b viewport viewport from the transfer function range.
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
class MODULE_VIZ_SCENE2D_CLASS_API SViewportRangeSelector : public sight::viz::scene2d::IAdaptor
{
public:

    SIGHT_DECLARE_SERVICE(SViewportRangeSelector, sight::viz::scene2d::IAdaptor);

    SViewportRangeSelector();

    /// Configures the adaptor.
    void configuring() override;

    /// Creates graphic items.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG of s_VIEWPORT_INPUT to
     * module::viz::scene2d::adaptor::SViewportRangeSelector::IService::slots::s_UPDATE.
     */
    KeyConnectionsMap getAutoConnections() const override;

    /// Does nothing.
    void updating() override;

    /// Does nothing.
    void stopping() override;

private:

    /**
     * @brief Filters the event to call the right methods from mouse informations.
     * @param _event the 2D scene event.
     */
    void processInteraction(sight::viz::scene2d::data::Event& _event) override;

    /// Recomputes the viewport. Can signal or not the data depending from where it is called.
    void updateViewport(bool _signalSelectedViewport);

    /**
     * @brief Update the viewport object according to the current state of the shutter.
     * @param x x position of the shutter.
     * @param y y position of the shutter.
     * @param width width of the shutter.
     * @param height height of the shutter.
     */
    void updateViewportFromShutter(double x, double y, double width, double height);

    /// Tells if the mouse cursor is at the good position to start interacting on shutter's left border.
    bool mouseOnShutterLeft(sight::viz::scene2d::vec2d_t _coord);

    /// Tells if the mouse cursor is at the good position to start interacting on shutter's right border.
    bool mouseOnShutterRight(sight::viz::scene2d::vec2d_t _coord);

    /// Tells if the mouse cursor is at the good position to start interacting on shutter's middle part.
    bool mouseOnShutterMiddle(sight::viz::scene2d::vec2d_t _coord);

    /// Stores the graphic item that represents the shutter.
    QGraphicsRectItem* m_shutter {nullptr};

    /// Sets if there is interaction onto shutter's left border.
    bool m_isLeftInteracting {false};

    /// Sets if there is interaction onto shutter's right border.
    bool m_isRightInteracting {false};

    /// Sets if there is interaction onto the whole shutter.
    bool m_isInteracting {false};

    /// Sets if there is a dragging interaction.
    sight::viz::scene2d::vec2d_t m_dragStartPoint {0., 0.};

    ///  Defines the shutter position when dragging starts.
    sight::viz::scene2d::vec2d_t m_dragStartShutterPos {0., 0.};

    /// Sets the spacing value for an easier picking onto shutter borders.
    int m_clickCatchRange {1};

    /// Stores the main layer.
    QGraphicsItemGroup* m_layer {nullptr};

    /// Defines the initial position of the shutter on the X axis.
    double m_initialX {0.F};

    /// Defines the initial width of the shutter.
    double m_initialWidth {1.};

    /// Defines the color used for graphic item's.
    QPen m_color;

    /// Cache the minimum intensity found in an image
    double m_imageMin {std::numeric_limits<double>::max()};

    /// Cache the maximum intensity found in an image
    double m_imageMax {std::numeric_limits<double>::lowest()};

    /// Cache the minimum intensity found in an image or in the transfer function
    double m_min {std::numeric_limits<double>::max()};

    /// Cache the maximum intensity found in an image or in the transfer function
    double m_max {std::numeric_limits<double>::lowest()};

    static constexpr std::string_view s_VIEWPORT_INOUT          = "viewport";
    static constexpr std::string_view s_SELECTED_VIEWPORT_INOUT = "selectedViewport";
    static constexpr std::string_view s_IMAGE_INPUT             = "image";
    static constexpr std::string_view s_TF_INPUT                = "tf";

    data::ptr<s2d::data::Viewport, sight::data::Access::inout> m_viewport {this, s_VIEWPORT_INOUT};
    data::ptr<s2d::data::Viewport, sight::data::Access::inout> m_selectedViewport {this, s_SELECTED_VIEWPORT_INOUT, true
    };
    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_image {this, s_IMAGE_INPUT, true, true};
    data::ptr<sight::data::TransferFunction, sight::data::Access::in> m_tf {this, s_TF_INPUT, true, true};
};

} // namespace sight::module::viz::scene2d::adaptor
