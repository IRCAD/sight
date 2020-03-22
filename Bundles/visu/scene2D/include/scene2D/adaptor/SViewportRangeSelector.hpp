/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "scene2D/config.hpp"

#include <fwRenderQt/IAdaptor.hpp>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief The viewport range selector adaptor allows to select a delimited range of a viewport.
 * It uses a graphical delimiter (called shutter) that can be moved from both left to right
 * and right to left directions (in those cases, shutter's width is changing).
 *
 * Clicking onto the approximative center of the shutter allows the user to change its position,
 * according to mouse's cursor position (width won't change).
 *
 * Clicking onto the approximative left/right (respectively) border of the shutter allows the
 * user to change the width of the shutter: the right/left (respectively) border doesn't move
 * during resizing.
 *
 * Each change onto the shutter will cause this adaptor to update the managed
 * ::fwRenderQt::data::Viewport object.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="viewportRange" type="::scene2D::adaptor::SViewportRangeSelector" autoConnect="yes">
       <inout key="viewport"  uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" zValue="5" initialWidth="1000" initialPos="-100" />
   </service>
   @endcode
 *
 * @subsection In In
 * - \b viewport [::fwRenderQt::data::Viewport]: the viewport object that is updated by this adaptor.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration.
 *    - \b xAxis (optional): x axis associated to the adaptor.
 *    - \b yAxis (optional): y axis associated to the adaptor.
 *    - \b zValue (optional, default=0): z value of the layer.
 *    - \b initialPos (optional, default 0.): initial position of the shutter on the X axis.
 *    - \b initialWidth (optional, default 0.): initial width of the shutter.
 *    - \b color (optional, default black): inner color.
 *    - \b opacity (optional, default=1.0): opacity of the gradient.
 *
 * @pre This adaptor is intended to be used with a ::scene2D::adaptor::ViewportUpdater adaptor.
 */
class SCENE2D_CLASS_API SViewportRangeSelector : public ::fwRenderQt::IAdaptor
{

public:

    fwCoreServiceMacro(SViewportRangeSelector, ::fwRenderQt::IAdaptor)

    /// Creates the adaptor.
    SCENE2D_API SViewportRangeSelector() noexcept;

    /// Destroys the adaptor.
    SCENE2D_API ~SViewportRangeSelector() noexcept;

private:

    /// Configures the adaptor
    void configuring() override;

    /// Creates graphic items.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwRenderQt::data::Viewport::s_MODIFIED_SIG of s_VIEWPORT_INPUT to
     * ::scene2D::adaptor::SViewportRangeSelector::s_UPDATE_SLOT.
     */
    KeyConnectionsMap getAutoConnections() const override;

    /// Does nothing.
    void updating() override;

    /// Does nothing.
    void stopping() override;

    /**
     * @brief Filters the event to call the right methods from mouse informations.
     * @param _event the 2D scene event.
     */
    void processInteraction(::fwRenderQt::data::Event& _event ) override;

    /**
     * @brief Update the viewport object according to the current state of the shutter.
     * @param x x position of the shutter.
     * @param y y position of the shutter.
     * @param width width of the shutter.
     * @param height height of the shutter.
     */
    void updateViewportFromShutter(double x, double y, double width, double height);

    /// Tells if the mouse cursor is at the good position to start interacting on shutter's left border.
    bool mouseOnShutterLeft(::fwRenderQt::data::Coord _coord);

    /// Tells if the mouse cursor is at the good position to start interacting on shutter's right border
    bool mouseOnShutterRight(::fwRenderQt::data::Coord _coord);

    /// Tells if the mouse cursor is at the good position to start interacting on shutter's middle part
    bool mouseOnShutterMiddle(::fwRenderQt::data::Coord _coord);

    /// Stores the graphic item that represents the shutter.
    QGraphicsRectItem* m_shutter;

    /// Sets if there is interaction onto shutter's left border.
    bool m_isLeftInteracting;

    /// Sets if there is interaction onto shutter's right border.
    bool m_isRightInteracting;

    /// Sets if there is interaction onto the whole shutter.
    bool m_isInteracting;

    /// Sets if there is a dragging interaction.
    ::fwRenderQt::data::Coord m_dragStartPoint;

    ///  Defines the shutter position when dragging starts.
    ::fwRenderQt::data::Coord m_dragStartShutterPos;

    /// Sets the spacing value for an easier picking onto shutter borders.
    const int m_clickCatchRange;

    /// Stores the main layer.
    QGraphicsItemGroup* m_layer;

    /// Defines the initial position of the shutter on the X axis.
    float m_initialX;

    /// Defines the initial width of the shutter.
    float m_initialWidth;

    /// Defines the color used for graphic item's.
    QPen m_color;
};

}   // namespace adaptor
}   // namespace scene2D
